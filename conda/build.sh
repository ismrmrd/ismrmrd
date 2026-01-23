#!/bin/bash

set -euo pipefail

mkdir -p build-conda
cd build-conda

cmake -G Ninja \
      -D CMAKE_BUILD_TYPE=Release \
      -D CMAKE_CXX_STANDARD=17 \
      -D CMAKE_INSTALL_PREFIX=${PREFIX} \
      ../

ninja
cd tests && ./test_ismrmrd && cd ../
ninja install
