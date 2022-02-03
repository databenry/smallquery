ZETASQL_VERSION=2021.09.1


.PHONY: build
build:
	PYTHON_BIN_PATH=$(shell which python3) bazelisk build //core:smallquery


.PHONY: test
test:
	bazelisk test --test_output=all //core:cc_test


.PHONY: zetasql
zetasql:
	rm -rf zetasql
	git clone --depth 1 -b $(ZETASQL_VERSION) https://github.com/google/zetasql


.PHONY: bazelisk
bazelisk:
	go get github.com/bazelbuild/bazelisk
