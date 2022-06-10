# Build instructions

The ISMRM Raw Data format is described by an XML schema and some C-style structs with fixed memory layout and as such does not have dependencies. However, it uses HDF5 files for storage and a C++ library for reading and writing the ISMRMRD files is included in this distribution. Furthermore, since the XML header is defined with an XML schema, we encourage using XML data binding when writing software using the format. To compile all components of this distribution you need:

* HDF5 (version 1.8 or higher) libraries. Available from http://www.hdfgroup.org/downloads/index.html.
* Boost (http://www.boost.org/)
* Cmake build tool (http://www.cmake.org/)
* Git if you would like to use the source code archive (http://git-scm.com/)
* FFTW if you would like to compile some of the example applications
  (http://www.fftw.org)
* Doxygen if you would like to generate API documentation (http://www.doxygen.org)

> It is only necessary to install the dependencies if you wish to develop compiled C/C++ software, which uses the ISMRMRD format. The format can be read in Matlab or Python without installing any additional software.

## Linux installation

The dependencies mentioned above should be included in most linux distributions. On Ubuntu you can install all required dependencies with::

```bash
sudo apt-get -y install doxygen git-core graphviz libboost-all-dev libfftw3-dev libhdf5-serial-dev
```

After installation of dependencies, the library can be installed with:

```bash
git clone https://github.com/ismrmrd/ismrmrd
cd ismrmrd/
mkdir build
cd build
cmake ../
make
sudo make install
```

This will install the library in `/usr/local/` by default. To specify
an alternative installation directory, pass `-D CMAKE_INSTALL_PREFIX=<install dir>` to `cmake`.


## External use of ISMRMRD C++ library in other projects

To use ISMRMRD for your externally developed projects, add the following to your CMakeLists.txt file:

```
find_package( ISMRMRD REQUIRED )
link_directories( ${ISMRMRD_LIBRARY_DIRS} )
include_directories( ${ISMRMRD_INCLUDE_DIRS} )
target_link_libraries( mytarget ${ISMRMRD_LIBRARIES} )
```

then when configuring your package use set the following cmake variables (command line variant shown):

```
cmake -DISMRMRD_DIR:PATH=<path to build/install tree of ISMRMRD> <path to my source tree>
```