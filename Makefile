ZETASQL_VERSION=2021.09.1


.PHONY: test
test:
	pip3 install pytest
	PYTHON_BIN_PATH=$(shell which python3) bazelisk test --test_output=all //core:py_test


.PHONY: wheel
wheel:
	rm -rf build/bdist*
	python3 tools/ci/bazelisk.py --version
	python3 -m pip install wheel
	PYTHON_BIN_PATH=$(shell which python3) python3 setup.py bdist_wheel -p manylinux1_x86_64


.PHONY: zetasql
zetasql:
	rm -rf zetasql
	git clone --depth 1 -b $(ZETASQL_VERSION) https://github.com/google/zetasql