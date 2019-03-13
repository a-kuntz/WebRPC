#!/bin/bash

pushd .
mkdir build
cd build
cmake .. -DBOOST_ROOT=~/dev/boost_1_69_0 -DCMAKE_BUILD_TYPE=debug
make
popd
