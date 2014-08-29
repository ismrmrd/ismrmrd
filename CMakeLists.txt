cmake_minimum_required(VERSION 2.8)
project(ISMRMRD)

# install into a named subdirectory
set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/ismrmrd)

# add project specific cmake find modules
list(APPEND CMAKE_MODULE_PATH cmake)

# set the build type to Release if not specified
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

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
else (WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif (WIN32)

# required packages for main library
find_package(HDF5 1.8 COMPONENTS C REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/xml 
    ${HDF5_C_INCLUDE_DIR})

add_library(ismrmrd SHARED 
		    ismrmrd.c 
		    ismrmrd.cpp 
		    ismrmrd_dataset.c
		    ismrmrd_dataset.cpp
  		    xml/ismrmrd_xml.cpp
  		    xml/ismrmrd_meta.cpp
  		    xml/pugixml.cpp)

# findHDF5.cmake does not work very well on Windows, let's help it out
if(WIN32)
    target_link_libraries(ismrmrd optimized ${HDF5_hdf5_LIBRARY_RELEASE})
    target_link_libraries(ismrmrd debug ${HDF5_hdf5_LIBRARY_DEBUG})
else(WIN32)
    target_link_libraries(ismrmrd ${HDF5_LIBRARIES})
endif(WIN32)

install(TARGETS ismrmrd DESTINATION lib)

install(FILES 
    ismrmrd.h
    ismrmrd_dataset.h
    ismrmrd_export.h
    xml/ismrmrd_xml.h
    xml/ismrmrd_meta.h
    DESTINATION include)

install(FILES
    schema/ismrmrd.xsd
    DESTINATION schema)

install(FILES
    cmake/FindIsmrmrd.cmake
    cmake/FindFFTW3.cmake
    cmake/FindNumPy.cmake
    DESTINATION cmake)

# copy the examples directory
install(DIRECTORY examples DESTINATION .)

add_subdirectory(doc)
#add_subdirectory(utilities)
#add_subdirectory(tests)
#add_subdirectory(matlab)
#add_subdirectory(bindings)
