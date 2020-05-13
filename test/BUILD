load("//lib:std.bzl", "COPTS", "LINKOPTS")

cc_test(
    name = "bes",
    size = "small",
    srcs = [
        "bes/filefinder.cc",
        "test.cc",
    ],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//lib:bes",
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
    srcs = [
        "templating/expressions.cc",
        "templating/rendering.cc",
        "templating/tags.cc",
        "templating/text.cc",
        "test.cc",
    ],
    copts = COPTS,
    data = [
        ":data/hello-world.out-1.html",
        ":data/hello-world.out-2.html",
        ":data/hello-world.out-3.html",
        ":data/hello-world.tmpl.html.twig",
    ],
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
