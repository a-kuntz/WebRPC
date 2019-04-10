#!/bin/bash

set -e
set -u
set -x

SCRIPT_DIR=$( cd "$(dirname "${BASH_SOURCE}")" ; pwd -P )
DEPS_DIR=${SCRIPT_DIR}/../deps

# ARCHIVE=~/Downloads/boost_1_68_0.tar.gz
ARCHIVE=~/Downloads/boost_1_69_0.tar.gz
LIB_ROOT=$(basename ${ARCHIVE} .tar.gz)

pushd .
mkdir -p ${DEPS_DIR}
cd ${DEPS_DIR}

tar -xzf ${ARCHIVE}
cd ${LIB_ROOT}
#./bootstrap.sh --show-libraries
#- atomic
#- chrono
#- container
#- context
#- contract
#- coroutine
#- date_time
#- exception
#- fiber
#- filesystem
#- graph
#- graph_parallel
#- iostreams
#- locale
#- log
#- math
#- mpi
#- program_options
#- python
#- random
#- regex
#- serialization
#- stacktrace
#- system
#- test
#- thread
#- timer
#- type_erasure
#- wave
 ./bootstrap.sh --prefix=. --with-libraries=filesystem,program_options
 ./b2 install

popd
