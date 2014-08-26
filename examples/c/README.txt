This is an example of a simple C project that is built on ISMRMRD.

Instructions for building:
1. Install ISMRMRD
2. Define the environment variable ISMRMRD_HOME,
   e.g. ISMRMRD_HOME=/usr/local/ismrmrd
3. In a directory of your choosing:
   mkdir buildcdemo
   cmake ${ISMRMRD_HOME}/examples/c
   make
   ./ismrmrd_c_demo