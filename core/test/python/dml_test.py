import pytest
import smallquery
from textwrap import dedent


@pytest.fixture()
def db():
    db = smallquery.SmallQuery()
    return db


def test_create_insert(db):
    db.execute('create table hello (col1 int64, answer int64)')
    db.execute('insert into hello values (1, 42)')
    db.execute('insert into hello values (1, 42)')
    db.execute('insert into hello values (2, 42 + 123)')
    db.execute('delete from hello where col1 = 1')

    ret = db.execute('select * from hello')

    assert len(ret['records']) == 1
    assert ret['records'][0]['col1'] == '2'
    assert ret['records'][0]['answer'] == str(42 + 123)


def test_insert_null(db):
    db.execute('create table hello (col1 int64, answer int64)')
    db.execute('insert into hello values (1, NULL)')

    ret = db.execute('select count(distinct col1) as cnt from hello where answer is null')

    assert len(ret['records']) == 1
    assert ret['records'][0]['cnt'] == '1'