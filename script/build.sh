#!/bin/bash

set -e
set -x

pushd .
mkdir -p build
cd build
cmake .. \
	-DBOOST_ROOT=${HOME}/deps/boost_1_69_0 \
	-DGTEST_ROOT=${HOME}/deps/googletest-release-1.8.1/build \
	-DCMAKE_BUILD_TYPE=debug \
	-DCMAKE_INSTALL_PREFIX=.
make all test
popd
