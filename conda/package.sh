#!/bin/bash
set -euo pipefail

usage()
{
  cat << EOF

Builds the ismrmrd conda package.

Usage: $0
EOF
}

output_path="$(dirname "$0")/build_pkg"

# Build up channel directives
channels=(
  conda-forge
)

channel_directives=$(printf -- "-c %s " "${channels[@]}")

mkdir -p "$output_path"
conda install -n base conda-libmamba-solver
conda config --set solver libmamba
bash -c "conda build --no-anaconda-upload --output-folder $output_path $channel_directives $(dirname "$0")"
