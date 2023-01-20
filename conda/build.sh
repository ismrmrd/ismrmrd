#!/bin/bash

set -euo pipefail

mkdir -p build
cd build

cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${PREFIX} \
      -DUSE_SYSTEM_PUGIXML=ON \
      ../

ninja
cd tests && ./test_ismrmrd && cd ../
ninja install
