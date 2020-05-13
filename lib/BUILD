package(default_visibility = ["//visibility:public"])

load("//bazel:deps.bzl", "COPTS", "LINKOPTS")

INCLUDES = ["."]

# Application kernel
cc_library(
    name = "app",
    srcs = glob(["app/**/*.cc"]),
    hdrs = glob([
        "app/**/*.h",
        "app/**/*.tcc",
    ]),
    copts = COPTS,
    includes = INCLUDES,
    linkopts = LINKOPTS,
    deps = [
        ":bes",
        ":cli",
        ":log",
        ":net",
        ":service",
    ],
)

# Core library with platform-wide common classes
cc_library(
    name = "bes",
    srcs = glob(["bes/**/*.cc"]),
    hdrs = glob([
        "bes/**/*.h",
        "bes/**/*.tcc",
    ]),
    copts = COPTS,
    includes = INCLUDES,
    linkopts = LINKOPTS,
    deps = [
        ":log",
        "@yaml-cpp//:yaml-cpp",
    ],
)

# CLI parser
cc_library(
    name = "cli",
    srcs = glob(["cli/**/*.cc"]),
    hdrs = glob([
        "cli/**/*.h",
        "cli/**/*.tcc",
    ]),
    copts = COPTS,
    includes = INCLUDES,
    linkopts = LINKOPTS,
    deps = [
        ":bes",
        ":log",
    ],
)

# FastCGI
cc_library(
    name = "fastcgi",
    srcs = glob(["fastcgi/**/*.cc"]),
    hdrs = glob([
        "fastcgi/**/*.h",
        "fastcgi/**/*.tcc",
    ]),
    copts = COPTS,
    includes = INCLUDES,
    linkopts = LINKOPTS,
    deps = [
        ":bes",
        ":log",
        ":net",
    ],
)

# Logging system
cc_library(
    name = "log",
    srcs = glob(["log/**/*.cc"]),
    hdrs = glob([
        "log/**/*.h",
        "log/**/*.tcc",
    ]),
    copts = COPTS,
    includes = INCLUDES,
    linkopts = LINKOPTS,
)

# Internet & networking
cc_library(
    name = "net",
    srcs = glob(["net/**/*.cc"]),
    hdrs = glob([
        "net/**/*.h",
        "net/**/*.tcc",
    ]),
    copts = COPTS,
    includes = INCLUDES,
    linkopts = LINKOPTS,
    deps = [
        ":bes",
        ":log",
    ],
)

# Service management
cc_library(
    name = "service",
    srcs = glob(["service/**/*.cc"]),
    hdrs = glob([
        "service/**/*.h",
        "service/**/*.tcc",
    ]),
    copts = COPTS,
    includes = INCLUDES,
    linkopts = LINKOPTS,
    deps = [
        ":bes",
        ":log",
        ":net",
        "@com_github_grpc_grpc//:grpc++",
    ],
)

# Templating
cc_library(
    name = "templating",
    srcs = glob(["templating/**/*.cc"]),
    hdrs = glob([
        "templating/**/*.h",
        "templating/**/*.tcc",
    ]),
    copts = COPTS,
    includes = INCLUDES,
    linkopts = LINKOPTS,
    deps = [
        ":bes",
        ":log",
    ],
)

# Web server
cc_library(
    name = "web",
    srcs = glob(["web/**/*.cc"]),
    hdrs = glob([
        "web/**/*.h",
        "web/**/*.tcc",
    ]),
    copts = COPTS,
    includes = INCLUDES,
    linkopts = LINKOPTS,
    deps = [
        ":bes",
        ":fastcgi",
        ":log",
        ":net",
        "@yaml-cpp//:yaml-cpp",
    ],
)

# Web + templating
cc_library(
    name = "web.templating",
    srcs = glob(["web.templating/**/*.cc"]),
    hdrs = glob([
        "web.templating/**/*.h",
        "web.templating/**/*.tcc",
    ]),
    copts = COPTS,
    includes = INCLUDES,
    linkopts = LINKOPTS,
    deps = [
        ":templating",
        ":web",
    ],
)
