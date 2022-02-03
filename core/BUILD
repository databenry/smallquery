package(default_visibility = ["//visibility:public"])

# pybind11
load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")

cc_library(
    name = "cc_smallquery",
    srcs = glob(["*.h", "*.cpp"]),
    deps = [
        "//core/proto:smallquery_cc_proto",
        "@com_google_zetasql//zetasql/public:analyzer",
        "@com_google_zetasql//zetasql/public:evaluator",
        "@com_google_zetasql//zetasql/public:simple_catalog",
        "@com_google_zetasql//zetasql/public:evaluator_table_iterator",
        "@com_google_zetasql//zetasql/public:value",
        "@com_google_zetasql//zetasql/public:type",
        "@com_google_zetasql//zetasql/public/functions:convert_string",
    ],
)

cc_test(
    name = "test",
    srcs = glob(["test/*.cpp"]),
    deps = [
        "@com_google_googletest//:gtest_main",
        ":cc_smallquery",
    ],
)

pybind_extension(
    name = "_smallquery",
    linkstatic = True,
    srcs = glob(["*.h", "*.cpp"]),
    deps = [
        "//core/proto:smallquery_cc_proto",
        ":cc_smallquery",
    ]
)

py_library(
    name = "smallquery",
    srcs = glob(["*.py"]),
    srcs_version = "PY3",
    data = [
        ":_smallquery.so"
    ]
)