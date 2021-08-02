load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Common-cpp
http_archive(
    name = "wfa_common_cpp",
    sha256 = "63f923b38a3519c57d18db19b799d2040817c636be520c8c82830f7a0d63af47",
    strip_prefix = "common-cpp-215be9e75b6d9f362d419e21c9804bd0d8d68916",
    url = "https://github.com/world-federation-of-advertisers/common-cpp/archive/215be9e75b6d9f362d419e21c9804bd0d8d68916.tar.gz",
)

load("@wfa_common_cpp//build:common_cpp_repositories.bzl", "common_cpp_repositories")

common_cpp_repositories()

load("@wfa_common_cpp//build:common_cpp_deps.bzl", "common_cpp_deps")

common_cpp_deps()
