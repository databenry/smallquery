import json
import yaml
import _smallquery as cc_smallquery


class SmallQuery:
    def __init__(self):
        self.db = cc_smallquery.SmallQuery()
    

    def create_table_from_yaml(self, table_yaml):
        table = yaml.load(table_yaml, Loader=yaml.BaseLoader)
        if 'records' in table:
            table['records'] = [ { 'map': m } for m in table['records'] ]
        else:
            table['records'] = []
        table_json = json.dumps(table)

        return self.db.create_table(table_json)


    def execute(self, sql):
        ret_json = self.db.execute(sql)
        ret = json.loads(ret_json)

        if 'records' in ret:
            ret['records'] = [ r['map'] for r in ret['records'] ]

        return ret