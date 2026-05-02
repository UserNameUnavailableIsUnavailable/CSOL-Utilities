import argparse
import bcrypt

parser = argparse.ArgumentParser(description='Compute password hash.')
parser.add_argument('password', type=str, help='The password to hash.')
args = parser.parse_args()

hash = bcrypt.hashpw(args.password.encode('utf-8'), bcrypt.gensalt())
print(hash.decode('utf-8'))