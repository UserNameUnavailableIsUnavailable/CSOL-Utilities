create table profiles (
    profile_id serial primary key,
    user_id integer not null references users(user_id) on delete cascade,
    profile_created_at timestamp not null default now(),
    profile_updated_at timestamp not null default now()
);