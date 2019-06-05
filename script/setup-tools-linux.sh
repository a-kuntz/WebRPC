#!/bin/bash

set -e
set -u
set -x

# apt update

sudo apt install -y \
	cmake \
	python-pip \
	clang-format

sudo pip install conan