from setuptools import setup, find_packages

setup(
    name='smallquery',
    version='2021.11.24.0',
    packages=find_packages(where='python'),
    package_dir={'smallquery': 'python/smallquery'},
    data_files=[
        ('', ['./bazel-bin/core/libcc_smallquery_core.so'])
    ],
    entry_points={
        'console_scripts': [
            'smallquery = smallquery.cli:main'
        ],
    },
)
