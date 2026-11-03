-- name: CreateProfileFile :exec
-- Creates an associated profile file for the specified profile.
insert into profile_files (profile_id, profile_name, profile_content, profile_description)
values ($1, $2, $3, $4);