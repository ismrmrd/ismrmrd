#!/bin/bash

set -eo pipefail

# Parse command-line argument
if [ $# -ne 1 ]; then
    echo "Usage: $0 [static|cpp98]" >&2
    echo "  static - Build and test static variant" >&2
    echo "  cpp98  - Build and test C++98 variant" >&2
    exit 1
fi

variant="$1"

# Dockerfile is in the root director one level up from this script
dockerfile="$(dirname "${BASH_SOURCE[0]}")/../Dockerfile.build-variants"
context="$(dirname "${BASH_SOURCE[0]}")/.."

case "$variant" in
    "static")
        echo "Building and testing static variant..."
        docker build -f "$dockerfile" "$context" \
            --build-arg UBUNTU_VARIANT=noble \
            --build-arg CXX_STANDARD=17 \
            --build-arg BUILD_STATIC=On \
            -t ismrmrd-static

        test_command="set -euo pipefail \
                && export PATH=$PATH:/opt/package/bin \
                && test -f /opt/package/lib/libismrmrd.a \
                && ! test -e /opt/package/lib/libismrmrd.so \
                && ismrmrd_generate_cartesian_shepp_logan -o testdata.h5 \
                && ismrmrd_hdf5_to_stream -i testdata.h5 --use-stdout \
                | ismrmrd_stream_recon_cartesian_2d --use-stdin --use-stdout \
                | ismrmrd_stream_to_hdf5 --use-stdin -o result.h5"
        docker run --rm ismrmrd-static /bin/bash -c "$test_command"
        ;;

    "cpp98")
        echo "Building and testing C++98 variant..."
        docker build -f "$dockerfile" "$context" \
            --build-arg UBUNTU_VARIANT=jammy \
            --build-arg CXX_STANDARD=98 \
            --build-arg BUILD_STATIC=Off \
            -t ismrmrd-cpp98

        test_command="set -euo pipefail \
                && export PATH=$PATH:/opt/package/bin \
                && ismrmrd_generate_cartesian_shepp_logan -o testdata.h5 \
                && ismrmrd_hdf5_to_stream -i testdata.h5 --use-stdout \
                | ismrmrd_stream_recon_cartesian_2d --use-stdin --use-stdout \
                | ismrmrd_stream_to_hdf5 --use-stdin -o result.h5"
        docker run --rm ismrmrd-cpp98 /bin/bash -c "$test_command"
        ;;

    *)
        echo "Error: Invalid argument '$variant'. Must be either 'static' or 'cpp98'." >&2
        echo "Usage: $0 [static|cpp98]" >&2
        exit 1
        ;;
esac
