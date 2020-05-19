COPTS = [
    "-std=c++17",
    "-I.",
]
LINKOPTS = [
    "-pthread",
]

def bes_cc_library(name, deps = []):
    native.cc_library(
        name = name,
        srcs = native.glob(["bes/" + name + "/**/*.cc"]),
        hdrs = native.glob([
            "bes/" + name + ".h",
            "bes/" + name + "/**/*.h",
            "bes/" + name + "/**/*.tcc",
        ]),
        copts = COPTS,
        includes = ["bes"],
        linkopts = LINKOPTS,
        deps = deps,
        visibility = ["//visibility:public"],
    )
