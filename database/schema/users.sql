-- check if the accounts table exists, if not, create it
create table users (
    user_id serial primary key,
    user_name text not null unique,
    user_role text not null,
    user_password_hash bytea not null,
    user_password_changed_at timestamp not null default now(),
    user_email text not null,
    user_created_at timestamp not null default now(),
    user_updated_at timestamp not null default now(),
    unique (user_email)
);