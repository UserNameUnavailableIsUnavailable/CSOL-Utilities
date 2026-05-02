create table profile_files (
    profile_id integer not null references profiles(profile_id) on delete cascade,
    profile_name text not null,
    profile_description text not null,
    profile_content bytea not null
);