-- name: CreateUser :one
insert into users (user_name, user_role, user_password_hash, user_email) values ($1, $2, $3, $4)
returning user_id; -- returns the new user's ID
