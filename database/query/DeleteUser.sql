-- name: DeleteUser :exec
delete from users
where user_id = $1;