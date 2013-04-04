find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

find_package(PythonLibs REQUIRED)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${PYTHON_INCLUDE_PATH} ${PYTHON_NUMPY_INCLUDE_DIR})
include_directories(${HDF5_INCLUDE_DIR} ${HDF5_INCLUDE_DIR}/cpp ${Boost_INCLUDE_DIR})

set_source_files_properties(ismrmrd_python.i PROPERTIES CPLUSPLUS ON)

SWIG_ADD_MODULE(ismrmrd python ismrmrd_python.i ../../ismrmrd_hdf5.cpp)
SWIG_LINK_LIBRARIES(ismrmrd ${HDF5_LIBRARIES} ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
