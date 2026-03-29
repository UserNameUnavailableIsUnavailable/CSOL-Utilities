package main

import (
	"context"
	"log/slog"
	"net"
	"net/http"
	"os"
	"os/signal"
	"syscall"
	"time"

	"git.macrohard.fun/root/csol-utilities/server/internal/auth"
	"git.macrohard.fun/root/csol-utilities/server/internal/config"
	"git.macrohard.fun/root/csol-utilities/server/internal/handler"
	"git.macrohard.fun/root/csol-utilities/server/internal/hub"
	"git.macrohard.fun/root/csol-utilities/server/internal/rpc"
	"google.golang.org/grpc"
)

func main() {
	quit := make(chan os.Signal, 1)
	signal.Notify(quit, syscall.SIGINT, syscall.SIGTERM)
	// ── Logger ───────────────────────────────────────────────────────────
	logger := slog.New(slog.NewTextHandler(os.Stdout, &slog.HandlerOptions{Level: slog.LevelDebug}))
	slog.SetDefault(logger)

	// ── Configuration ────────────────────────────────────────────────────
	cfg, err := config.Load("config.yaml")
	if err != nil {
		slog.Error("failed to load config", "error", err)
		os.Exit(1)
	}

	// ── Auth service ─────────────────────────────────────────────────────
	authSvc := auth.NewService(cfg.JWT.Secret, cfg.JWT.Expiry)

	// ── WebSocket hub ────────────────────────────────────────────────────
	wsHub := hub.New()
	go wsHub.Run()

	// ── HTTP router ──────────────────────────────────────────────────────
	mux := http.NewServeMux()
	handler.Register(mux, authSvc, wsHub)

	httpServer := &http.Server{
		Addr:         cfg.HTTP.Addr,
		Handler:      handler.WithCORS(mux),
		ReadTimeout:  10 * time.Second,
		WriteTimeout: 10 * time.Second,
		IdleTimeout:  120 * time.Second,
	}

	// ── gRPC server ─────────────────────────────────────────────────────
	grpcServer := grpc.NewServer()
	rpc.Register(grpcServer, wsHub)

	// ── Start servers ────────────────────────────────────────────────────
	go func() {
		slog.Info("HTTP server starting", "addr", cfg.HTTP.Addr)
		if err := httpServer.ListenAndServe(); err != nil && err != http.ErrServerClosed {
			slog.Error("HTTP server error", "error", err)
			os.Exit(1)
		}
	}()

	go func() {
		lis, err := net.Listen("tcp", cfg.GRPC.Addr)
		if err != nil {
			slog.Error("gRPC listen error", "error", err)
			os.Exit(1)
		}
		slog.Info("gRPC server starting", "addr", cfg.GRPC.Addr)
		if err := grpcServer.Serve(lis); err != nil {
			slog.Error("gRPC server error", "error", err)
			os.Exit(1)
		}
	}()

	// ── Graceful shutdown ────────────────────────────────────────────────
	sig := <-quit
	slog.Info("shutting down", "signal", sig)

	ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()

	grpcServer.GracefulStop()
	if err := httpServer.Shutdown(ctx); err != nil {
		slog.Error("HTTP shutdown error", "error", err)
	}
	slog.Info("server stopped")
}
