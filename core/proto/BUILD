package(default_visibility = ["//visibility:public"])

load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_proto_library")

proto_library(
    name = "smallquery_proto",
    srcs = ["smallquery.proto"],
)

cc_proto_library(
    name = "smallquery_cc_proto",
    deps = [":smallquery_proto"],
)