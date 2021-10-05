load("//bazel:build.bzl", "COPTS", "LINKOPTS")

filegroup(
    name = "data",
    srcs = glob(["data/*"]),
)

cc_test(
    name = "app",
    size = "small",
    srcs = [
        "app/sidecar.cc",
        "test.cc",
    ],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//:app",
        "@gtest",
    ],
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
        "//:core",
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
        "//:cli",
        "@gtest",
    ],
)

cc_test(
    name = "dbal",
    size = "small",
    srcs = [
        "dbal/cassandra.cc",
        "dbal/context.cc",
        "dbal/redis.cc",
        "test.cc",
    ],
    copts = COPTS,
    linkopts = LINKOPTS,
    tags = [
        "exclusive",
        "integration",
    ],
    deps = [
        "//:dbal.cassandra",
        "//:dbal.redis",
        "@gtest",
    ],
)

cc_test(
    name = "net",
    size = "small",
    srcs = [
        "net/http.cc",
        "test.cc",
    ],
    copts = COPTS,
    linkopts = LINKOPTS,
    tags = [
        "exclusive",
        "integration",
    ],
    deps = [
        "//:net",
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
        "//:templating",
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
        "//:web",
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
    tags = ["integration"],
    deps = [
        "//:web.redis",
        "@gtest",
    ],
)
