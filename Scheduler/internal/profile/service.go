package profile

import (
	"fmt"
	"net/http"

	"git.macrohard.fun/root/csol-utilities/Scheduler/internal/auth"
	"git.macrohard.fun/root/csol-utilities/Scheduler/internal/store"
)

type Service struct {
	profiles store.Profile
}

func NewService(profiles *store.Profile) *Service {
	return &Service{
		profiles: *profiles,
	}
}

func (svc *Service) Install(mux *http.ServeMux, authSvc *auth.Service, apiBase string) {
	apiBase = fmt.Sprintf("/api%s", apiBase)
	// GET /api/profiles?user_id={user_id}
	listHandler := authSvc.InjectTokenMiddleware(http.HandlerFunc(svc.list))
	mux.Handle("GET " + apiBase, listHandler)
	// POST /api/profiles
	createHandler := authSvc.InjectTokenMiddleware(http.HandlerFunc(svc.create))
	mux.Handle("POST " + apiBase, createHandler)
	// PUT /api/profiles/{profile_id}
	updateHandler := authSvc.InjectTokenMiddleware(http.HandlerFunc(svc.update))
	mux.Handle("PUT " + apiBase + "/{profile_id}", updateHandler)
	// DELETE /api/profiles/{profile_id}
	deleteHandler := authSvc.InjectTokenMiddleware(http.HandlerFunc(svc.delete))
	mux.Handle("DELETE " + apiBase + "/{profile_id}", deleteHandler)
}
