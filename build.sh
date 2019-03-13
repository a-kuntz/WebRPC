#!/bin/bash

pushd .
mkdir build
cd build
cmake .. -DBOOST_ROOT=~/dev/boost_1_69_0
make
popd
