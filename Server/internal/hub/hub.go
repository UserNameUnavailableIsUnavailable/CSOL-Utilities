package hub

import (
	"log/slog"
	"sync"
)

// MessageType identifies the kind of participant sending a message.
type MessageType int

const (
	MsgFromManipulator MessageType = iota // Web UI → Controller
	MsgFromController                     // Controller → Web UI
)

// Message is a payload flowing through the hub.
type Message struct {
	Type    MessageType
	Payload []byte
}

// Client represents a single WebSocket connection.
type Client struct {
	ID   string
	Send chan []byte
}

// Hub maintains connected clients and broadcasts messages.
type Hub struct {
	mu          sync.RWMutex
	clients     map[string]*Client
	broadcast   chan Message
	register    chan *Client
	unregister  chan *Client
}

// New creates an idle Hub; call Run() in a goroutine to start it.
func New() *Hub {
	return &Hub{
		clients:    make(map[string]*Client),
		broadcast:  make(chan Message, 256),
		register:   make(chan *Client),
		unregister: make(chan *Client),
	}
}

// Run is the main event loop — must be launched as a goroutine.
func (h *Hub) Run() {
	for {
		select {
		case c := <-h.register:
			h.mu.Lock()
			h.clients[c.ID] = c
			h.mu.Unlock()
			slog.Info("client registered", "id", c.ID)

		case c := <-h.unregister:
			h.mu.Lock()
			if _, ok := h.clients[c.ID]; ok {
				close(c.Send)
				delete(h.clients, c.ID)
			}
			h.mu.Unlock()
			slog.Info("client unregistered", "id", c.ID)

		case msg := <-h.broadcast:
			h.mu.RLock()
			for _, c := range h.clients {
				select {
				case c.Send <- msg.Payload:
				default:
					// drop slow client
					slog.Warn("dropping message for slow client", "id", c.ID)
				}
			}
			h.mu.RUnlock()
		}
	}
}

// Register adds a client to the hub.
func (h *Hub) Register(c *Client) { h.register <- c }

// Unregister removes a client from the hub.
func (h *Hub) Unregister(c *Client) { h.unregister <- c }

// Broadcast sends a message to every connected client.
func (h *Hub) Broadcast(msg Message) { h.broadcast <- msg }

// Send delivers a payload to a specific client by ID.
func (h *Hub) Send(clientID string, data []byte) bool {
	h.mu.RLock()
	defer h.mu.RUnlock()
	c, ok := h.clients[clientID]
	if !ok {
		return false
	}
	select {
	case c.Send <- data:
		return true
	default:
		return false
	}
}
