#!/bin/bash

set -e
set -u
set -x

# apt update

sudo apt install -y \
	cmake \
	conan \
	clang-format
