-- The password is for testing purposes only, please change it before using it in production environment
create user if not exists csol_utilities with password '123457';
create database if not exists csol_utilities_db owner csol_utilities;
grant all privileges on database csol_utilities_db to csol_utilities;
-- switch to the csol_utilities_db database
\c csol_utilities_db;
grant all privileges on database csol_utilities_db to csol_utilities;
grant all privileges on schema public to csol_utilities;
grant all privileges on all tables in schema public to csol_utilities;
grant all privileges on all sequences in schema public to csol_utilities;