## SmallQuery

:warning: This repository is still under construction and experimental, so please be careful to use for any purpose.

### Usage Examples

#### CLI

```sql
$ echo select 42 as answer | smallquery
{"answer": "42"}
```

YAML support

```yaml
$ cat ./example/my_table.yaml
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
```

```sql
$ echo 'select * from my_table' | smallquery ./example/my_table.yaml
{"col_str": "\"hello\"", "col_int": "1"}
{"col_str": "\"world\"", "col_int": "2"}
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


### Installation

WIP: under construction now

SmallQuery::Core (C++) can be built with the following command:

```
$ make bazelisk
$ make zetasql
$ make
```

And then, you can install SmallQuery (Python) as follows:

```
$ cd python
$ pip install .
```

Running Tests:

```
$ make test
```


### :heavy_check_mark: TODO

- [ ] Simplify Installation
- [ ] Support Python UDF
- [ ] Integrate with dbt

### License

[Apache License 2.0](LICENSE)


### Special Thanks

https://github.com/google/zetasql