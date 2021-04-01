#!/bin/bash

set -x

# Jenkins Build script
#   - Ubuntu 18.04
#
export BASE_DIR="$PWD"
export CMAKE_CXX_FLAGS="-isystem /usr/lib/gcc/x86_64-linux-gnu/7/include"


if [ -d "build" ]; then
  rm -rf build
fi

mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE="$BASE_DIR"/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
