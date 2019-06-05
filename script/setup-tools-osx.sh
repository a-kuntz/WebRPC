#!/bin/bash

set -e
set -u
set -x

brew update
brew install cmake
brew install conan
brew install clang-format
