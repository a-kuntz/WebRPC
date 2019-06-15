#!/bin/bash

set -e
set -u
set -x

BUILD_DIR=build

cmake -B${BUILD_DIR} -H. \
	-DWEBRPC_USE_ADDRESS_SANITIZER \
	-DCMAKE_BUILD_TYPE=debug \
	-DCMAKE_INSTALL_PREFIX=./fsdev/

CTEST_OUTPUT_ON_FAILURE=1 make -C ${BUILD_DIR} all test
