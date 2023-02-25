#!/bin/bash

set -eo pipefail

socket_port=""

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    key="$1"
    case $key in
        --socket-port)
            socket_port="$2"
            shift
            shift
            ;;
        *)
            echo "Unknown option $key"
            exit 1
            ;;
    esac
done

# enable conda for this shell
# shellcheck disable=SC1091
. /opt/conda/etc/profile.d/conda.sh

# activate the environment
conda activate ismrmrd

if [ -z "$socket_port" ]; then
    exec ismrmrd_stream_recon_cartesian_2d --use-stdin --use-stdout
else
    exec socat TCP4-LISTEN:$socket_port,fork,reuseaddr "EXEC:ismrmrd_stream_recon_cartesian_2d --use-stdin --use-stdout"
fi
