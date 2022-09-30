cc_library(
    name = "helplib",
    srcs = [
        "help.cpp",
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
    srcs = ["main.cpp"],
    deps = [":helplib"]
)