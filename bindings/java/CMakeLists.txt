include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${HDF5_INCLUDE_DIR} ${HDF5_INCLUDE_DIR}/cpp ${Boost_INCLUDE_DIR})
include_directories(${JAVA_INCLUDE_PATH})
if (UNIX)
    include_directories(${JAVA_INCLUDE_PATH}/linux)
endif (UNIX)

set(CMAKE_SWIG_FLAGS -package org.ismrm.ismrmrd)
set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/org/ismrm/ismrmrd)

set(SWIG_SOURCES ismrmrd_java.i ${CMAKE_SOURCE_DIR}/ismrmrd_hdf5.cpp)
set_source_files_properties(${SWIG_SOURCES} PROPERTIES CPLUSPLUS ON)
swig_add_module(jismrmrd java ${SWIG_SOURCES})
swig_link_libraries(jismrmrd ${HDF5_LIBRARIES} ${Boost_LIBRARIES})

## TODO: not use a unix shell script
configure_file(make_jar.sh.in make_jar.sh @ONLY)
set_source_files_properties(ismrmrd.jar PROPERTIES GENERATED true)
add_custom_command(
    TARGET jismrmrd
    POST_BUILD
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Compiling java files and creating jar" VERBATIM
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/JNILibLoader.java
               ${CMAKE_CURRENT_BINARY_DIR}/org/ismrm/ismrmrd/
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/org/ismrm/ismrmrd/xmlhdr
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/XMLString.java
               ${CMAKE_CURRENT_BINARY_DIR}/org/ismrm/ismrmrd/xmlhdr/
    COMMAND sh make_jar.sh
    VERBATIM
)

install(TARGETS jismrmrd DESTINATION java)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ismrmrd.jar DESTINATION java)
