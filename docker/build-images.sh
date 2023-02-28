#!/bin/bash

set -eo pipefail

tag="latest"
image_base_name="ghcr.io/ismrmrd"

# parse command line arguments
while [[ $# -gt 0 ]]; do
    key="$1"
    case $key in
        --tag)
            tag="$2"
            shift
            shift
            ;;
        --image-base-name)
            image_base_name="$2"
            shift
            shift
            ;;
        *)
            echo "Unknown option $key"
            exit 1
            ;;
    esac
done

# Dockerfile is in the root director one level up from this script
dockerfile="$(dirname "${BASH_SOURCE[0]}")/../Dockerfile"
context="$(dirname "${BASH_SOURCE[0]}")/.."

# Array for targets to build
targets=( "ismrmrd-test" "ismrmrd" "ismrmrd-stream-recon" "ismrmrd-stream-recon-server" )

# Loop over targets
for target in "${targets[@]}"; do
    echo "Building ${target}..."
    docker build -t "${image_base_name}/${target}:${tag}" -f "$dockerfile" "$context" --target "$target"
done
