find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

find_package(Java REQUIRED)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${JAVA_INCLUDE_DIRS})
include_directories(${HDF5_INCLUDE_DIR} ${HDF5_INCLUDE_DIR}/cpp ${Boost_INCLUDE_DIR})

set_source_files_properties(ismrmrd_java.i PROPERTIES CPLUSPLUS ON)

SWIG_ADD_MODULE(jismrmrd java ismrmrd_java.i ../../ismrmrd_hdf5.cpp)
SWIG_LINK_LIBRARIES(jismrmrd ${HDF5_LIBRARIES} ${Boost_LIBRARIES} ${JAVA_LIBRARIES})
