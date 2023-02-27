#!/bin/bash

set -eo pipefail

if [ -n "${HOST_WORKSPACE_DIR:-}" ]; then
    WORKDIR="${HOST_WORKSPACE_DIR:-}"
else
    WORKDIR="$(pwd)"
fi

rm -rf testdata.h5
rm -rf images.h5

docker run --rm -v "${WORKDIR}":/tmp ismrmrd ismrmrd_generate_cartesian_shepp_logan -o /tmp/testdata.h5
docker run -i --rm -v "${WORKDIR}":/tmp ismrmrd ismrmrd_hdf5_to_stream -i /tmp/testdata.h5 --use-stdout \
    | docker run -i --rm ismrmrd-stream-recon \
    | docker run -i --rm -v "${WORKDIR}":/tmp ismrmrd ismrmrd_stream_to_hdf5 -o /tmp/images.h5 --use-stdin
