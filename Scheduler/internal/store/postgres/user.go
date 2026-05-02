package postgres

import (
	"context"

	"git.macrohard.fun/root/csol-utilities/Scheduler/internal/db"
	"git.macrohard.fun/root/csol-utilities/Scheduler/internal/store"
	"golang.org/x/crypto/bcrypt"
)

type UserStore struct {
	queries *db.Queries
}

func NewUserStore(dbtx db.DBTX) (*UserStore, error) {
	queries := db.New(dbtx)
	us := &UserStore{
		queries: queries,
	}
	return us, nil
}

func (us *UserStore) GetPasswordHash(password string) ([]byte, error) {
	passwordHash, err := bcrypt.GenerateFromPassword([]byte(password), bcrypt.DefaultCost)
	return passwordHash, err
}

func (us *UserStore) CompareAndHashPassword(passwordHash []byte, password string) error {
	return bcrypt.CompareHashAndPassword(passwordHash, []byte(password))
}

func (us *UserStore) GetByID(ctx context.Context, id int32) (*store.User, error) {
	res, err := us.queries.GetUserByID(ctx, id)
	if err != nil {
		return nil, err
	}
	user := &store.User{
		ID:           res.UserID,
		Name:         res.UserName,
		Role:         res.UserRole,
		PasswordHash: res.UserPasswordHash,
		Email:        res.UserEmail,
	}
	return user, nil
}

func (us *UserStore) Create(ctx context.Context, user *store.User) error {
	params := db.CreateUserParams{
		UserName:         user.Name,
		UserRole:         user.Role,
		UserPasswordHash: user.PasswordHash,
		UserEmail:        user.Email,
	}
	id, err := us.queries.CreateUser(ctx, params)
	if err != nil {
		return err
	}
	user.ID = id // Set the ID of the user after creation
	return nil
}

func (us *UserStore) Remove(ctx context.Context, id int32) error {
	err := us.queries.DeleteUser(ctx, id)
	return err
}

func (us *UserStore) UpdateName(ctx context.Context, id int32, name string) error {
	params := db.UpdateUserNameParams{
		UserID:   id,
		UserName: name,
	}
	err := us.queries.UpdateUserName(ctx, params)
	return err
}

func (us *UserStore) UpdatePasswordHash(ctx context.Context, id int32, passwordHash []byte) error {
	params := db.UpdateUserPasswordHashParams{
		UserID:           id,
		UserPasswordHash: passwordHash,
	}
	err := us.queries.UpdateUserPasswordHash(ctx, params)
	return err
}

func (us *UserStore) UpdateEmail(ctx context.Context, id int32, email string) error {
	params := db.UpdateUserEmailParams{
		UserID:    id,
		UserEmail: email,
	}
	err := us.queries.UpdateUserEmail(ctx, params)
	return err
}
