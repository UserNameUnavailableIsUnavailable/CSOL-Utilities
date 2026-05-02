create table profile_files (
    profile_id integer not null references profiles(profile_id) on delete cascade,
    profile_file_name text not null,
    profile_file_content bytea not null
);