#!/bin/bash

pushd .
mkdir build
cd build
cmake .. \
	-DBOOST_ROOT=~/dev/boost_1_69_0 \
	-DGTEST_ROOT=~/dev/googletest-release-1.8.1/build \
	-DCMAKE_BUILD_TYPE=debug \
	-DCMAKE_INSTALL_PREFIX=.
make all test
popd
