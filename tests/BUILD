cc_test(
  name = "hello_test",
  size = "small",
  srcs = ["hello_test.cc"],
  deps = [
    "@com_google_googletest//:gtest_main",
    "//src:lib_ocp"
    ],
)

cc_test(
  name = "queue-rec",
  size = "small",
  srcs = ["ipc/queue/test_receiver.cc", "ipc/queue/common.cc"] + glob(["ipc/queue/*.hpp"]),
  #hdrs = glob(["ipc/queue/*.hpp"]),
  deps = [
    "@com_google_googletest//:gtest_main",
    "//src:lib_ocp"
    ],
)
cc_test(
  name = "queues",
  size = "small",
  srcs = glob(["ipc/queue/*.cc", "ipc/queue/*.hpp"]),
  #hdrs = glob(["ipc/queue/*.hpp"]),
  deps = [
    "@com_google_googletest//:gtest_main",
    "//src:lib_ocp"
    ],
)