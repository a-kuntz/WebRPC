#!/bin/bash

set -e
set -u
set -x

BUILD_DIR=build

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

conan install ..

cmake .. \
	-DCMAKE_BUILD_TYPE=debug \
	-DCMAKE_INSTALL_PREFIX=.

CTEST_OUTPUT_ON_FAILURE=1 make all test

cd -
