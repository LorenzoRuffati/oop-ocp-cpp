cc_library(
    name = "helplib",
    srcs = [
        "help.cc",
        "help.h",
    ],
    hdrs = ["help.h"],
    deps = [],
    visibility = [
        "//tests:__pkg__",
    ],
)

cc_binary(
    name = "src",
    srcs = glob(["**/*.cc", "**/*.hpp"]),
    deps = [":helplib", "@boost//:program_options"]
)