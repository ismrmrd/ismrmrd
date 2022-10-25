cmake_minimum_required(VERSION 3.0)
foreach(p
    CMP0025 # CMake 3.0 Compiler id for Apple Clang is now ``AppleClang``.
    CMP0042 # CMake 3.0 ``MACOSX_RPATH`` is enabled by default.
    CMP0046 # CMake 3.0 Error on non-existent dependency in add_dependencies.
    CMP0054 # CMake 3.1 Only interpret ``if()`` arguments as variables or keywords when unquoted.
    CMP0056 # CMake 3.2 Honor link flags in ``try_compile()`` source-file signature.
    CMP0058 # CMake 3.3 Ninja requires custom command byproducts to be explicit.
    )
  if(POLICY ${p})
    cmake_policy(SET ${p} NEW)
  endif()
endforeach()
project(ISMRMRD)

# set project specific cmake module path
set (ISMRMRD_CMAKE_DIR ${PROJECT_SOURCE_DIR}/cmake CACHE PATH
  "Location of CMake scripts")

# command line options
option(USE_SYSTEM_PUGIXML "Use pugixml installed on the system" OFF)
option(USE_HDF5_DATASET_SUPPORT "Compile with support for reading and writing datasets to HDF5 files" ON)
option(BUILD_TESTS "Build unit tests " ON)
option(BUILD_UTILITIES "Build utilities tests " ON)
option(BUILD_EXAMPLES "Build examples tests " ON)
# If defined, this will modify the build environment to use General Electric's Libraries,
# which are needed to link against and use their SDK to open and process their raw data.
option(build4GE FALSE OFF)

# and include it to the search list
list(APPEND CMAKE_MODULE_PATH ${ISMRMRD_CMAKE_DIR})


# set the build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif()

# compiler flags
if (WIN32)
    add_definitions(-DWIN32 -D_WIN32 -D_WINDOWS -DBOOST_UUID_FORCE_AUTO_LINK)
    add_definitions(-DUNICODE -D_UNICODE)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /WX")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO")
    add_definitions(-D__func__=__FUNCTION__)
elseif (build4GE)
    message (STATUS, " Setting up ISMRMRD build to use GE SDK HDF5 and Boost components for GE converter.")
    if(DEFINED ENV{SDKTOP})
       cmake_policy (SET CMP0074 OLD)
       set(HDF5_ROOT                   "$ENV{SDKTOP}/3p")
       set(BOOST_ROOT                  "$ENV{SDKTOP}/3p")
    else ()
       message (FATAL_ERROR, " SDKTOP not defined - cannot configure ISMRMRD build for GE properly.")
    endif ()
    set(Boost_USE_STATIC_LIBS       "ON")
    set(Boost_NO_SYSTEM_PATHS       "TRUE")
    set(HDF5_USE_STATIC_LIBRARIES   "yes")
    set(CMAKE_C_FLAGS               "${CMAKE_C_FLAGS} -std=c99 -Wall")
    set(CMAKE_CXX_FLAGS             "${CMAKE_CXX_FLAGS} -w -std=c++11 -D_GLIBCXX_USE_CXX11_ABI=0")
else ()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Werror")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -Werror")
endif ()

#  ---   VERSIONING  (begin) ----
#The ISMRMRD convention is to use version numbers with the format:
#   XX.YY.ZZ (major, minor, patch)
#
#The major number increments when the binary compatibility of
#the fixed memory layout struts (e.g. AcquisitionHeader) is broken.
#The minor number changes when there are changes to the XML schema for
#the flexible header. The micro number changes when there are small changes
#in the utility libraries, that don't affect the data format itself.
# For more information see http://semver.org/
set(ISMRMRD_VERSION_MAJOR 1)
set(ISMRMRD_VERSION_MINOR 11)
set(ISMRMRD_VERSION_PATCH 2)
set(ISMRMRD_VERSION_STRING ${ISMRMRD_VERSION_MAJOR}.${ISMRMRD_VERSION_MINOR}.${ISMRMRD_VERSION_PATCH})
set(ISMRMRD_SOVERSION ${ISMRMRD_VERSION_MAJOR}.${ISMRMRD_VERSION_MINOR})


set(ISMRMRD_XML_SCHEMA_SHA1  "725698104a8e8abbd34a419b9ac29184240a470b")

