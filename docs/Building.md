Building the Platform
=====================
The entire build platform is managed using [Bazel](https://bazel.build/). To get started, first get Bazel set up on your
local environment. There is no CMake support, and this is not planned.

Supported Compilers
-------------------
Currently, this project is only tested on GCC. It's recommended to use at least GCC version 8 or above.

If you're using an OS that supports multiple versions, you can either symlink to the correct version or instruct Bazel
on which to use. Consider:

    # Ensure Bazel looks to the correct GCC version:
    export CC="/usr/bin/gcc-9"
    
    # -- OR -- 

    # Set GCC to use GCC-9 system wide
    ln -sf gcc-9 /usr/bin/gcc
    ln -sf g++-9 /usr/bin/g++


Prerequisites
-------------
The DBAL library requires the [Datastax Cassandra drivers](https://downloads.datastax.com/cpp-driver/) available on the
OS. You require the Cassandra driver and the and `libuv` package.

* [Cassandra Driver](https://downloads.datastax.com/cpp-driver/ubuntu/18.04/cassandra/v2.16.0/)
* [LibUV](https://downloads.datastax.com/cpp-driver/ubuntu/18.04/dependencies/libuv/v1.35.0/)

You only need the above to use the DBAL library.

### Ubuntu Quick Steps

    declare -a packages=(
        "http://security.ubuntu.com/ubuntu/pool/main/g/glibc/multiarch-support_2.27-3ubuntu1.2_amd64.deb"
        "https://downloads.datastax.com/cpp-driver/ubuntu/18.04/cassandra/v2.16.0/cassandra-cpp-driver_2.16.0-1_amd64.deb"
        "https://downloads.datastax.com/cpp-driver/ubuntu/18.04/dependencies/libuv/v1.35.0/libuv1_1.35.0-1_amd64.deb"
    )    
    for pkg in "${packages[@]}"; do wget -O /tmp/pkg.deb "${pkg}" && dpkg -i /tmp/pkg.deb && rm -f /tmp/pkg.deb; done 

Building Libraries
------------------
To build an application, use the `build` command with a Bazel build target path:

    bazel build //:templating
    bazel build //:all
    
Testing
-------
See [Testing](Testing.md).
