load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("//tools/install:install/bzl", "install")
load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "rtk_yt_localization_component_lib",
    srcs = [
        "rtk_yt_component.cc",
        "rty_yt_localization.cc",
    ],
    hdrs = [
        "rtk_yt_component.cc",
        "rty_yt_localization.cc",
    ],
    copts = ["-DMODULE_NAME=\\\"yt_localization\\\""],
    deps = [
        "//cyber",
        "//cyber/time:clock",
        "//modules/common/math",
        "//modules/common/monitor_log",
        "//modules/common/proto:geometry_cc_proto",
        "//modules/common/util:message_util",
        "//modules/common/util:string_util",
        "//modules/drivers/gnss/proto:gnss_best_pose_cc_proto",
        "//modules/drivers/gnss/proto:gnss_cc_proto",
        "//modules/drivers/gnss/proto:ins_cc_proto",
        "//modules/localization/proto:gps_cc_proto",
        "//modules/localization/proto:imu_cc_proto",
        "//modules/localization/proto:localization_cc_proto",
        "//modules/localization/proto:rtk_config_cc_proto",
        "//modules/transform:transform_broadcaster",
        "@com_google_googletest//:gtest",
        "@eigen",
    ],
)

cc_binary(
    name = "librtk_yt_localization_component.so",
    linkshared = True,
    linkstatic = False,
    deps = [":rtk_yt_localization_component_lib"],
)

install(
    name = "install",
    targets = [
        ":librtk_yt_localization_component.so",
    ],
)

cpplint()