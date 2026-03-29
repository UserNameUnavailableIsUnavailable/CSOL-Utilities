-- check if the accounts table exists, if not, create it
create table if not exists accounts (
    id serial primary key,
    username text not null unique,
    password_hash text not null,
    created_at timestamp not null default NOW()
);

-- create a default admin account for testing
insert into accounts (username, password_hash) values ('admin', '123456') on conflict do nothing;