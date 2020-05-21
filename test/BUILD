load("//bazel:build.bzl", "COPTS", "LINKOPTS")

filegroup(
    name = "data",
    srcs = glob(["data/*"]),
)

cc_test(
    name = "core",
    size = "small",
    srcs = [
        "core/filefinder.cc",
        "test.cc",
    ],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//lib:core",
        "@gtest",
    ],
)

cc_test(
    name = "cli",
    size = "small",
    srcs = [
        "cli/arg.cc",
        "test.cc",
    ],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//lib:cli",
        "@gtest",
    ],
)

cc_test(
    name = "service",
    size = "small",
    srcs = [
        "service/sidecar.cc",
        "test.cc",
    ],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//lib:service",
        "@gtest",
    ],
)

cc_test(
    name = "templating",
    size = "small",
    srcs = glob([
        "templating/*.cc",
        "test.cc",
    ]),
    copts = COPTS,
    data = [":data"],
    linkopts = LINKOPTS,
    deps = [
        "//lib:templating",
        "@gtest",
    ],
)

cc_test(
    name = "web",
    size = "small",
    srcs = [
        "test.cc",
        "web/router.cc",
    ],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//lib:web",
        "@gtest",
    ],
)

cc_test(
    name = "web.redis",
    size = "small",
    srcs = [
        "test.cc",
        "web.redis/redis_session.cc",
    ],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//lib:web.redis",
        "@gtest",
    ],
)
