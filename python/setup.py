from setuptools import setup, find_packages
from glob import glob

setup(
    name='smallquery',
    version='2021.11.24.0',
    packages=find_packages(),
    package_dir={'smallquery': 'smallquery'},
    data_files=[
        ('lib', glob('./smallquery/lib/lib*.so'))
    ],
    entry_points={
        'console_scripts': [
            'smallquery = smallquery.cli:main'
        ],
    },
)
