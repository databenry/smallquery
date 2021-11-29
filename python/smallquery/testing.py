import os
import ctypes

libsq = ctypes.cdll.LoadLibrary('libsmallquery.so')
libsq.SmallQuery_execute.restype = ctypes.c_char_p


import yaml
import json


class Database:
    def __init__(self):
        self.ptr = libsq.SmallQuery_new()
    
    def close(self):
        return libsq.SmallQuery_delete()

    def execute(self, sql):
        sql_str = ctypes.create_string_buffer(sql.encode('utf8'))
        return libsq.SmallQuery_execute(self.ptr, sql_str)

    def create_table_from_yaml(self, table_yaml):
        table = yaml.load(table_yaml, Loader=yaml.BaseLoader)
        table['records'] = [ { 'map': m } for m in table['records'] ]
        table_json = json.dumps(table)
        table_json_str = ctypes.create_string_buffer(table_json.encode('utf8'))

        return libsq.SmallQuery_create_table(self.ptr, table_json_str)