package(default_visibility = ["//visibility:public"])

load("@bes//bazel:build.bzl", "COPTS", "LINKOPTS")

cc_library(
    name = "nlohmann-json",
    hdrs = glob([
        "include/**/**/*.h",
        "include/**/**/*.hpp",
    ]),
    copts = COPTS,
    includes = ["include"],
    linkopts = LINKOPTS,
    deps = [
    ],
)
