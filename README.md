# WebRPC

WebRPC is a web based remote procedure call (RPC) library.

E.g. a remote method `DateTime` may be called via `http://localhost/DateTime` from any software that is capable of emitting http requests. For instance, this can be your browser or the WebRPC sampleclient.

## Sample Session

Start sampleserver which is located in the build folder

    ./build/sampleserver

Call method `DateTime` via sampleclient, which is also locate in the build folder

    ./build/sampleclient http://localhost:8080/DateTime

Call method `Sum` with an array of integer and double values `[1,2.3,4e-5,-6]`

    $ ./build/sampleclient http://localhost:8080/Sum/[1,2.3,4e-5,-6]
    -2.69996

## Cloning and building WebRPC

    git clone https://github.com/yoshiii/WebRPC.git
    cd WebRPC
    mkdir build
    cd build
    cmake ..
    make all test

## Dependencies

- Boost (v 1.69.0)
  - Boost.Spirit X3
  - Boost.Asio
  - Boost.Filesystem
  - Boost.Program_options
- GTest (v. 1.8.1)

## Building WebRPC and all its dependencies from scratch

In case your system does not provide `boost` and `gtest` in the required versions, you can use the setup scripts `scripts/setup_boost.sh` and `scripts/setup_gtest.sh` to unpack and install the sources to folder `./deps` in your working copy.

To compile WebRPC set `BOOST_ROOT` and `GTEST_ROOT` when invoking cmake. There is also a convenience script `scripts/build.sh`.

    mkdir build
    cd build
    cmake .. \
      -DBOOST_ROOT=../deps/boost_1_69_0 \
      -DGTEST_ROOT=../deps/googletest-release-1.8.1/build \
      -DCMAKE_BUILD_TYPE=debug \
      -DCMAKE_INSTALL_PREFIX=.
    make all test
