load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")
load("@com_google_googleapis//:repository_rules.bzl", "switched_rules_by_language")

def bes_secondary_deps():
    # gRPC
    grpc_extra_deps()

    # Configure @com_google_googleapis to only compile C++ and gRPC libraries.
    switched_rules_by_language(
        name = "com_google_googleapis_imports",
        cc = True,
        grpc = True,
    )
