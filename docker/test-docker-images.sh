#!/bin/bash

set -eo pipefail

if [ -n "${HOST_WORKSPACE_DIR:-}" ]; then
    WORKDIR="${HOST_WORKSPACE_DIR:-}"
else
    WORKDIR="$(pwd)"
fi

# if socat is not installed install it using apt
if ! command -v socat &> /dev/null; then
    sudo apt-get update
    sudo apt-get install socat
fi

rm -rf testdata.h5
rm -rf "*images.h5"

docker run --rm -v "${WORKDIR}":/tmp ghcr.io/ismrmrd/ismrmrd ismrmrd_generate_cartesian_shepp_logan -o /tmp/testdata.h5 -n 0.0

# Run the standard streaming reconstruction with standard complex images
docker run -i --rm -v "${WORKDIR}":/tmp ghcr.io/ismrmrd/ismrmrd ismrmrd_hdf5_to_stream -i /tmp/testdata.h5 --use-stdout \
    | docker run -i --rm ghcr.io/ismrmrd/ismrmrd-stream-recon \
    | docker run -i --rm -v "${WORKDIR}":/tmp ghcr.io/ismrmrd/ismrmrd ismrmrd_stream_to_hdf5 -o /tmp/cplx_images.h5 --use-stdin

docker run --rm -v "${WORKDIR}":/tmp ghcr.io/ismrmrd/ismrmrd-test --reference /tmp/testdata.h5 --testdata /tmp/cplx_images.h5

# Run the standard streaming reconstruction with magnitude images
docker run -i --rm -v "${WORKDIR}":/tmp ghcr.io/ismrmrd/ismrmrd ismrmrd_hdf5_to_stream -i /tmp/testdata.h5 --use-stdout \
    | docker run -i --rm ghcr.io/ismrmrd/ismrmrd ismrmrd_stream_recon_cartesian_2d --use-stdin --use-stdout --output-magnitude \
    | docker run -i --rm -v "${WORKDIR}":/tmp ghcr.io/ismrmrd/ismrmrd ismrmrd_stream_to_hdf5 -o /tmp/mag_images.h5 --use-stdin

docker run --rm -v "${WORKDIR}":/tmp ghcr.io/ismrmrd/ismrmrd-test --reference /tmp/testdata.h5 --testdata /tmp/mag_images.h5

# Set error trap
trap 'docker rm -f ismrmrd-stream-server' EXIT

# Start stream server
docker run --name ismrmrd-stream-server -d -p 9010:9002 ghcr.io/ismrmrd/ismrmrd-stream-recon-server

# Sleep for 5 seconds to allow the server to start
sleep 5

docker run -i --rm -v "${WORKDIR}":/tmp ghcr.io/ismrmrd/ismrmrd ismrmrd_hdf5_to_stream -i /tmp/testdata.h5 --use-stdout \
    | socat -t10 - TCP4:localhost:9010 \
    | docker run -i --rm -v "${WORKDIR}":/tmp ghcr.io/ismrmrd/ismrmrd ismrmrd_stream_to_hdf5 -o /tmp/socket_images.h5 --use-stdin


docker run --rm -v "${WORKDIR}":/tmp ghcr.io/ismrmrd/ismrmrd-test --reference /tmp/testdata.h5 --testdata /tmp/socket_images.h5
