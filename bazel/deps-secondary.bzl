load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")

def bes_secondary_deps():
    # gRPC
    grpc_extra_deps()
