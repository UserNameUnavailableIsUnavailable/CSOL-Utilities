package handler

import (
	"encoding/json"
	"log/slog"
	"net/http"

	"github.com/google/uuid"
	"github.com/gorilla/websocket"
	"git.macrohard.fun/root/csol-utilities/server/internal/auth"
	"git.macrohard.fun/root/csol-utilities/server/internal/hub"
)

var upgrader = websocket.Upgrader{
	CheckOrigin: func(r *http.Request) bool { return true }, // TODO: restrict in production
}

// Register wires all HTTP routes onto the given mux.
func Register(mux *http.ServeMux, authSvc *auth.Service, wsHub *hub.Hub) {
	// Public
	mux.HandleFunc("POST /api/login", loginHandler(authSvc))

	// Protected (requires JWT)
	protected := auth.Middleware(authSvc)
	mux.Handle("GET /api/ws", protected(http.HandlerFunc(wsHandler(wsHub))))
}

// ── Login ────────────────────────────────────────────────────────────────────

type loginRequest struct {
	Username string `json:"username"`
	Password string `json:"password"`
}

type loginResponse struct {
	Token string `json:"token"`
}

func loginHandler(svc *auth.Service) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		var req loginRequest
		if err := json.NewDecoder(r.Body).Decode(&req); err != nil {
			http.Error(w, "bad request", http.StatusBadRequest)
			return
		}

		token, err := svc.Login(req.Username, req.Password)
		if err != nil {
			http.Error(w, "unauthorized", http.StatusUnauthorized)
			return
		}

		w.Header().Set("Content-Type", "application/json")
		json.NewEncoder(w).Encode(loginResponse{Token: token})
	}
}

// ── WebSocket ────────────────────────────────────────────────────────────────

func wsHandler(wsHub *hub.Hub) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		conn, err := upgrader.Upgrade(w, r, nil)
		if err != nil {
			slog.Error("websocket upgrade failed", "error", err)
			return
		}

		client := &hub.Client{
			ID:   uuid.NewString(),
			Send: make(chan []byte, 256),
		}
		wsHub.Register(client)

		go writePump(conn, client, wsHub)
		readPump(conn, client, wsHub)
	}
}

// readPump reads messages from the WebSocket connection and broadcasts them.
func readPump(conn *websocket.Conn, client *hub.Client, wsHub *hub.Hub) {
	defer func() {
		wsHub.Unregister(client)
		conn.Close()
	}()

	for {
		_, message, err := conn.ReadMessage()
		if err != nil {
			if websocket.IsUnexpectedCloseError(err, websocket.CloseGoingAway, websocket.CloseNormalClosure) {
				slog.Warn("websocket read error", "error", err)
			}
			break
		}
		wsHub.Broadcast(hub.Message{
			Type:    hub.MsgFromManipulator,
			Payload: message,
		})
	}
}

// writePump sends queued messages to the WebSocket connection.
func writePump(conn *websocket.Conn, client *hub.Client, wsHub *hub.Hub) {
	defer conn.Close()

	for msg := range client.Send {
		if err := conn.WriteMessage(websocket.TextMessage, msg); err != nil {
			slog.Warn("websocket write error", "error", err)
			return
		}
	}
}
