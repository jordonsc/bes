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

Building Libraries
------------------
To build an application, use the `build` command with a Bazel build target path:

    bazel build //:templating
    bazel build //:all
    
Testing
-------
See [Testing](Testing.md).
