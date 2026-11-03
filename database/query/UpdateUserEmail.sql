-- name: UpdateUserEmail :exec
update users
set user_email = $1,
    user_updated_at = now()
where user_id = $2;