#Remove line breaks and white space that does not change the meaning of the schema
file(STRINGS ${CMAKE_SOURCE_DIR}/schema/ismrmrd.xsd SCHEMA_STRINGS) #Read all strings from file
string(REPLACE ";" "" SCHEMA_NO_BREAKS  ${SCHEMA_STRINGS}) #Concatenate the list of strings
string(REGEX REPLACE ">[ \t]+<" "><" SCHEMA_NO_SPACE ${SCHEMA_NO_BREAKS}) #Remove spaces and tabs
string(STRIP ${SCHEMA_NO_SPACE} SCHEMA_NO_SPACE) #Strip any leading/trailing whitespace
set(ISMRMRD_SCHEMA_DIR ${CMAKE_CURRENT_BINARY_DIR})
file(WRITE ${ISMRMRD_SCHEMA_DIR}/ismrmrd_no_white_space.xsd ${SCHEMA_NO_SPACE}) #Write to file

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


# Find HDF5 for dataset support


if (USE_HDF5_DATASET_SUPPORT)
    if (VCPKG_TARGET_TRIPLET) #VCPKG HDF5 is packaged differently.
        find_package(HDF5 CONFIG COMPONENTS C shared REQUIRED)
        set(HDF5_C_LIBRARIES hdf5::hdf5-shared)
    else ()
        find_package(HDF5 COMPONENTS C REQUIRED)
    endif ()
    set(ISMRMRD_DATASET_SUPPORT true)
    set(ISMRMRD_DATASET_SOURCES libsrc/dataset.c libsrc/dataset.cpp)
    set(ISMRMRD_DATASET_INCLUDE_DIR ${HDF5_INCLUDE_DIRS})
    set(ISMRMRD_DATASET_LIBRARIES ${HDF5_C_LIBRARIES})
    message(STATUS "HDF5 include found at: ${HDF5_INCLUDE_DIRS}")
    message(STATUS "HDF5 libs found at: ${HDF5_C_LIBRARIES}")

else ()
    set(ISMRMRD_DATASET_SUPPORT false)
    message(WARNING " Dataset and file support unavailable!")
endif ()

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
    endif ()
else()
  set(ISMRMRD_GIT_SHA1 "NA")
endif()
configure_file(include/version.in ${CMAKE_BINARY_DIR}/include/ismrmrd/version.h)
install(FILES ${CMAKE_BINARY_DIR}/include/ismrmrd/version.h DESTINATION include/ismrmrd COMPONENT Devel)
# note: for the utilities in this project that need ismrmrd/version.h
# remember to add ${CMAKE_BINARY_DIR}/include to the include path


#  ---   VERSIONING  (end) ----

#  ---   Main Library  (begin) ----
# in windows, install the HDF5 dependencies

# include directories for main library
set(ISMRMRD_TARGET_INCLUDE_DIRS
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${ISMRMRD_DATASET_INCLUDE_DIR}
)

set(ISMRMRD_TARGET_SOURCES
  libsrc/ismrmrd.c
  libsrc/ismrmrd.cpp
  libsrc/xml.cpp
  libsrc/meta.cpp
  libsrc/waveform.cpp
  libsrc/waveform.c
  ${ISMRMRD_DATASET_SOURCES}
)

set(ISMRMRD_TARGET_LINK_LIBS ${ISMRMRD_DATASET_LIBRARIES})

# optional handling of system-installed pugixml
if (USE_SYSTEM_PUGIXML)

    if (VCPKG_TARGET_TRIPLET) #VCPKG .
        find_package(PugiXML CONFIG REQUIRED)
        list(APPEND ISMRMRD_TARGET_LINK_LIBS pugixml)
    else ()
        find_package(PugiXML REQUIRED)

        message("Found system pugixml: ${PugiXML_INCLUDE_DIR} ${PugiXML_LIBRARY}")
        list(APPEND ISMRMRD_TARGET_INCLUDE_DIRS ${PugiXML_INCLUDE_DIR})
        list(APPEND ISMRMRD_TARGET_LINK_LIBS ${PugiXML_LIBRARY})
        list(APPEND ISMRMRD_TARGET_INCLUDE_DIRS ${PugiXML_INCLUDE_DIR})
        list(APPEND ISMRMRD_TARGET_LINK_LIBS ${PugiXML_LIBRARY})
    endif ()
else ()
    list(APPEND ISMRMRD_TARGET_SOURCES libsrc/pugixml.cpp)
endif ()

if (build4GE)
   list(APPEND ISMRMRD_TARGET_LINK_LIBS pthread z dl)
endif ()

# main library
add_library(ismrmrd SHARED ${ISMRMRD_TARGET_SOURCES})
target_include_directories(ismrmrd PUBLIC ${ISMRMRD_TARGET_INCLUDE_DIRS})
if (USE_HDF5_DATASET_SUPPORT)
  target_compile_options(ismrmrd PUBLIC ${HDF5_DEFINITIONS})
