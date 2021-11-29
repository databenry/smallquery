import sys
import json
from .testing import Database


def run_query(args, f_in, f_out):
    db = Database()
    for a in args:
        with open(a, 'r') as f:
            table_yaml = f.read()
            db.create_table_from_yaml(table_yaml)

    query = f_in.read()
    ret = db.execute(query)

    for r in ret['records']:
        f_out.write(json.dumps(r['map']))
        f_out.write('\n')


def main():
    run_query(sys.argv[1:], sys.stdin, sys.stdout)