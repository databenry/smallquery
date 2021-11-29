import os
import ctypes

libsmallquery = ctypes.cdll.LoadLibrary('libsmallquery.so')
libsmallquery.SmallQuery_execute.restype = ctypes.c_char_p


class Database:
    def __init__(self):
        self.ptr = libsmallquery.SmallQuery_new()
    
    def close(self):
        return libsmallquery.SmallQuery_delete()

    def execute(self, sql):
        sql_str = ctypes.create_string_buffer(sql.encode('utf8'))
        return libsmallquery.SmallQuery_execute(self.ptr, sql_str)