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

cc_library(
    name = "lib_ocp",
    srcs = glob(["**/*.cc", "**/*.hpp"]),
    hdrs = glob(["**/*.hpp"]),
    deps = [":helplib", "@boost//:program_options"],
    visibility = [
        "//tests:__pkg__"
    ],
)

cc_binary(
    name = "src",
    srcs = glob(["**/*.cc", "**/*.hpp"]),
    deps = [":helplib", "@boost//:program_options"]
)