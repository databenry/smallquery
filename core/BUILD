package(default_visibility = ["//visibility:public"])

load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")

pybind_extension(
    name = "_smallquery",
    srcs = glob(["*.h", "*.cpp"]),
    features = ["-use_header_modules"],
    linkstatic = True,
    deps = [
        "//core/proto:smallquery_cc_proto",
        "@com_google_zetasql//zetasql/public:analyzer",
        "@com_google_zetasql//zetasql/public:evaluator",
        "@com_google_zetasql//zetasql/public:simple_catalog",
        "@com_google_zetasql//zetasql/public:evaluator_table_iterator",
        "@com_google_zetasql//zetasql/public:value",
        "@com_google_zetasql//zetasql/public:type",
        "@com_google_zetasql//zetasql/public/functions:convert_string",
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

cc_test(
    name = "cc_smallquery_test",
    srcs = glob(["test/*.cpp"]),
    deps = [
        "@com_google_googletest//:gtest_main",
        ":_smallquery",
    ]
)

py_test(
    name = "py_test",
    srcs = glob(["test/python/*.py"]),
    deps = [
        ":smallquery",
    ],
    main = "test/python/main.py",
)