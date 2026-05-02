package store_test

import (
	"context"
	"os"
	"testing"

	"git.macrohard.fun/root/csol-utilities/Scheduler/config"
	"git.macrohard.fun/root/csol-utilities/Scheduler/internal/store"
	"git.macrohard.fun/root/csol-utilities/Scheduler/platform/postgre"
)

func TestCreateProfile(t *testing.T)  {
	password := os.Getenv("DB_PASSWORD")
	if password == "" {
		t.Fatal("DB_PASSWORD not set")
		return
	}
	dbCfg := &config.DB{
		Host:     "127.0.0.1",
		Port:     5432,
		User:     "csol_utilities",
		Password: password,
		Name:     "csol_utilities_db",
		SSLMode:  "require",
	}
	pool, err := postgre.New(dbCfg)
	if err != nil {
		t.Error("Failed to connect DB")
		return
	}
	ps := store.NewProfileStore(pool)
	profile := store.Profile{
		UserID: 1,
		ID: 0,
		Name: "测试",
		Description: "测试",
		Content: store.ProfileContent{
			WeaponList: "Weapon:new({})",
			Setting: "Setting = {}",
		},
	}
	err = ps.Create(context.Background(), 1, &profile)
	if err != nil {
		t.Errorf("Failed to create profile: %v", err)
		return
	}
	t.Logf("Successfully created profile with id %d", profile.ID)
}
