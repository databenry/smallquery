import pytest
import json
from textwrap import dedent
from python.smallquery.testing import Database


@pytest.fixture
def db():
    test_db = Database()
    test_db.create_table_from_yaml(dedent('''
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
    return test_db


def test_select(db):
    ret = db.execute('''
        select
            42 as answer
    ''')
    
    assert len(ret['records']) == 1
    assert ret['records'][0]['answer'] == '42'


def test_select_table(db):
    ret = db.execute('''
        select
            *
        from
            my_table
    ''')

    assert len(ret['records']) == 2
    assert ret['records'][0]['col_int'] == '1'
    assert ret['records'][0]['col_str'] == '"hello"'
    assert ret['records'][1]['col_int'] == '2'
    assert ret['records'][1]['col_str'] == '"world"'