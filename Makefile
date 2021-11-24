ZETASQL_VERSION=2021.09.1


.PHONY: test
test:
	bazelisk test --test_output=all //core:test


.PHONY: zetasql
zetasql:
	rm -rf zetasql
	git clone --depth 1 -b $(ZETASQL_VERSION) https://github.com/google/zetasql
	sed -i 's;//zetasql/base:zetasql_implementation;//visibility:public;' zetasql/zetasql/parser/BUILD
	sed -i 's;//zetasql/base:zetasql_implementation;//visibility:public;' zetasql/zetasql/resolved_ast/BUILD


.PHONY: bazelisk
bazelisk:
	go get github.com/bazelbuild/bazelisk