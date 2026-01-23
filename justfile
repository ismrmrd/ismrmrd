set shell := ['bash', '-ceuo', 'pipefail']

cpp_version := "17"
build_type := "RelWithDebInfo"

@default: test

@all: test build-docs build-docker build-static build-cpp98 build-conda

@configure:
    mkdir -p build; \
    cd build; \
    cmake -GNinja \
        -D CMAKE_BUILD_TYPE={{ build_type }} \
        -D CMAKE_CXX_STANDARD={{ cpp_version }} \
        -D CMAKE_INSTALL_PREFIX=$(conda info --json | jq -r .default_prefix) \
        ..

@autoconfigure:
    if [ ! -f "build/build.ninja" ]; then \
        echo "Ninja file not found. Running cmake..."; \
        just configure; \
    fi

@build: autoconfigure
    cd build && ninja

@test: build
    cd build/tests && ./test_ismrmrd

@install: test
    cd build && ninja install

@build-docker:
    ./docker/build-images.sh; \
    ./docker/test-images.sh

@build-static:
    ./docker/build-and-test-variant.sh static

@build-cpp98:
    ./docker/build-and-test-variant.sh cpp98

@build-docs: autoconfigure
    cd build && ninja doc

@build-conda:
    bash -il ./conda/setup-conda-build.sh; \
    cd conda && ./package.sh
