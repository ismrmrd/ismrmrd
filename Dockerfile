#########################################################
# file-normalizer stage
# In order to use BuildKit remote caching, input files must have
# not only the right content hash, but also the right permissions.
# Git only tracks whether the owner can execute a file.
# Here we bring in all files that are going to be used in the
# subsequent stage and normalize the permissions.
#########################################################

ARG USERNAME="vscode"
ARG USER_UID=1000
ARG USER_GID=$USER_UID

FROM mcr.microsoft.com/oss/busybox/busybox:1.33.1 as file-normalizer

COPY environment.yml /data/
RUN chmod -R 555 /data/

RUN mkdir -p /entrypoints
COPY docker/entrypoint.sh /entrypoints/
COPY docker/entrypoint-stream.sh /entrypoints/

RUN sed -i 's/\r$//' /entrypoints/*.sh
RUN chmod +x /entrypoints/*.sh

FROM mcr.microsoft.com/vscode/devcontainers/base:0.201.8-focal AS basecontainer

# Install needed packages and setup non-root user.
ARG USERNAME
ARG USER_UID
ARG USER_GID

ARG CONDA_GID=900
ARG CONDA_ENVIRONMENT_NAME=ismrmrd

RUN apt-get update && apt-get install -y \
    libc6-dbg \
    && rm -rf /var/lib/apt/lists/*

ARG MAMBA_VERSION=1.3.1

# Based on https://github.com/ContinuumIO/docker-images/blob/master/miniconda3/debian/Dockerfile.
RUN wget --quiet https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh -O miniconda.sh \
    && mkdir -p /opt \
    && sh miniconda.sh -b -p /opt/conda \
    && ln -s /opt/conda/etc/profile.d/conda.sh /etc/profile.d/conda.sh \
    && find /opt/conda/ -follow -type f -name '*.a' -delete \
    && find /opt/conda/ -follow -type f -name '*.js.map' -delete \
    && /opt/conda/bin/conda install -n base -c conda-forge mamba=${MAMBA_VERSION} \
    && /opt/conda/bin/conda clean -afy \
    && groupadd -r conda --gid ${CONDA_GID} \
    && usermod -aG conda ${USERNAME} \
    && chown -R :conda /opt/conda \
    && chmod -R g+w /opt/conda \
    && find /opt -type d | xargs -n 1 chmod g+s


FROM basecontainer AS devcontainer

ARG USER_UID
ARG USER_GID

# Create a conda environment from the environment file in the repo root.
COPY --from=file-normalizer --chown=$USER_UID:conda /data/environment.yml /tmp/build/
RUN umask 0002 \
    && /opt/conda/bin/mamba env create -f /tmp/build/environment.yml \
    && /opt/conda/bin/mamba clean -fy \
    && sudo chown -R :conda /opt/conda/envs

# Add a section to /etc/bash.bashrc that ensures that a section is present at the end of ~/.bashrc.
# We can't just write to .bashrc from here because it will be overwritten if the devcontainer user has
# opted to use their own dotfiles repo. The dotfiles repo is cloned after the postCreateCommand
# in the devcontainer.json file is executed.
RUN echo "\n\
if ! grep -q \"^source /opt/conda/etc/profile.d/conda.sh\" /home/${USERNAME}/.bashrc; then\n\
	echo \"source /opt/conda/etc/profile.d/conda.sh\" >> /home/${USERNAME}/.bashrc\n\
	echo \"conda activate $(grep 'name:' /tmp/build/environment.yml | awk '{print $2}')\" >> /home/${USERNAME}/.bashrc\n\
fi\n" >> /etc/bash.bashrc

ENV CMAKE_GENERATOR=Ninja

# Create a kits file for the VSCode CMake Tools extension, so you are not prompted for which kit to select whenever you open VSCode
RUN mkdir -p /home/vscode/.local/share/CMakeTools \
    && echo '[{"name":"GCC-11","compilers":{"C":"/opt/conda/envs/ismrmrd/bin/x86_64-conda_cos6-linux-gnu-gcc","CXX":"/opt/conda/envs/ismrmrd/bin/x86_64-conda_cos6-linux-gnu-g++"}}]' > /home/vscode/.local/share/CMakeTools/cmake-tools-kits.json \
    && chown vscode:conda /home/vscode/.local/share/CMakeTools/cmake-tools-kits.json

FROM devcontainer AS ismrmrd-build

ARG USER_UID
ARG USER_GID
SHELL ["/bin/bash", "-c"]
COPY --chown=$USER_UID:$USER_GID . /opt/code/ismrmrd/

RUN . /opt/conda/etc/profile.d/conda.sh && umask 0002 && conda activate ismrmrd && mkdir -p /opt/package && \
    cd /opt/code/ismrmrd && \
    mkdir build && \
    cd build && \
    cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/package ../ && \
    ninja && \
    ninja install

FROM ismrmrd-build AS ismrmrd-test

ENTRYPOINT [ "/opt/code/ismrmrd/docker/entrypoint.sh", "python", "/opt/code/ismrmrd/docker/validate_results.py" ]

FROM basecontainer AS ismrmrd

ARG USER_UID

RUN apt-get update && apt-get install -y socat \
    && rm -rf /var/lib/apt/lists/*

COPY --from=file-normalizer --chown=$USER_UID:conda /data/environment.yml /tmp/build/
RUN grep -v "#.*\<dev\>" /tmp/build/environment.yml > /tmp/build/filtered_environment.yml \
    && mv /tmp/build/filtered_environment.yml /tmp/build/environment.yml \
    && umask 0002 \
    && /opt/conda/bin/mamba env create -f /tmp/build/environment.yml \
    && /opt/conda/bin/mamba clean -fy \
    && sudo chown -R :conda /opt/conda/envs

COPY --from=ismrmrd-build --chown=$USER_UID:conda /opt/package /opt/conda/envs/ismrmrd/
COPY --from=file-normalizer --chown=$USER_UID:conda /entrypoints/entrypoint.sh /opt/
COPY --from=file-normalizer --chown=$USER_UID:conda /entrypoints/entrypoint-stream.sh /opt/

ENTRYPOINT [ "/opt/entrypoint.sh" ]


FROM ismrmrd AS ismrmrd-stream-recon

ENTRYPOINT [ "/opt/entrypoint-stream.sh" ]

FROM ismrmrd AS ismrmrd-stream-recon-server

ENTRYPOINT [ "/opt/entrypoint-stream.sh", "--socket-port", "9002" ]
EXPOSE 9002
