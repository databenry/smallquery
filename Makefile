ZETASQL_VERSION=2021.09.1


.PHONY: build
build:
	pip3 install -r ./python/requirements.txt

	PYTHON_BIN_PATH=$(shell which python3) bazelisk build //core:smallquery.so

	# rm -rf python/smallquery/lib/
	# mkdir -p python/smallquery/lib/
	# cp bazel-bin/core/*.so python/smallquery/lib/


.PHONY: test
test:
	bazelisk test --test_output=all //core:test

	# pip3 install -e ./python
	# LD_LIBRARY_PATH=./python/smallquery/lib python3 -m pytest ./python/tests


.PHONY: zetasql
zetasql:
	rm -rf zetasql
	git clone --depth 1 -b $(ZETASQL_VERSION) https://github.com/google/zetasql


.PHONY: bazelisk
bazelisk:
	go get github.com/bazelbuild/bazelisk
