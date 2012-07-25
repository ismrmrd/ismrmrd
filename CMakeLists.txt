cmake_minimum_required(VERSION 2.8)
project(ISMRMRD)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(XSD REQUIRED)
find_package(XercesC REQUIRED)

#Process the XSD files
SET(XSDS schema/ismrmrd.xsd)
SET(XSD_ARGS cxx-tree --generate-serialization)

WRAP_XSD(XSDS_SOURCES XSD_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/schema ${XSDS} OPTIONS ${XSD_ARGS})

INCLUDE_DIRECTORIES(${XSD_INCLUDES})
add_executable(ismrmrd main.cpp ${XSDS_SOURCES})
target_link_libraries(ismrmrd ${XERCESC_LIBRARIES})
