package auth

import (
	"fmt"
	"net/http"
	"time"

	"git.macrohard.fun/root/csol-utilities/Scheduler/config"
	"git.macrohard.fun/root/csol-utilities/Scheduler/internal/store"
)

// Service provides authentication services
type Service struct {
	users  store.UserStore
	secret []byte
	expiry time.Duration
}

// NewService creates a new authentication service
func NewService(users *store.UserStore, authCfg *config.Auth) *Service {
	return &Service{
		users:  *users,
		secret: []byte(authCfg.Secret),
		expiry: authCfg.Expiry,
	}
}

func (svc *Service) Install(mux *http.ServeMux, routes *config.Routes) {
	apiBase := fmt.Sprintf("POST /api%s", routes.Auth)
	mux.HandleFunc(apiBase + "/login", svc.getLoginHandler())
}
