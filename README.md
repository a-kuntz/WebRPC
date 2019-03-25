# WebRPC

A web based RPC library.

## Dependencies

- Boost (v 1.69.0)
  - Boost.Spirit X3
  - Boost.Asio
- GTest (v. 1.8.1)

## Build instructions

You build WebRPC as follows, assuming Boost is located in `~/dev/boost_1_69_0` and Gtest in `~/dev/googletest-release-1.8.1/build`:

    mkdir build
    cd build
    cmake .. \
      -DBOOST_ROOT=~/dev/boost_1_69_0 \
      -DGTEST_ROOT=~/dev/googletest-release-1.8.1/build \
      -DCMAKE_BUILD_TYPE=debug \
      -DCMAKE_INSTALL_PREFIX=.
    make all test
