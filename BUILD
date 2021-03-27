load("//bazel:build.bzl", "bes_cc_library")

# Application kernel
bes_cc_library(
    name = "app",
    deps = [
        ":cli",
        ":core",
        ":log",
        ":net",
    ],
)

# CLI parser
bes_cc_library(
    name = "cli",
    deps = [
        ":core",
        ":log",
    ],
)

# Core library with platform-wide common classes
bes_cc_library(
    name = "core",
    deps = [
        ":log",
        "@yaml-cpp",
    ],
)

# FastCGI
bes_cc_library(
    name = "fastcgi",
    deps = [
        ":core",
        ":log",
        ":net",
    ],
)

# Logging system
bes_cc_library(
    name = "log",
)

# Internet & networking
bes_cc_library(
    name = "net",
    deps = [
        ":core",
    ],
)

# RPC service management
bes_cc_library(
    name = "rpc",
    deps = [
        ":app",
        ":core",
        ":log",
        ":net",
        "@com_github_grpc_grpc//:grpc++",
    ],
)

# Templating
bes_cc_library(
    name = "templating",
    deps = [
        ":core",
        ":log",
    ],
)

# Web server
bes_cc_library(
    name = "web",
    deps = [
        ":core",
        ":fastcgi",
        ":log",
        ":net",
        "@yaml-cpp",
    ],
)

# Web + Redis sessions
bes_cc_library(
    name = "web.redis",
    deps = [
        ":log",
        ":net",
        ":web",
        "@cpp_redis",
        "@yaml-cpp",
    ],
)

# Web + templating
bes_cc_library(
    name = "web.templating",
    deps = [
        ":app",
        ":templating",
        ":web",
        "@nlohmann-json",
        "@yaml-cpp",
    ],
)
