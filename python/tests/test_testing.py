import pytest
import json
from python.smallquery.testing import Database


@pytest.fixture
def db():
    test_db = Database()
    return test_db


def test_select(db):
    ret_json = db.execute('select 42 as answer')
    ret = json.loads(ret_json)
    
    assert len(ret['records']) == 1
    assert ret['records'][0]['map']['answer'] == '42'