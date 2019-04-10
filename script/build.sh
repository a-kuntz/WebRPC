#!/bin/bash

set -e
set -u
set -x

SCRIPT_DIR=$( cd "$(dirname "${BASH_SOURCE}")" ; pwd -P )
DEPS_DIR=${SCRIPT_DIR}/../deps

pushd .
mkdir -p build
cd build
cmake .. \
	-DBOOST_ROOT=${DEPS_DIR}/boost_1_69_0 \
	-DGTEST_ROOT=${DEPS_DIR}/googletest-release-1.8.1/build \
	-DCMAKE_BUILD_TYPE=debug \
	-DCMAKE_INSTALL_PREFIX=.
CTEST_OUTPUT_ON_FAILURE=1 make all test
popd
