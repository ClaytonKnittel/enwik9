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
        ":compressor",
        "@abseil-cpp//absl/status:statusor",
    ]
)

cc_library(
    name = "util",
    hdrs = ["util.h"],
    deps = [
        "@abseil-cpp//absl/base",
        "@abseil-cpp//absl/status",
    ]
)
