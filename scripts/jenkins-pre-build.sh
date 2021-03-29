#!/bin/bash

# Jenkins Pre Build script
#   - Ubuntu 18.04
#

apt update
apt install git cmake build-essential curl zip unzip tar pkg-config -y

## current dir - RPC source directory
#export BASE_DIR="$PWD"

#cd "$BASE_DIR"/rpc
## build vcpkg
git -C vcpkg pull || git clone --branch 2020.11-1 https://github.com/microsoft/vcpkg.git vcpkg
cd vcpkg
./bootstrap-vcpkg.sh

## install CPPRestSDK
./vcpkg install cpprestsdk[websockets]
