#!/bin/bash

set -e
set -u
set -x

BUILD_DIR=build

cmake -B${BUILD_DIR} -H. \
	-DCMAKE_BUILD_TYPE=debug \
	-DCMAKE_INSTALL_PREFIX=.

CTEST_OUTPUT_ON_FAILURE=1 make -C ${BUILD_DIR} all test
