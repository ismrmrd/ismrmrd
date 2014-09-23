cmake_minimum_required(VERSION 2.8)
project(ISMRMRD)

# add project specific cmake find modules
list(APPEND CMAKE_MODULE_PATH cmake)

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
#The major number increaments when the binary compatibility of
#the fixed memory layout strucs (e.g. AcquisitionHeader) is broken.
#The minor number changes when there are changes to the XML schema for 
#the flexible header. The micro number changes when there are small changes
#in the utility libraries, that don't affect the data format itself.
set(ISMRMRD_VERSION_MAJOR 0)
set(ISMRMRD_VERSION_MINOR 0)
set(ISMRMRD_VERSION_PATCH 1) 

set(ISMRMRD_XML_SCHEMA_SHA1 "6f1ed6b9209075de481c02e66a37913d9a46b772")

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
#  ---   VERSIONING  (end) ----
 
#  ---   Main Library  (begin) ----
# required packages for main library
find_package(HDF5 1.8 COMPONENTS C REQUIRED)

# include directories for main library
include_directories(
  include
  ${HDF5_C_INCLUDE_DIR}
)

# main library
add_library(ismrmrd SHARED 
  libsrc/ismrmrd.c
  libsrc/ismrmrd.cpp
  libsrc/dataset.c
  libsrc/dataset.cpp
  libsrc/xml.cpp
  libsrc/meta.cpp
  libsrc/pugixml.cpp
)
set_target_properties(ismrmrd PROPERTIES
  VERSION ${ISMRMRD_VERSION_STRING}
  SOVERSION ${ISMRMRD_SOVERSION}
)
target_link_libraries(ismrmrd ${HDF5_LIBRARIES})

# install the main library
install(TARGETS ismrmrd DESTINATION lib)

# install the headers
install(DIRECTORY include/ismrmrd  DESTINATION include)

# install the schema file
install(FILES schema/ismrmrd.xsd
  DESTINATION share/ismrmrd/schema)

# install the cmake modules
install(FILES
  cmake/FindIsmrmrd.cmake
  cmake/FindFFTW3.cmake
  cmake/FindNumPy.cmake
DESTINATION
  share/ismrmrd/cmake
)

#  ---   Main Library  (end) ----

# turn on testing
enable_testing()

# process subdirectories
#add_subdirectory(examples/c++)
#add_subdirectory(examples/c)
#add_subdirectory(utilities)
#add_subdirectory(tests)
#add_subdirectory(doc)
#add_subdirectory(matlab)
#add_subdirectory(bindings)
add_subdirectory(info)