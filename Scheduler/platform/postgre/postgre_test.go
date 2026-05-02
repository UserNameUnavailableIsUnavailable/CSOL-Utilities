package postgre_test

import (
	"context"
	"os"
	"testing"

	"git.macrohard.fun/root/csol-utilities/Scheduler/config"
	"git.macrohard.fun/root/csol-utilities/Scheduler/platform/postgre"
)

func TestCreateDSN(t *testing.T) {
	password := os.Getenv("DB_PASSWORD")
	if password == "" {
		t.Fatal("DB_PASSWORD not set")
	}
	dbCfg := &config.DB{
		Host: "127.0.0.1",
		Port: 5432,
		User: "csol_utilities",
		Password: password,
		Name: "csol_utilities_db",
		SSLMode: "require",
	}
	dsn, err := postgre.CreateDSN(dbCfg)
	if err != nil {
		t.Errorf("Failed to create DSN: %v", err)
	}
	t.Logf("DSN: %s", dsn)
}

func TestNewPostgre(t *testing.T) {
	password := os.Getenv("DB_PASSWORD")
	if password == "" {
		t.Fatal("DB_PASSWORD not set")
	}
	dbCfg := &config.DB{
		Host: "127.0.0.1",
		Port: 5432,
		User: "csol_utilities",
		Password: password,
		Name: "csol_utilities_db",
		SSLMode: "require",
	}
	db, err := postgre.New(dbCfg)
	if err != nil {
		t.Errorf("Failed to establish connection to Postgre DB: %v", err)
	}
	defer db.Close()
	db.Ping(context.Background())
}
