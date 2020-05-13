workspace(name = "bes")

# All Bes dependencies
load("//bazel:deps.bzl", "bes_deps", "bes_test_deps")

bes_deps()

bes_test_deps()

# Load gRPC dependencies
load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()

load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")

grpc_extra_deps()
