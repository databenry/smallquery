import pytest
import smallquery
from textwrap import dedent


@pytest.fixture()
def db():
    db = smallquery.SmallQuery()
    db.create_table_from_yaml(dedent('''
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
    return db


def test_select(db):
    db.execute('select 42 as answer')


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