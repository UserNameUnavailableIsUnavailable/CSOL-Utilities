package auth

import (
	"context"
	"fmt"
	"log/slog"
	"time"

	"git.macrohard.fun/root/csol-utilities/Scheduler/internal/store"
	"github.com/golang-jwt/jwt/v5"
)

// LoginRequest represents the login request payload
type LoginRequest struct {
	ID       int32  `json:"id"`
	Role     string `json:"role"`
	Password string `json:"password"`
}

// LoginResponse represents the login response payload, returned by Login on success
type LoginResponse struct {
	Token string `json:"token"`
}

// Claims represents the JWT claims used for authentication
type Claims struct {
	ID    int32  `json:"id"`
	Name  string `json:"name"`
	Role  string `json:"role"`
	Email string `json:"email"`
	jwt.RegisteredClaims
}

// Login handles the login process for a user
func (svc *Service) Login(ctx context.Context, req *LoginRequest) (*LoginResponse, error) {
	if req.ID == 0 || req.Password == "" || req.Role == "" {
		return nil, ErrMissingCredentials
	}

	user, err := svc.verifyIdentity(ctx, req)

	if err != nil {
		return nil, err
	}

	// NOTE: Do not log the password or password hash for security reasons
	userDump := fmt.Sprintf("ID: %d, Name: %s, Role: %s, Email: %s", user.ID, user.Name, user.Role, user.Email)
	slog.Info("User logged in successfully", "user", userDump)

	claims := &Claims{
		ID:    user.ID,
		Name:  user.Name,
		Role:  user.Role,
		Email: user.Email,
	}

	token, err := svc.generateToken(claims)
	if err != nil {
		return nil, err
	}
	return &LoginResponse{Token: token}, nil
}

func (svc *Service) verifyIdentity(ctx context.Context, req *LoginRequest) (*store.User, error) {
	user, err := svc.users.GetByID(ctx, req.ID)
	if err != nil {
		return nil, err
	}
	err = svc.users.CompareAndHashPassword(user.PasswordHash, req.Password)
	if err != nil {
		return nil, ErrIncorrectPassword
	}
	// TODO: admin can login as normal user
	if req.Role != user.Role {
		return nil, ErrMismatchingRole
	}
	return user, nil
}

func (svc *Service) generateToken(claims *Claims) (string, error) {
	now := time.Now()
	claims.RegisteredClaims.IssuedAt = jwt.NewNumericDate(now)
	claims.RegisteredClaims.ExpiresAt = jwt.NewNumericDate(now.Add(svc.expiry))

	token := jwt.NewWithClaims(jwt.SigningMethodHS256, claims)
	return token.SignedString(svc.secret)
}

// validateToken parses and validates a JWT string, returning its claims.
func (svc *Service) validateToken(tokenStr string) (*Claims, error) {
	token, err := jwt.ParseWithClaims(tokenStr, &Claims{}, func(t *jwt.Token) (any, error) {
		if _, ok := t.Method.(*jwt.SigningMethodHMAC); !ok {
			return nil, ErrInValidToken
		}
		return svc.secret, nil
	})
	if err != nil {
		return nil, ErrInValidToken
	}

	claims, ok := token.Claims.(*Claims)
	if !ok || !token.Valid {
		return nil, ErrInValidToken
	}
	return claims, nil
}
