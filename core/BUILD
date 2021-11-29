package(default_visibility = ["//visibility:public"])

# protobuf
load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_proto_library")

proto_library(
    name = "smallquery.proto",
    srcs = ["proto/smallquery.proto"],
)

cc_proto_library(
    name = "smallquery_cc.proto",
    deps = [":smallquery.proto"],
)

# core
cc_library(
    name = "cc_smallquery_core",
    hdrs = glob(["*.h"]),
    srcs = glob(["*.cpp"]),
    deps = [
        ":smallquery_cc.proto",

        "@com_google_zetasql//zetasql/public:analyzer",
        "@com_google_zetasql//zetasql/public:evaluator",
        "@com_google_zetasql//zetasql/public:simple_catalog",
        "@com_google_zetasql//zetasql/public:evaluator_table_iterator",
        "@com_google_zetasql//zetasql/public:value",
        "@com_google_zetasql//zetasql/public:type",
        "@com_google_zetasql//zetasql/public/functions:convert_string",
    ]
)

cc_binary(
    name = "libsmallquery.so",
    linkshared = True,
    srcs = [
        ":cc_smallquery_core",
    ],
    deps = [
        ":smallquery_cc.proto",
        ":cc_smallquery_core",

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
        ":cc_smallquery_core",
    ],
)