cc_library(
    name = "helplib",
    srcs = [
        "help.cc",
        "help.hpp",
    ],
    hdrs = ["help.hpp"],
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