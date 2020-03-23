#!/bin/bash
## see: https://apt.kitware.com

set -x
set -u
set -e

# If you are using a minimal Ubuntu image or a Docker image, you may need to install the following packages
sudo apt-get update
sudo apt-get install apt-transport-https ca-certificates gnupg software-properties-common wget

# Obtain a copy of kitware signing key
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | sudo apt-key add -

RELEASE=$(lsb_release -a)

if [[ ${RELEASE} =~ ^(bionic|xenial)$ ]]
then
	sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ ${RELEASE} main'
else
	exit 1
fi

sudo apt-get update
sudo apt-get install cmake
