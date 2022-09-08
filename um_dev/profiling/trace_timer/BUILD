load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "trace_timer",
    srcs = ["trace_timer.cc"],
    hdrs = ["trace_timer.h"],
    deps = [
        "//cyber/time:time",
        "//um_dev/utils:files",
    ],
)

cpplint()