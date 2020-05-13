# Standard cxx options
COPTS = [
    "-std=c++17",
]

# Standard linker options
LINKOPTS = [
    "-pthread",
]

# All app binaries require these at a minimum
STD_APP_DEPS = [
    "//lib:app",
    "//lib:bes",
    "//lib:cli",
    "//lib:log",
]
