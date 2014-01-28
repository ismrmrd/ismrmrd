cmake_minimum_required(VERSION 2.8)
project(ISMRMRD)

enable_testing()

if (WIN32)
ADD_DEFINITIONS(-DWIN32 -D_WIN32 -D_WINDOWS)
ADD_DEFINITIONS(-DUNICODE -D_UNICODE)
ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
endif (WIN32)

set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/ismrmrd)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(XSD REQUIRED)
find_package(XercesC REQUIRED)

set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost COMPONENTS thread system REQUIRED)
find_package(HDF5 1.8 COMPONENTS C CXX REQUIRED)

#Process the XSD files
SET(XSDS schema/ismrmrd.xsd)
SET(XSD_ARGS cxx-tree --generate-serialization)
#SET(XSD_ARGS cxx-tree --generate-serialization --export-symbol EXPORTISMRMRDXSD --hxx-prologue-file ${CMAKE_SOURCE_DIR}/ismrmrd_xsd_export.h)
WRAP_XSD(XSDS_SOURCES XSD_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/schema ${XSDS} OPTIONS ${XSD_ARGS})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/src/xsd ${XSD_INCLUDE_DIR} ${XERCESC_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${HDF5_CXX_INCLUDE_DIR} ${HDF5_C_INCLUDE_DIR})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

# This dummy is required to trigger autogeneration of ${XSDS_SOURCES}
add_library(dummy SHARED ${XSDS_SOURCES})
target_link_libraries(dummy ${XERCESC_LIBRARIES})

# Add library ismrmrd
add_library(ismrmrd SHARED ismrmrd_hdf5.cpp)

#The findHDF5.cmake does not work very well on Windows, let's help it out
IF(WIN32)
    target_link_libraries(ismrmrd optimized ${HDF5_hdf5_LIBRARY_RELEASE} ${HDF5_hdf5_cpp_LIBRARY_RELEASE})
    target_link_libraries(ismrmrd debug ${HDF5_hdf5_LIBRARY_DEBUG} ${HDF5_hdf5_cpp_LIBRARY_DEBUG})

    target_link_libraries(ismrmrd ${Boost_LIBRARIES})
ELSE (WIN32)
    target_link_libraries(ismrmrd ${HDF5_LIBRARIES} ${Boost_LIBRARIES})
ENDIF(WIN32)

INSTALL(FILES ismrmrd.h ismrmrd_hdf5.h ismrmrd_hdf5_datatypes.h ismrmrd_export.h DESTINATION include)
INSTALL(FILES schema/ismrmrd.xsd ${XSDS_SOURCES} DESTINATION schema)
INSTALL(FILES cmake/FindIsmrmrd.cmake cmake/FindFFTW3.cmake cmake/FindXSD.cmake
        cmake/FindXercesC.cmake cmake/FindNumPy.cmake DESTINATION cmake)
INSTALL(TARGETS ismrmrd DESTINATION lib)

add_subdirectory(examples/c++)
add_subdirectory(utilities)
add_subdirectory(tests)
add_subdirectory(doc)
add_subdirectory(matlab)
add_subdirectory(bindings)
