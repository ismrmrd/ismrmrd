#!/bin/bash

set -euo pipefail

mkdir -p build-conda
cd build-conda

cmake_args=()
if [[ -n "${CONDA_BUILD_SYSROOT:-}" ]]; then
    cmake_args+=("-DCMAKE_OSX_SYSROOT=${CONDA_BUILD_SYSROOT}")
fi

cmake -G Ninja \
      -D CMAKE_BUILD_TYPE=Release \
      -D CMAKE_CXX_STANDARD=17 \
      -D CMAKE_INSTALL_PREFIX="${PREFIX}" \
      "${cmake_args[@]}" \
      ../

ninja
cd tests && ./test_ismrmrd && cd ../
ninja install
