COPTS = [
    "-std=c++17",
    "-Ilib",
]
LINKOPTS = [
    "-pthread",
]

def bes_cc_library(name, deps = [], copts = [], linkops = []):
    native.cc_library(
        name = name,
        srcs = native.glob(["lib/bes/" + name + "/**/*.cc"]),
        hdrs = native.glob([
            "lib/bes/" + name + ".h",
            "lib/bes/" + name + "/**/*.h",
            "lib/bes/" + name + "/**/*.tcc",
        ]),
        copts = COPTS + copts,
        linkopts = LINKOPTS + linkops,
        includes = ["lib"],
        deps = deps,
        visibility = ["//visibility:public"],
    )
