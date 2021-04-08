load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")
load("@cpp_redis//bazel:deps.bzl", "cpp_redis_deps")
load("@com_github_googleapis_google_cloud_cpp//bazel:google_cloud_cpp_deps.bzl", "google_cloud_cpp_deps")

def bes_primary_deps():
    # GCP SDK
    google_cloud_cpp_deps()

    # gRPC
    grpc_deps()

    # Redis
    cpp_redis_deps()
