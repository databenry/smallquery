## SmallQuery

This repository is still under construction and experimental, so please be careful to use for any purpose.

### Usage

#### CLI

```
$ echo select 42 as answer | smallquery
{"answer": "42"}
```

#### Python

```py
$ cat example/hello.py
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
```

```
$ python3 example/hello.py
answer is 42
```

### License

[Apache License 2.0](LICENSE)