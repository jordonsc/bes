workspace(name = "bes")

### LOCAL DEPS ###
load("//bazel:deps.bzl", "bes_deps", "bes_test_deps")

bes_deps()

bes_test_deps()

### GRPC ###
load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()

load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")

grpc_extra_deps()

### CPP_REDIS ###
load("@cpp_redis//bazel:deps.bzl", "cpp_redis_deps")

cpp_redis_deps()
