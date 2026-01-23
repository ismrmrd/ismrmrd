#!/usr/bin/env bash

set -e

conda install -y -n base conda-build anaconda-client
conda activate base
