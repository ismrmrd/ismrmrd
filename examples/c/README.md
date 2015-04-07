This is an example of a simple C project that is built on ISMRMRD.

Instructions for building:

1. Install ISMRMRD and CMake
2. Define the environment variable ISMRMRD_HOME,
   e.g. `export ISMRMRD_HOME=/usr/local/ismrmrd`
3. Compile the example:

        mkdir build
        cmake ..
        make
        ./ismrmrd_c_example
