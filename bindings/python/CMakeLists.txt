find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_PATH})

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

#set(CMAKE_SWIG_FLAGS "-c++")
set(CMAKE_SWIG_FLAGS "")
set_source_files_properties(ismrmrd.i PROPERTIES CPLUSPLUS ON)
#set_source_files_properties(ismrmrd.i PROPERTIES SWIG_FLAGS "-includeall")
SWIG_ADD_MODULE(ismrmrd python ismrmrd.i)
SWIG_LINK_LIBRARIES(ismrmrd ${PYTHON_LIBRARIES})
