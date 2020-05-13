workspace(name = "bes")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

# Google Tests
http_archive(
    name = "gtest",
    strip_prefix = "googletest-1.10.x",
    urls = [
        "https://github.com/google/googletest/archive/v1.10.x.tar.gz",
    ],
)

# gRPC
http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "4cbce7f708917b6e58b631c24c59fe720acc8fef5f959df9a58cdf9558d0a79b",
    strip_prefix = "grpc-1.28.1",
    urls = [
        "https://github.com/grpc/grpc/archive/v1.28.1.tar.gz",
    ],
)

load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()

load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")

grpc_extra_deps()

# YAML
http_archive(
    name = "yaml-cpp",
    sha256 = "77ea1b90b3718aa0c324207cb29418f5bced2354c2e483a9523d98c3460af1ed",
    strip_prefix = "yaml-cpp-yaml-cpp-0.6.3",
    urls = [
        "https://github.com/jbeder/yaml-cpp/archive/yaml-cpp-0.6.3.tar.gz",
    ],
)

git_repository(
    name = "yaml-cpp",
    commit = "26faac387c237ccac75a56925c6858baf8ccda1b",
    remote = "https://github.com/jbeder/yaml-cpp.git",
)
