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
SET(XSD_ARGS cxx-tree --generate-serialization --export-symbol EXPORTISMRMRDXSD --hxx-prologue-file ${CMAKE_SOURCE_DIR}/ismrmrd_xsd_export.h)
WRAP_XSD(XSDS_SOURCES XSD_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/schema ${XSDS} OPTIONS ${XSD_ARGS})
INCLUDE_DIRECTORIES(${XSD_INCLUDES} ${XSD_INCLUDE_DIR} ${XERCESC_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${HDF5_INCLUDE_DIR} ${HDF5_INCLUDE_DIR}/cpp)
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

add_library(ismrmrd SHARED ismrmrd.cpp ismrmrd_hdf5.cpp)
add_library(ismrmrd_xsd SHARED ${XSDS_SOURCES})

# Process the MATLAB files
file(GLOB MatlabMFiles  ${CMAKE_SOURCE_DIR}/matlab/+ismrmrd/*.m)
# Convert the schema to Java Classes


#The findHDF5.cmake does not work very well on Windows, let's help it out
IF(WIN32)
	SET(HDF5_LIB_DIR ${HDF5_INCLUDE_DIR}/../lib)
	target_link_libraries(ismrmrd optimized ${HDF5_LIB_DIR}/hdf5dll.lib)
	target_link_libraries(ismrmrd optimized ${HDF5_LIB_DIR}/hdf5_cppdll.lib)
	target_link_libraries(ismrmrd optimized ${HDF5_LIB_DIR}/hdf5_hldll.lib)

	target_link_libraries(ismrmrd debug ${HDF5_LIB_DIR}/hdf5dll.lib)
	target_link_libraries(ismrmrd debug ${HDF5_LIB_DIR}/hdf5_cppdll.lib)
	target_link_libraries(ismrmrd debug ${HDF5_LIB_DIR}/hdf5_hldll.lib)

	target_link_libraries(ismrmrd optimized ${HDF5_LIB_DIR}/zlib.lib)
	target_link_libraries(ismrmrd optimized ${HDF5_LIB_DIR}/szip.lib)
	target_link_libraries(ismrmrd debug ${HDF5_LIB_DIR}/zlib.lib)
	target_link_libraries(ismrmrd debug ${HDF5_LIB_DIR}/szip.lib)

	target_link_libraries(ismrmrd ${Boost_LIBRARIES})
ELSE (WIN32)
	target_link_libraries(ismrmrd ${HDF5_LIBRARIES} ${Boost_LIBRARIES})
ENDIF(WIN32)

target_link_libraries(ismrmrd_xsd ${XERCESC_LIBRARIES})

INSTALL(FILES ismrmrd.h ${XSDS_SOURCES} ismrmrd_hdf5.h ismrmrd_hdf5_datatypes.h ismrmrd_export.h DESTINATION include)
INSTALL(FILES schema/ismrmrd.xsd DESTINATION schema)
INSTALL(FILES cmake/FindIsmrmrd.cmake DESTINATION cmake)
INSTALL(TARGETS ismrmrd DESTINATION lib)
INSTALL(TARGETS ismrmrd_xsd DESTINATION lib)
INSTALL(FILES ${MatlabMFiles} DESTINATION matlab/+ismrmrd)

add_subdirectory(doc)
add_subdirectory(examples/c++)
add_subdirectory(tests/c++)
