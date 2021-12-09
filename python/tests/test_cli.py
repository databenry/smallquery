import pytest
import io
import json
from textwrap import dedent
from smallquery.cli import run_query


def test_cli(tmpdir):
    filename = tmpdir + '/test.yaml'
    with open(filename, 'w') as f:
        f.write(dedent('''
            name: my_table
            columns:
            - name: col_int
              type: int64
            - name: col_str
              type: string

            records:
            - col_int: 1
              col_str: hello
            - col_int: 2
              col_str: world
        '''))

    fr = io.StringIO('''
        select
            col_int,
            col_int + 42 as new_col_int
        from
            my_table
    ''')

    fw = io.StringIO()

    run_query([filename], fr, fw)

    ret = fw.getvalue().strip().split('\n')

    assert len(ret) == 2
    assert json.loads(ret[0])['col_int'] == '1'
    assert json.loads(ret[0])['new_col_int'] == str(1 + 42)
