-- name: UpdateUserName :exec
update users
set user_name = $1,
    user_updated_at = now()
where user_id = $2;