endif()
add_library(ismrmrd::ismrmrd ALIAS ismrmrd)
set_target_properties(ismrmrd PROPERTIES
  VERSION ${ISMRMRD_VERSION_STRING}
  SOVERSION ${ISMRMRD_SOVERSION}
)

set_target_properties(ismrmrd
  PROPERTIES
  EXPORT_NAME ISMRMRD)

target_link_libraries(ismrmrd ${ISMRMRD_TARGET_LINK_LIBS})
list(APPEND ISMRMRD_LIBRARIES ismrmrd) # Add to list of libraries to be found
list(APPEND ISMRMRD_LIBRARY_DIRS ${CMAKE_BINARY_DIR} ) # Add to list of directories to find libraries

include(GNUInstallDirs)

# install the main library
install(TARGETS ismrmrd EXPORT ISMRMRDTargets
   LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
   ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
   RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} 
   COMPONENT Devel
)

# install the headers
install(DIRECTORY include/ismrmrd  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT Devel)

# install the schema file
install(FILES schema/ismrmrd.xsd DESTINATION share/ismrmrd/schema COMPONENT Devel)

#  ---   Main Library  (end) ----

# process subdirectories
add_subdirectory(doc)

if (BUILD_UTILITIES)
  add_subdirectory(utilities)
endif()


if (HDF5_FOUND AND BUILD_EXAMPLES)
    add_subdirectory(examples/c)
endif ()

# TODO: make this work on Windows
if (BUILD_TESTS)
   if (NOT build4GE)

       enable_testing()
      add_subdirectory(tests)
   endif ()
endif ()

# install the matlab api
install(DIRECTORY matlab DESTINATION share/ismrmrd )

#--- Create cmake package for downstream projects
#

include(CMakePackageConfigHelpers)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/ISMRMRD)

set(CONFIG_ISMRMRD_SCHEMA_DIR   ${ISMRMRD_SCHEMA_DIR})
set(CONFIG_ISMRMRD_TARGET_INCLUDE_DIRS ${ISMRMRD_TARGET_INCLUDE_DIRS})
set(CONFIG_ISMRMRD_LIBRARY_DIRS ${ISMRMRD_LIBRARY_DIRS})
configure_file(cmake/ISMRMRDConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/ISMRMRDConfig.cmake"
  @ONLY
)

set(CONFIG_ISMRMRD_SCHEMA_DIR   ${CMAKE_INSTALL_PREFIX}/share/ismrmrd/schema)
set(CONFIG_ISMRMRD_TARGET_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include)
set(CONFIG_ISMRMRD_LIBRARY_DIRS ${CMAKE_INSTALL_PREFIX}/lib)
if (ISMRMRD_DATASET_SUPPORT)
  list(APPEND CONFIG_ISMRMRD_TARGET_INCLUDE_DIRS ${HDF5_INCLUDE_DIRS})
endif ()

configure_package_config_file(cmake/ISMRMRDConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/InstallFiles/ISMRMRDConfig.cmake"
  INSTALL_DESTINATION ${INSTALL_CONFIGDIR})

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/ISMRMRDConfigVersion.cmake"
  VERSION ${ISMRMRD_VERSION_STRING}
  COMPATIBILITY SameMajorVersion
  )

# Write a CMake file with all the targets information
# (not for installing, but for external project to import targets from the
#  current build tree)
if(CMAKE_VERSION VERSION_LESS 3.0)
  export(TARGETS ismrmrd
    FILE ${CMAKE_CURRENT_BINARY_DIR}/ISMRMRDTargets.cmake
    NAMESPACE ISMRMRD::
    )
else()
  export(EXPORT ISMRMRDTargets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/ISMRMRDTargets.cmake
    NAMESPACE ISMRMRD::
    )
endif()

install(EXPORT ISMRMRDTargets
  FILE ISMRMRDTargets.cmake
  NAMESPACE ISMRMRD::
  DESTINATION ${INSTALL_CONFIGDIR}
  )

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/ISMRMRDConfigVersion.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/InstallFiles/ISMRMRDConfig.cmake"
  DESTINATION ${INSTALL_CONFIGDIR}
  COMPONENT Devel)

export(PACKAGE ISMRMRD)

# Create package
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
include(${ISMRMRD_CMAKE_DIR}/ismrmrd_cpack.cmake)
if(CPACK_GENERATOR)
  message(STATUS "Found CPack generators: ${CPACK_GENERATOR}")
  configure_file("${ISMRMRD_CMAKE_DIR}/cpack_options.cmake.in" ${ISMRMRD_CPACK_CFG_FILE} @ONLY)
  set(CPACK_PROJECT_CONFIG_FILE ${ISMRMRD_CPACK_CFG_FILE})
  include(CPack)
endif()
