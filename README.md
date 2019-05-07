# WebRPC

WebRPC implements the idea of providing a call to a remote library as a simple call to an URL. In other words, it is a web based remote procedure call (RPC) library.

## Key features

- make your remote library functionality accessible through the abstraction of a simple URL
    - call a remote library from your browser via http(s) (and soon via websocket)
    - WebRPC provides a JSON RPC inspired transfer encoding
- simple build
    - modern and clean C++ 17 code
    - cmake based build
    - little dependencies (Boost and GTest only)

## Sample Session

E.g. a remote method `DateTime` may be called via `http://localhost/DateTime` from any software that is capable of emitting http requests. For instance, this can be your browser or the WebRPC sampleclient.

Start sampleserver which is located in the build folder

    $ ./build/sampleserver

Call method `DateTime` through URL `http://localhost:8080/DateTime`. You can use your browser or sampleclient, which is also located in the build folder

    $ ./build/sampleclient http://localhost:8080/DateTime

Call method `Sum` with an array of integer and double values `[1,2.3,4e-5,-6]`

    $ ./build/sampleclient http://localhost:8080/Sum/[1,2.3,4e-5,-6]
    -2.69996

Introspection allows to list the available methods of a server

    $ ./build/sampleclient http://localhost:8080/system.list_methods

## Cloning and building WebRPC

    git clone https://github.com/a-kuntz/WebRPC.git
    cd WebRPC
    cmake -Bbuild -H.
    make -C build all test

## Dependencies

### Libraries

The productive code of the library depends only on Boost libraries. WebRPC unit tests depend on Google Test. Thus WebRPC is lightweight in terms of library dependencies.

- Boost (v 1.69.0)
  - Boost.Spirit X3
  - Boost.Asio
  - Boost.Filesystem
  - Boost.Program_options
- GTest (v. 1.8.1) - for unit tests

### Tools

WebRPC is build using cmake. It compiles and is tested on the following platforms and compilers:

| OS            | Compiler           |
| ------------- | ------------------ |
| Ubuntu 18.04  | gcc                |
| macOS 10.13.x | clang-1000.11.45.5 |

## Building WebRPC and all its dependencies from scratch

In case your system does not provide `boost` and `gtest` in the required versions, you can download the sources from the web and use the setup scripts `scripts/setup_boost.sh` and `scripts/setup_gtest.sh` to unpack and install the sources to folder `./deps` of your WebRPC working copy.

To compile WebRPC set `BOOST_ROOT` and `GTEST_ROOT` when invoking cmake. There is also a convenience script `scripts/build.sh`.

    cmake -Bbuild -H. \
      -DBOOST_ROOT=../deps/boost_1_69_0 \
      -DGTEST_ROOT=../deps/googletest-release-1.8.1/build \
      -DCMAKE_BUILD_TYPE=debug \
      -DCMAKE_INSTALL_PREFIX=.
    make -C build all test
