import argparse
import bcrypt

parser = argparse.ArgumentParser(description='Compare a password with a given hash.')
parser.add_argument('-P', '--password', type=str, help='password.')
parser.add_argument('-H', '--hash', type=str, help='password hash to compare with the given password.')
args = parser.parse_args()

if bcrypt.checkpw(args.password.encode('utf-8'), args.hash.encode('utf-8')):
    print("Password is correct.")
else:
    print("Password is incorrect.")               