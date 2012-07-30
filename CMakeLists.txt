cmake_minimum_required(VERSION 2.8)
project(ISMRMRD)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(XSD REQUIRED)
find_package(XercesC REQUIRED)
find_package(Boost REQUIRED)
find_package(HDF5 1.8 COMPONENTS C CXX HL REQUIRED)

#Process the XSD files
SET(XSDS schema/ismrmrd.xsd)
SET(XSD_ARGS cxx-tree --generate-serialization)

WRAP_XSD(XSDS_SOURCES XSD_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/schema ${XSDS} OPTIONS ${XSD_ARGS})

INCLUDE_DIRECTORIES(${XSD_INCLUDES} ${XERCESC_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${HDF5_INCLUDES})
add_executable(ismrmrd_test main.cpp ${XSDS_SOURCES})
target_link_libraries(ismrmrd_test ${XERCESC_LIBRARIES})

add_library(ismrmrd SHARED ismrmrd_hdf5.cpp)
target_link_libraries(ismrmrd ${HDF5_LIBRARIES})

INSTALL(FILES ismrmrd.h ismrmrd_hdf5.h ismrmrd_hdf5_datatypes.h ismrmrd_export.h DESTINATION include)
INSTALL(FILES schema/ismrmrd.xsd DESTINATION schema)
INSTALL(FILES cmake/FindIsmrmrd.cmake DESTINATION cmake)
INSTALL(TARGETS ismrmrd DESTINATION lib)
INSTALL(TARGETS ismrmrd_test DESTINATION bin)