from smallquery.testing import Database

db = Database()
db.create_table_from_yaml('''
  name: my_table
  columns:
  - name: answer
    type: int64
''')

db.execute('insert into my_table values (42)')

ret = db.execute('select * from my_table')
answer = ret['records'][0]['answer']

print(f'answer is {answer}')