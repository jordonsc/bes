load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def bes_repos():
    if "yaml-cpp" not in native.existing_rules():
        # Bazel files are missing in the most recent release, need a commit after they added them
        git_repository(
            name = "yaml-cpp",
            commit = "26faac387c237ccac75a56925c6858baf8ccda1b",
            shallow_since = "1585522435 -0500",
            remote = "https://github.com/jbeder/yaml-cpp.git",
        )

    if "nlohmann-json" not in native.existing_rules():
        http_archive(
            name = "nlohmann-json",
            strip_prefix = "json-3.9.1",
            build_file = "@bes//bazel/nlohmann-json:BUILD",
            sha256 = "4cf0df69731494668bdd6460ed8cb269b68de9c19ad8c27abc24cd72605b2d5b",
            urls = ["https://github.com/nlohmann/json/archive/refs/tags/v3.9.1.tar.gz"],
        )

    if "com_github_grpc_grpc" not in native.existing_rules():
        http_archive(
            name = "com_github_grpc_grpc",
            sha256 = "51403542b19e9ed5d3b6551ce4a828e17883a1593d4ca408b098f04b0767d202",
            strip_prefix = "grpc-1.36.2",
            urls = [
                "https://github.com/grpc/grpc/archive/v1.36.2.tar.gz",
            ],
        )

    if "cpp_redis" not in native.existing_rules():
        # Sorry - fixing the Bazel support for this, will try to get the author to merge soon
        git_repository(
            name = "cpp_redis",
            #branch = "bazel-fixes",
            remote = "https://github.com/jordonsc/cpp_redis.git",
            commit = "1dec935453ac21dc7786b6a6f104f0d07d0529e4",
            shallow_since = "1589885515 +1000",
        )

    if "com_github_googleapis_google_cloud_cpp" not in native.existing_rules():
        http_archive(
            name = "com_github_googleapis_google_cloud_cpp",
            sha256 = "b61ced7ae18cb4a296377fcba17683319c06c7626c889be9acb4f44e21568f3c",
            strip_prefix = "google-cloud-cpp-1.26.0",
            urls = [
                "https://github.com/googleapis/google-cloud-cpp/archive/refs/tags/v1.26.0.tar.gz",
            ],
        )

def bes_test_repos():
    if "gtest" not in native.existing_rules():
        http_archive(
            name = "gtest",
            strip_prefix = "googletest-1.10.x",
            sha256 = "4d7cd95cdb0ef420eed163696a906cccd70964801ea611b2020f31177432c27d",
            urls = [
                "https://github.com/google/googletest/archive/v1.10.x.tar.gz",
            ],
        )
