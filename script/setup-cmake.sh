#!/bin/bash

set -e
set -u
set -x

VERSION=3.15.0
ARCHIVE=cmake-${VERSION}.tar.gz
DESTDIR=~/deps

## cmake download via https depends on openssl-dev
## ccmake depends on ncurses-dev
sudo apt-get install -y \
    libcurl4-openssl-dev \
    libncurses5-dev

mkdir -p ${DESTDIR}
pushd ${DESTDIR}
wget https://github.com/Kitware/CMake/releases/download/v${VERSION}/${ARCHIVE} -O ${ARCHIVE}
tar -xzf ${ARCHIVE}

pushd cmake-${VERSION}
cmake . -DCMAKE_USE_SYSTEM_CURL=ON -DBUILD_CursesDialog=ON
make
make DESTDIR=${DESTDIR}/fs/ install
popd
popd
