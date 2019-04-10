#!/bin/bash

set -e
set -u
set -x

SCRIPT_DIR=$( cd "$(dirname "${BASH_SOURCE}")" ; pwd -P )
DEPS_DIR=${SCRIPT_DIR}/../deps

ARCHIVE=~/Downloads/googletest-release-1.8.1.tar.gz
LIB_ROOT=$(basename ${ARCHIVE} .tar.gz)

pushd .
mkdir -p ${DEPS_DIR}
cd ${DEPS_DIR}

tar -xzf ${ARCHIVE}
cd ${LIB_ROOT}
mkdir -p build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=.
make clean all install
cmake .. -DCMAKE_INSTALL_PREFIX=. -DCMAKE_BUILD_TYPE=debug
make clean all install
# ls ./include/gtest/gtest.h ./lib/libgtest*
# find . -name gtest.h -o -name libgtest*

popd
