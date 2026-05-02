package store

import (
	"context"

	"git.macrohard.fun/root/csol-utilities/Scheduler/internal/db"
	"github.com/jackc/pgx/v5/pgxpool"
)

// Profile is a set of user-specific files.
// Each user can have multiple profiles. The user can name each profile and store a JSON string as the content of the profile.
// Currently in CSOL-Utilities, the profile contains Setting.lua and WeaponList.lua, these two files are encoded in a JSON object and stored in the content field of the profile.
type Profile struct {
	UserID      int32
	ID          int32
	Name        string
	Description string
	Content     ProfileContent
}

type ProfileContent struct {
	Setting    string `json:"Setting.lua"`
	WeaponList string `json:"WeaponList.lua"`
}

type ProfileStore struct {
	pool      *pgxpool.Pool
	queries *db.Queries
}

func NewProfileStore(p *pgxpool.Pool) *ProfileStore {
	queries := db.New(p)
	ps := &ProfileStore{
		pool:      p,
		queries: queries,
	}
	return ps
}

func (ps *ProfileStore) Create(ctx context.Context, uid int32, profile *Profile) error {
	json := []byte(`{"Setting.lua": "` + profile.Content.Setting + `", "WeaponList.lua": "` + profile.Content.WeaponList + `"}`)
	params := db.CreateProfileFileParams{
		ProfileID:          0,
		ProfileName:        profile.Name,
		ProfileDescription: profile.Description,
		ProfileContent:     json,
	}
	// begin transaction
	tx, err := ps.pool.Begin(ctx)
	if err != nil {
		return err
	}
	defer tx.Rollback(ctx)
	qtx := ps.queries.WithTx(tx)
	profileID, err := qtx.CreateProfile(ctx, uid)
	if err != nil {
		return err
	}
	params.ProfileID = profileID
	err = qtx.CreateProfileFile(ctx, params)
	if err != nil {
		return err
	}
	err = tx.Commit(ctx)
	if err != nil {
		return err
	}
	profile.ID = params.ProfileID
	return nil
}

