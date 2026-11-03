package main

import (
	"fmt"
	"log/slog"
	"net/http"
	"time"

	"git.macrohard.fun/root/csol-utilities/Scheduler/config"
	"git.macrohard.fun/root/csol-utilities/Scheduler/internal/auth"
	"git.macrohard.fun/root/csol-utilities/Scheduler/internal/store"
	"git.macrohard.fun/root/csol-utilities/Scheduler/platform/postgre"
)

func main() {
	var err error
	config, err := config.Load("config.yaml")
	if err != nil {
		slog.Error("Failed to load config.")
		return
	}
	mux := http.NewServeMux()

	httpSrv := &http.Server{
		Addr:         fmt.Sprintf(":%v", config.HTTP.Port),
		Handler:      mux,
		ReadTimeout:  5 * time.Second,
		WriteTimeout: 10 * time.Second,
	}

	db, err := postgre.New(&config.DB)
	if err != nil {
		slog.Error("Failed to establish connection to DB", "error", err)
		return
	}
	defer db.Close()

	us, err := store.NewUserStore(db)
	if err != nil {
		slog.Error("Failed to create UserStore.")
		return
	}

	authSvc := auth.NewService(us, &config.Auth)
	authSvc.Install(mux, &config.Routes)

	if err = httpSrv.ListenAndServe(); err != nil {
		slog.Error("Failed to create HTTP server.")
		return
	}
}
