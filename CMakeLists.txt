cmake_minimum_required(VERSION 2.8)
project(ISMRMRD)

# set project specific cmake module path
set (ISMRMRD_CMAKE_DIR ${PROJECT_SOURCE_DIR}/cmake CACHE PATH
  "Location of CMake scripts")

# command line options
option(USE_SYSTEM_PUGIXML "Use pugixml installed on the system" OFF)

# and include it to the search list
list(APPEND CMAKE_MODULE_PATH ${ISMRMRD_CMAKE_DIR})

# whether to install dependencies
if (WIN32)
    OPTION(ISMRMRD_INSTALL_DEPENDENCIES "Install ismrmrd dependencies in windows" Off)
endif (WIN32)

# set the build type to Release if not specified
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

# compiler flags
if (WIN32)
    add_definitions(-DWIN32 -D_WIN32 -D_WINDOWS)
    add_definitions(-DUNICODE -D_UNICODE)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
    add_definitions(-D__func__=__FUNCTION__)
else (WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif (WIN32)

#  ---   VERSIONING  (begin) ----
#The ISMRMRD convention is to use version numbers with the format:
#   XX.YY.ZZ (major, minor, patch)
#
#The major number increments when the binary compatibility of
#the fixed memory layout struts (e.g. AcquisitionHeader) is broken.
#The minor number changes when there are changes to the XML schema for 
#the flexible header. The micro number changes when there are small changes
#in the utility libraries, that don't affect the data format itself.
set(ISMRMRD_VERSION_MAJOR 1)
set(ISMRMRD_VERSION_MINOR 2)
set(ISMRMRD_VERSION_PATCH 1) 

set(ISMRMRD_XML_SCHEMA_SHA1 "99a63f4e8cf08ffc268f5ee8b8c8b2a1edf69412")

#Remove line breaks and white space that does not change the meaning of the schema
file(STRINGS ${CMAKE_SOURCE_DIR}/schema/ismrmrd.xsd SCHEMA_STRINGS) #Read all strings from file 
string(REPLACE ";" "" SCHEMA_NO_BREAKS  ${SCHEMA_STRINGS}) #Concatenate the list of strings
string(REGEX REPLACE ">[ \t]+<" "><" SCHEMA_NO_SPACE ${SCHEMA_NO_BREAKS}) #Remove spaces and tabs
string(STRIP ${SCHEMA_NO_SPACE} SCHEMA_NO_SPACE) #Strip any leading/trailing whitespace
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/ismrmrd_no_white_space.xsd ${SCHEMA_NO_SPACE}) #Write to file

#Now hash the cleaned up file
file(SHA1 ${CMAKE_CURRENT_BINARY_DIR}/ismrmrd_no_white_space.xsd ISMRMRD_CURRENT_XML_SCHEMA_SHA1)

#Compare to last known hash
if (NOT (${ISMRMRD_XML_SCHEMA_SHA1} STREQUAL ${ISMRMRD_CURRENT_XML_SCHEMA_SHA1}))
  message("")
  message("-----------------------------------------------")
  message("            !!VERSION ERROR!!                  ")
  message("                                               ")
  message(" Expected SHA1 hash:                           ")
  message("    ${ISMRMRD_XML_SCHEMA_SHA1}")
  message(" Actual SHA1 hash:                             ")
  message("    ${ISMRMRD_CURRENT_XML_SCHEMA_SHA1}")
  message("                                               ")
  message(" The XML Schema (ismrmrmd.xsd) has changed and ")
  message(" the MINOR version number should be increased  ")
  message(" and the SHA1 has should be updated in the     ")
  message(" CMakelists.txt file.                          ")
  message("                                               ")
  message(" If you don't know what this message means, you")
  message(" probably shouldn't be changing anything       ")
  message("-----------------------------------------------")
  message("")
  message(FATAL_ERROR "     FATAL XML VERSION ERROR")
endif()

set(ISMRMRD_VERSION_STRING ${ISMRMRD_VERSION_MAJOR}.${ISMRMRD_VERSION_MINOR}.${ISMRMRD_VERSION_PATCH})
set(ISMRMRD_SOVERSION ${ISMRMRD_VERSION_MAJOR}.${ISMRMRD_VERSION_MINOR})

# Generate the version.h header file
find_package(Git)
if (GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD WORKING_DIRECTORY
            ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE ISMRMRD_GIT_SHA1 ERROR_VARIABLE ISMRMRD_GIT_STDERR)
    string(STRIP "${ISMRMRD_GIT_SHA1}" ISMRMRD_GIT_SHA1)
    string(LENGTH "${ISMRMRD_GIT_SHA1}" ISMRMRD_GIT_SHA1_LEN)
    if (${ISMRMRD_GIT_SHA1_LEN} LESS 40)
        message(WARNING "Could not determine SHA-1 hash: ${ISMRMRD_GIT_STDERR}")
        set(ISMRMRD_GIT_SHA1 "NA")
    endif (${ISMRMRD_GIT_SHA1_LEN} LESS 40)
else()
  set(ISMRMRD_GIT_SHA1 "NA")
endif()
configure_file(include/version.in ${CMAKE_BINARY_DIR}/include/ismrmrd/version.h)
install(FILES ${CMAKE_BINARY_DIR}/include/ismrmrd/version.h DESTINATION include/ismrmrd COMPONENT dev)
# note: for the utilities in this project that need ismrmrd/version.h
# remember to add ${CMAKE_BINARY_DIR}/include to the include path

#  ---   VERSIONING  (end) ----
 
#  ---   Main Library  (begin) ----
# required packages for main library
find_package(HDF5 1.8 COMPONENTS C REQUIRED)

