package postgres_test

import (
	"context"
	"database/sql"
	"errors"
	"os"
	"testing"

	"git.macrohard.fun/root/csol-utilities/Scheduler/config"
	"git.macrohard.fun/root/csol-utilities/Scheduler/internal/store/postgres"
	"git.macrohard.fun/root/csol-utilities/Scheduler/platform/db"
)

func NewDB() (*sql.DB, error) {
	password := os.Getenv("DB_PASSWORD")
	if password == "" {
		return nil, errors.New("DB_PASSWORD not set")
	}

	dbCfg := &config.DB{
		Host:     "127.0.0.1",
		Port:     5432,
		User:     "csol_utilities",
		Password: password,
		Name:     "csol_utilities_db",
		SSLMode:  false,
	}
	db, err := db.NewPostgre(dbCfg)
	if err != nil {
		return nil, err
	}
	return db, nil
}

func TestGetByID(t *testing.T) {
	db, err := NewDB()
	if err != nil {
		t.Errorf("Failed to establish connection to DB: %v", err)
		return
	}
	us, err := postgres.NewUserStore(db)
	if err != nil {
		t.Errorf("CreateUserStore failed: %v", err)
		return
	}
	user, err := us.GetByID(context.Background(), 1)
	if err != nil {
		t.Errorf("GetByID failed: %v", err)
		return
	}
	t.Logf("User: %v", user)
}

func TestUpdatePasswordHash(t *testing.T) {
	db, err := NewDB()
	if err != nil {
		t.Errorf("Failed to establish connection to DB: %v", err)
		return
	}
	us, err := postgres.NewUserStore(db)
	if err != nil {
		t.Errorf("CreateUserStore failed: %v", err)
		return
	}
	passwordHash, err := us.GetPasswordHash("114514")
	if err != nil {
		t.Errorf("GetPasswordHash failed: %v", err)
		return
	}
	t.Logf("PasswordHash: %v", passwordHash)
	err = us.UpdatePasswordHash(context.Background(), 1, passwordHash)
	if err != nil {
		t.Errorf("UpdatePasswordHash failed: %v", err)
		return
	}
}

func TestUpdateName(t *testing.T) {
	db, err := NewDB()
	if err != nil {
		t.Errorf("Failed to establish connection to DB: %v", err)
		return
	}
	us, err := postgres.NewUserStore(db)
	if err != nil {
		t.Errorf("CreateUserStore failed: %v", err)
		return
	}
	err = us.UpdateName(context.Background(), 1, "Administrator")
	if err != nil {
		t.Errorf("UpdateName failed: %v", err)
		return
	}
}
