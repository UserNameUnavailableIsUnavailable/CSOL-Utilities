package auth

import "errors"

var (
	ErrInvalidCredentials  = errors.New("invalid credentials")
	ErrUserNotFound        = errors.New("user not found")
	ErrIncorrectPassword   = errors.New("incorrect password")
	ErrMismatchingRole     = errors.New("mismatching role")
	ErrInValidToken        = errors.New("invalid token")
	ErrMissingCredentials  = errors.New("missing credentials")
	ErrInternalServerError = errors.New("internal server error")
)
