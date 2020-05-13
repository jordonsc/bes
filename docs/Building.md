Building the Platform
=====================
The entire build platform is managed using [Bazel](https://bazel.build/). Look to the 
[Local Environment](Local_Environment.md) docs for details on setting up Bazel and requisite tools.

Supported Compilers
-------------------
Currently, this project is only tested on GCC. It's recommended to use at least GCC version 8 or above.

If you're using an OS that supports multiple versions, you can either symlink to the correct version or instruct Bazel
on which to use. Consider:

    # Ensure Bazel looks to the correct GCC version:
    export CC="/usr/bin/gcc-8"
    
    # -- OR -- 

    # Set GCC to use GCC-8 system wide
    ln -sf /usr/bin/gcc-8 /usr/bin/gcc
    ln -sf /usr/bin/g++-8 /usr/bin/g++


Building Libraries
------------------
To build an application, use the `build` command with a Bazel build target path:

    bazel build //lib:templating
    bazel build //lib:all
    

Running Tests
-------------
As above, but with the `test` Bazel command:

    `bazel test //test:all`

The `.bazelrc` file will instruct the test command output only to be verbose on failure.

