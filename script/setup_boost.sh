#!/bin/bash

set -e
set -x

# ARCHIVE=~/Downloads/boost_1_68_0.tar.gz
ARCHIVE=~/Downloads/boost_1_69_0.tar.gz
DST_DIR=~/deps/
LIB_ROOT=$(basename ${ARCHIVE} .tar.gz)

pushd .
mkdir -p ${DST_DIR}
cd ${DST_DIR}

tar -xzf ${ARCHIVE}
# cd ${LIB_ROOT}
#./bootstrap.sh --show-libraries
# ./bootstrap.sh --prefix=. --with-libraries=chrono,date_time,system
# ./b2 install

popd
