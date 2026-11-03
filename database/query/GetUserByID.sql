-- name: GetUserByID :one
select user_id, user_name, user_role, user_password_hash, user_email
from users
where user_id = $1;