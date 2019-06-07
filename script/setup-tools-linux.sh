#!/bin/bash

set -e
set -u
set -x

# sudo apt-get update

sudo apt install -y \
	cmake \
	python3-pip \
	clang-format

sudo -H pip3 install conan

conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan

set +x

COLOR="\e[34m"	# blue
RESET="\e[39m"

echo -e "
make sure to add the following settings to your ~/.conan/profiles/default
${COLOR}
[settings]
os=Linux
os_build=Linux
arch=x86_64
arch_build=x86_64
build_type=Release
compiler=gcc
compiler.libcxx=libstdc++11
compiler.version=7
[options]
[build_requires]
[env]
${RESET}
"
