
cc_binary(
    name = "main",
    srcs = ["main.c"],
    # linkopts = select({
    #     "@platforms//os:osx": ["-lncurses"],
    #     "//conditions:default": ["-lncursesw"],
    # }),
    # defines = select({
    #     "@platforms//os:linux": ["_XOPEN_SOURCE"],
    #     "//conditions:default": [],
    # }),
)

# cc_library(
#     name = "lib",
#     srcs = ["lib.c"],
#     hdrs = ["lib.h"],
# )

cc_binary(
    name = "test",
    srcs = ["test.c"],
)
