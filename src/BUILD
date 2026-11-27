cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [
        ":huffman",
    ]
)

cc_library(
    name = "compressor",
    hdrs = ["compressor.h"],
    srcs = ["compressor.cc"],
    deps = [
        ":util",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/status:statusor",
        "@abseil-cpp//absl/strings",
    ]
)

cc_library(
    name = "huffman",
    hdrs = ["huffman.h"],
    srcs = ["huffman.cc"],
    deps = [
        ":char",
        ":compressor",
        "@abseil-cpp//absl/container:flat_hash_map",
        "@abseil-cpp//absl/status:statusor",
    ]
)

cc_library(
    name = "char",
    hdrs = ["char.h"],
    deps = [
        "@abseil-cpp//absl/log:absl_check",
    ]
)

cc_library(
    name = "util",
    hdrs = ["util.h"],
    srcs = ["util.cc"],
    deps = [
        "@abseil-cpp//absl/base:core_headers",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/status:statusor",
    ]
)
