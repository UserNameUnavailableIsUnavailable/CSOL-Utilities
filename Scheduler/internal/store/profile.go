package store

import "context"

// Each user can have multiple profiles. The user can name each profile and store a JSON string as the content of the profile.
// Currently in CSOL-Utilities, the profile contains Setting.lua and WeaponList.lua, these two files are encoded in a JSON object and stored in the content field of the profile.
type Profile struct {
	UserID  int64
	ID      int64
	Name    string
	Content ProfileContent
}

type ProfileContent struct {
	Setting    string `json:"Setting.lua"`
	WeaponList string `json:"WeaponList.lua"`
}

type ProfileStore interface {
	Create(ctx context.Context, uid int64, profile *Profile) (int64, error)
	Retrieve(ctx context.Context, uid int64, pid int64) ([]*Profile, error)
	Update(ctx context.Context, uid int64, pid int64, profile *Profile) error
	Delete(ctx context.Context, uid int64, pid int64) error
}
