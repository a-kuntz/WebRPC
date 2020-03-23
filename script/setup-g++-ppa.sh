#!/bin/bash

sudo apt install software-properties-common
sudo add-apt-repository ppa:ubuntu-toolchain-r/test

sudo apt-get g++-9

sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 90 --slave /usr/bin/g++ g++ /usr/bin/g++-9
