package auth

import (
	"encoding/json"
	"net/http"
)

func (svc *Service) getLoginHandler() http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		defer r.Body.Close()
		var req LoginRequest
		decoder := json.NewDecoder(r.Body)
		decoder.DisallowUnknownFields()

		if r.Header.Get("Content-Type") != "application/json" {
			http.Error(w, "unsupported Content-Type (must be application/json)", http.StatusUnsupportedMediaType)
			return
		}

		if err := decoder.Decode(&req); err != nil {
			http.Error(w, "bad Request", http.StatusBadRequest)
			return
		}

		resp, err := svc.Login(r.Context(), &req)
		if err != nil {
			http.Error(w, "unauthorized", http.StatusUnauthorized)
			return
		}

		w.Header().Set("Content-Type", "application/json")
		encoder := json.NewEncoder(w)

		if err = encoder.Encode(LoginResponse{
			Token: resp.Token,
		}); err != nil {
			http.Error(w, ErrInternalServerError.Error(), http.StatusInternalServerError)
			return
		}
	}
}

func getProfileHandler() http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {

	}
}
