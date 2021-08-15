package(default_visibility = ["//visibility:public"])

load("@bes//bazel:build.bzl", "COPTS", "LINKOPTS")

cc_library(
    name = "curlpp",
    srcs = glob([
        "src/curlpp/**/*.cpp",
    ]),
    hdrs = glob([
        "include/**/**/*.inl",
        "include/**/**/*.hpp",
    ]),
    copts = COPTS + ["-lcurl"],
    includes = ["include"],
    linkopts = LINKOPTS + ["-lcurl"],
    deps = [
    ],
)
