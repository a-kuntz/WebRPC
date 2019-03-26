#!/bin/bash

set -e
set -x

ARCHIVE=~/Downloads/googletest-release-1.8.1.tar.gz
DST_DIR=~/deps/
LIB_ROOT=$(basename ${ARCHIVE} .tar.gz)

pushd .
mkdir -p ${DST_DIR}
cd ${DST_DIR}

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