# in windows, install the HDF5 dependencies
if (WIN32)
    if (ISMRMRD_INSTALL_DEPENDENCIES)
        if ( HDF5_FOUND )
            if(DEFINED ENV{HDF5_ROOT})
                set(HDF5_BIN_DIR $ENV{HDF5_ROOT}/bin)
            else (DEFINED ENV{HDF5_ROOT})
                set(HDF5_BIN_DIR ${HDF5_C_INCLUDE_DIR}/../bin)
            endif (DEFINED ENV{HDF5_ROOT})
            message("Install hdf5 libraries from ${HDF5_BIN_DIR} ")
            install( DIRECTORY ${HDF5_BIN_DIR} DESTINATION bin/.. FILES_MATCHING PATTERN "*.dll" )
        endif (HDF5_FOUND)
    endif (ISMRMRD_INSTALL_DEPENDENCIES)
endif (WIN32)

# include directories for main library
set(ISMRMRD_TARGET_INCLUDE_DIRS
  include
  ${CMAKE_BINARY_DIR}/include
  ${HDF5_C_INCLUDE_DIR}
)

set(ISMRMRD_TARGET_SOURCES
  libsrc/ismrmrd.c
  libsrc/ismrmrd.cpp
  libsrc/dataset.c
  libsrc/dataset.cpp
  libsrc/xml.cpp
  libsrc/meta.cpp
)

set(ISMRMRD_TARGET_LINK_LIBS ${HDF5_LIBRARIES})

# optional handling of system-installed pugixml
if(USE_SYSTEM_PUGIXML)
  find_package(PugiXML)
  if(PugiXML_FOUND)
    message("Found system pugixml: ${PugiXML_INCLUDE_DIR} ${PugiXML_LIBRARY}")
    list(APPEND ISMRMRD_TARGET_INCLUDE_DIRS ${PugiXML_INCLUDE_DIR})
    list(APPEND ISMRMRD_TARGET_LINK_LIBS ${PugiXML_LIBRARY})
  else(PugiXML_FOUND)
    message(FATAL_ERROR "Pugixml library not found on the system, try without "
    "setting USE_SYSTEM_PUGIXML to use the version provided in the source "
    "tree.")
  endif(PugiXML_FOUND)
  list(APPEND ISMRMRD_TARGET_INCLUDE_DIRS ${PugiXML_INCLUDE_DIR})
  list(APPEND ISMRMRD_TARGET_LINK_LIBS ${PugiXML_LIBRARY})
else(USE_SYSTEM_PUGIXML)
  list(APPEND ISMRMRD_TARGET_SOURCES libsrc/pugixml.cpp)
endif(USE_SYSTEM_PUGIXML)

# main library
include_directories(${ISMRMRD_TARGET_INCLUDE_DIRS})
add_library(ismrmrd SHARED ${ISMRMRD_TARGET_SOURCES})
set_target_properties(ismrmrd PROPERTIES
  VERSION ${ISMRMRD_VERSION_STRING}
  SOVERSION ${ISMRMRD_SOVERSION}
)
target_link_libraries(ismrmrd ${ISMRMRD_TARGET_LINK_LIBS})

# install the main library
install(TARGETS ismrmrd DESTINATION lib COMPONENT lib)

# install the headers
install(DIRECTORY include/ismrmrd  DESTINATION include COMPONENT dev)

# install the schema file
install(FILES schema/ismrmrd.xsd DESTINATION share/ismrmrd/schema COMPONENT lib)

# install the cmake modules
install(FILES cmake/FindIsmrmrd.cmake cmake/FindFFTW3.cmake cmake/FindNumPy.cmake DESTINATION share/ismrmrd/cmake COMPONENT dev)

#  ---   Main Library  (end) ----

# turn on testing
enable_testing()

# process subdirectories
add_subdirectory(doc)
add_subdirectory(examples/c)
add_subdirectory(utilities)

# install the matlab api
install(DIRECTORY matlab DESTINATION share/ismrmrd COMPONENT dev)

# TODO build and install the python bindings

# Create package

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)

find_program(DPKG_PROGRAM dpkg)
if(EXISTS ${DPKG_PROGRAM})
  list(APPEND CPACK_GENERATOR "DEB")
endif(EXISTS ${DPKG_PROGRAM})

# Set dependencies explicitly
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libhdf5-7, libfftw3-3, libboost-program-options-dev")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

# Enable/Disable component install for CPack generator DEB
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_DEB_PACKAGE_COMPONENT ON)

# Set component dependencies explicitly
set(CPACK_DEB_lib_PACKAGE_DEPENDS "libhdf5-7, libfftw3-3, libboost-program-options-dev")
set(CPACK_DEB_dev_PACKAGE_DEPENDS "ismrmrd-lib")
set(CPACK_DEB_utils_PACKAGE_DEPENDS "ismrmrd-lib")
set(CPACK_DEB_whole_PACKAGE_DEPENDS "ismrmrd-dev, ismrmrd-utils")

# Where the package metadata are
set(ISMRMRD_CPACK_CFG_FILE "${PROJECT_BINARY_DIR}/cpack_options.cmake")

# Where the package to be installed 
#set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CMAKE_INSTALL_PREFIX})
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

#include(${ISMRMRD_CMAKE_DIR}/ismrmrd_cpack.cmake)
if(CPACK_GENERATOR)
  message(STATUS "Found CPack generators: ${CPACK_GENERATOR}")
  configure_file("${ISMRMRD_CMAKE_DIR}/cpack_options.cmake.in" ${ISMRMRD_CPACK_CFG_FILE} @ONLY)
  set(CPACK_PROJECT_CONFIG_FILE ${ISMRMRD_CPACK_CFG_FILE})
  
  # A list containing the names of all components that should be installed
  set(CPACK_COMPONENTS_ALL lib dev utils whole)
  include(CPack)
endif()
