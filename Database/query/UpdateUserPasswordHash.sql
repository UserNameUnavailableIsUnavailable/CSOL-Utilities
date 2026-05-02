-- name: UpdateUserPasswordHash :exec
update users
set user_password_hash = $1,
    user_password_changed_at = now(),
    user_updated_at = now()
where user_id = $2;