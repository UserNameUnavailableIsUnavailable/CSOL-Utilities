-- create a default admin account for testing purposes, the password is "123456"
insert into users (user_name, user_role, user_password_hash, user_email) values ('admin', 'admin', '$2b$12$WlMWJCiRJaUnX0RfbRsZYezW0UeJjaZMhHHlx0HuMAYHqJmTsGwoi', 'admin@macrohard.fun') on conflict do nothing;
