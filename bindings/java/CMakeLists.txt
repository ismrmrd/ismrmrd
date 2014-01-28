include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${HDF5_INCLUDE_DIR} ${HDF5_INCLUDE_DIR}/cpp ${Boost_INCLUDE_DIR})
include_directories(${JNI_INCLUDE_DIRS})

set(CMAKE_SWIG_FLAGS -package org.ismrm.ismrmrd)
set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/org/ismrm/ismrmrd)

set(SWIG_SOURCES ismrmrd_java.i ${CMAKE_SOURCE_DIR}/ismrmrd_hdf5.cpp)
set_source_files_properties(${SWIG_SOURCES} PROPERTIES CPLUSPLUS ON)
swig_add_module(jismrmrd java ${SWIG_SOURCES})
swig_link_libraries(jismrmrd ${HDF5_LIBRARIES} ${Boost_LIBRARIES})

#### ISMRMRD XML Header Java library ####
configure_file(make_jar.sh.in make_jar.sh @ONLY)

set(JAVADOC_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/doc")
set(JAVA_OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/ismrmrd.jar" ${JAVADOC_OUTPUT})

## TODO: not use a shell script... it breaks 'make'
add_custom_command(
    OUTPUT ${JAVA_OUTPUTS}
    DEPENDS jismrmrd "${CMAKE_CURRENT_BINARY_DIR}/make_jar.sh"
    VERBATIM
    COMMENT "Compiling java files, building jar, generating API docs"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/JNILibLoader.java
               ${CMAKE_CURRENT_BINARY_DIR}/org/ismrm/ismrmrd/
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/org/ismrm/ismrmrd/xmlhdr
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/XMLString.java
               ${CMAKE_CURRENT_BINARY_DIR}/org/ismrm/ismrmrd/xmlhdr/
    COMMAND sh make_jar.sh
    COMMAND javadoc -quiet -d ${JAVADOC_OUTPUT} -subpackages org.ismrm.ismrmrd.xmlhdr
)
add_custom_target(ismrmrdjar ALL DEPENDS ${JAVA_OUTPUTS})

install(TARGETS jismrmrd DESTINATION java)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ismrmrd.jar DESTINATION java)

# Note: the API documentation is only for the XML header, and is installed
# in ${INSTALL_PREFIX}/matlab/doc
install(DIRECTORY ${JAVADOC_OUTPUT} DESTINATION matlab)
