package store

import (
	"context"
)

type User struct {
	ID           int32
	Name         string
	Role         string
	PasswordHash []byte
	Email        string
}

type UserStore interface {
	// GetByID retrieves a user by their ID
	GetByID(ctx context.Context, id int32) (*User, error)
	// Create adds a new user to the store, and sets the ID of the user after creation
	Create(ctx context.Context, user *User) error
	// Remove deletes a user from the store by their ID
	Remove(ctx context.Context, id int32) error
	// UpdateName updates the name of a user
	UpdateName(ctx context.Context, id int32, name string) error
	// UpdatePasswordHash updates the password hash of a user
	UpdatePasswordHash(ctx context.Context, id int32, passwordHash []byte) error
	// UpdateEmail updates the email of a user
	UpdateEmail(ctx context.Context, id int32, email string) error
	// GetPasswordHash retrieves the password hash for a given password
	GetPasswordHash(password string) ([]byte, error)
	// CompareAndHashPassword compares a password with a hash and returns an error if they do not match
	CompareAndHashPassword(passwordHash []byte, password string) error
}
