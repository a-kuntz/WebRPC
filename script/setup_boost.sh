#!/bin/bash

set -e
set -x

ARCHIVE=~/Downloads/boost_1_69_0.tar
DST_DIR=~/deps/
LIB_ROOT=$(basename ${ARCHIVE} .tar)

pushd .
mkdir -p ${DST_DIR}
cd ${DST_DIR}

tar -xzf ${ARCHIVE}

pushd