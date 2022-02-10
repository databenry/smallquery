ZETASQL_VERSION=2021.09.1


.PHONY: wheel/Linux
wheel/Linux:
	python3 setup.py sdist
	PYTHON_BIN_PATH=$(shell which python3) python3 setup.py bdist_wheel --keep-temp -p manylinux1_x86_64

.PHONY: wheel/macOS
wheel/macOS:
	python3 setup.py sdist
	PYTHON_BIN_PATH=$(shell which python3) python3 setup.py bdist_wheel --keep-temp

.PHONY: test
test:
	PYTHON_BIN_PATH=$(shell which python3) bazelisk build //core:smallquery
	PYTHON_BIN_PATH=$(shell which python3) bazelisk test --test_output=all //core:cc_test
	
	# PYTHON_BIN_PATH=$(shell which python3) pip3 install .
	# pip3 install pytest
	# bazelisk test --test_output=all //core:py_test


.PHONY: zetasql
zetasql:
	rm -rf zetasql
	git clone --depth 1 -b $(ZETASQL_VERSION) https://github.com/google/zetasql


.PHONY: bazelisk
bazelisk:
	go get github.com/bazelbuild/bazelisk
