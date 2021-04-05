load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")
load("@cpp_redis//bazel:deps.bzl", "cpp_redis_deps")

def bes_primary_deps():
    # gRPC
    grpc_deps()

    # Redis
    cpp_redis_deps()
