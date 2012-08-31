cmake_minimum_required(VERSION 2.8)
project(ISMRMRD)

set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/ismrmrd)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(XSD REQUIRED)
find_package(XercesC REQUIRED)
set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost COMPONENTS thread system REQUIRED)
find_package(HDF5 1.8 COMPONENTS C CXX HL REQUIRED)

#Process the XSD files
SET(XSDS schema/ismrmrd.xsd)
SET(XSD_ARGS cxx-tree --generate-serialization --export-symbol EXPORTISMRMRD --hxx-prologue-file ${CMAKE_SOURCE_DIR}/ismrmrd_export.h)
WRAP_XSD(XSDS_SOURCES XSD_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/schema ${XSDS} OPTIONS ${XSD_ARGS})
INCLUDE_DIRECTORIES(${XSD_INCLUDES} ${XERCESC_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${HDF5_INCLUDE_DIR})
add_library(ismrmrd SHARED ismrmrd_hdf5.cpp ${XSDS_SOURCES})
target_link_libraries(ismrmrd ${HDF5_LIBRARIES} ${XERCESC_LIBRARIES} ${Boost_LIBRARIES})

add_executable(ismrmrd_test main.cpp ${XSDS_SOURCES})
target_link_libraries(ismrmrd_test ${XERCESC_LIBRARIES} ismrmrd)

INSTALL(FILES ismrmrd.h ${XSDS_SOURCES} ismrmrd_hdf5.h ismrmrd_hdf5_datatypes.h ismrmrd_export.h DESTINATION include)
INSTALL(FILES schema/ismrmrd.xsd DESTINATION schema)
INSTALL(FILES cmake/FindIsmrmrd.cmake DESTINATION cmake)
INSTALL(TARGETS ismrmrd DESTINATION lib)
INSTALL(TARGETS ismrmrd_test DESTINATION bin)

add_subdirectory(doc)
