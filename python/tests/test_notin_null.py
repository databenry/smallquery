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
        - name: id
          type: int64
        - name: category
          type: string

        records:
        - id: 1
          category: A
        - id: 2
          category: A
        - id: 3
          category: B
    '''))
    return test_db


def test_select_nonull(db):
    ret = db.execute('''
        select
            count(distinct id) as cnt_id
        from
            my_table
        where
            category not in ('B')
    ''')

    assert ret['records'][0]['cnt_id'] == '2'


def test_select_with_null(db):
    db.execute('insert into my_table values (4, null)')

    ret = db.execute('''
        select
            count(distinct id) as cnt_id
        from
            my_table
        where
            category not in ('B')
    ''')

    assert ret['records'][0]['cnt_id'] == '2'