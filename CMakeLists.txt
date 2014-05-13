cmake_minimum_required(VERSION 2.8)
project(ISMRMRD)

enable_testing()

# define install paths globally
set(ISMRMRD_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/ismrmrd)
set(ISMRMRD_INSTALL_LIB_DIR ${ISMRMRD_INSTALL_PREFIX}/lib)
set(ISMRMRD_INSTALL_INCLUDE_DIR ${ISMRMRD_INSTALL_PREFIX}/include)
set(ISMRMRD_INSTALL_BIN_DIR ${ISMRMRD_INSTALL_PREFIX}/bin)
set(ISMRMRD_INSTALL_MODULE_DIR ${ISMRMRD_INSTALL_PREFIX}/cmake)
set(ISMRMRD_INSTALL_SCHEMA_DIR ${ISMRMRD_INSTALL_PREFIX}/schema)
set(ISMRMRD_INSTALL_MATLAB_DIR ${ISMRMRD_INSTALL_PREFIX}/matlab)
set(ISMRMRD_INSTALL_JAVA_DIR ${ISMRMRD_INSTALL_PREFIX}/java)
set(ISMRMRD_INSTALL_PYTHON_DIR ${ISMRMRD_INSTALL_PREFIX}/python)
set(ISMRMRD_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(ISMRMRD_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(ISMRMRD_SCHEMA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/schema)
set(ISMRMRD_SCHEMA_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/src/xsd)

# add project specific cmake find modules
list(APPEND CMAKE_MODULE_PATH ${ISMRMRD_MODULE_DIR})

if (WIN32)
    add_definitions(-DWIN32 -D_WIN32 -D_WINDOWS)
    add_definitions(-DUNICODE -D_UNICODE)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
endif (WIN32)

# required packages for main library
find_package(Boost COMPONENTS thread system REQUIRED)
find_package(HDF5 1.8 COMPONENTS C CXX REQUIRED)

if(WIN32)
    set(Boost_NO_BOOST_CMAKE ON)
endif(WIN32)

if(WIN32)
  add_definitions( -DBOOST_ALL_NO_LIB )
  add_definitions( -DBOOST_ALL_DYN_LINK )
endif(WIN32)

add_library(ismrmrd SHARED ismrmrd_hdf5.cpp)
include_directories(${HDF5_C_INCLUDE_DIR} ${HDF5_CXX_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR})

#The findHDF5.cmake does not work very well on Windows, let's help it out
if(WIN32)
    target_link_libraries(ismrmrd optimized ${HDF5_hdf5_LIBRARY_RELEASE}
        optimized ${HDF5_hdf5_cpp_LIBRARY_RELEASE})
    target_link_libraries(ismrmrd debug ${HDF5_hdf5_LIBRARY_DEBUG}
        debug ${HDF5_hdf5_cpp_LIBRARY_DEBUG})
    target_link_libraries(ismrmrd ${Boost_LIBRARIES})
else(WIN32)
    target_link_libraries(ismrmrd ${HDF5_LIBRARIES} ${Boost_THREAD_LIBRARY}
        ${Boost_SYSTEM_LIBRARY})
endif(WIN32)

install(TARGETS ismrmrd
    DESTINATION ${ISMRMRD_INSTALL_LIB_DIR})

install(FILES ${ISMRMRD_SOURCE_DIR}/ismrmrd.h
    ${ISMRMRD_SOURCE_DIR}/ismrmrd_hdf5.h
    ${ISMRMRD_SOURCE_DIR}/ismrmrd_hdf5_datatypes.h
    ${ISMRMRD_SOURCE_DIR}/ismrmrd_export.h
    DESTINATION ${ISMRMRD_INSTALL_INCLUDE_DIR})

# required packages for support xsd library
find_package(XSD REQUIRED)
find_package(XercesC REQUIRED)

# process the XSD files
set(XSDS ${ISMRMRD_SCHEMA_DIR}/ismrmrd.xsd)
set(XSD_ARGS cxx-tree --generate-serialization)
#SET(XSD_ARGS cxx-tree --generate-serialization --export-symbol EXPORTISMRMRDXSD --hxx-prologue-file ${CMAKE_SOURCE_DIR}/ismrmrd_xsd_export.h)
wrap_xsd(XSDS_SOURCES XSD_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/schema ${XSDS}
    OPTIONS ${XSD_ARGS})
#include_directories(${ISMRMRD_SCHEMA_SOURCE_DIR} ${XSD_INCLUDE_DIR}
    #${XERCESC_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${HDF5_CXX_INCLUDE_DIR}
    #${HDF5_C_INCLUDE_DIR})
#link_directories(${Boost_LIBRARY_DIRS})

add_library(ismrmrd_xsd SHARED ${XSDS_SOURCES})
include_directories(${XERCESC_INCLUDE_DIR})
target_link_libraries(ismrmrd_xsd ${XERCESC_LIBRARIES})

install(FILES ${XSDS} ${XSDS_SOURCES}
    DESTINATION ${ISMRMRD_INSTALL_SCHEMA_DIR})

install(TARGETS ismrmrd_xsd
    DESTINATION ${ISMRMRD_INSTALL_LIB_DIR})

install(FILES ${ISMRMRD_MODULE_DIR}/FindIsmrmrd.cmake
    ${ISMRMRD_MODULE_DIR}/FindFFTW3.cmake
    ${ISMRMRD_MODULE_DIR}/FindXSD.cmake
    ${ISMRMRD_MODULE_DIR}/FindXercesC.cmake
    ${ISMRMRD_MODULE_DIR}/FindNumPy.cmake
    DESTINATION ${ISMRMRD_INSTALL_MODULE_DIR})

add_subdirectory(examples/c++)
add_subdirectory(utilities)
add_subdirectory(tests)
add_subdirectory(doc)
add_subdirectory(matlab)
add_subdirectory(bindings)
