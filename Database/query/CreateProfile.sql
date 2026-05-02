-- name: CreateProfile :one
-- Creates a new profile entry for the specified user and returns the new profile_id.
insert into profiles (user_id)
values ($1)
returning profile_id;