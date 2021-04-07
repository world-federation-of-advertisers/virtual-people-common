load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_protobuf",
    sha256 = "355cf346e6988fd219ff7b18e6e68a742aaef09a400a0cf2860e7841468a12ac",
    strip_prefix = "protobuf-3.15.7",
    urls = ["https://github.com/protocolbuffers/protobuf/releases/download/v3.15.7/protobuf-all-3.15.7.tar.gz"],
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()
