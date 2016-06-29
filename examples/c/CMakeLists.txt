cmake_minimum_required(VERSION 2.8)
project(ISMRMRD-C-EXAMPLE)

# if building this example as a standalone project
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    if(NOT DEFINED ENV{ISMRMRD_HOME})
        message(FATAL_ERROR "ISMRMRD_HOME environment variable must be defined")
    endif()

    list(APPEND CMAKE_MODULE_PATH "$ENV{ISMRMRD_HOME}/share/ismrmrd/cmake")

    find_package(Ismrmrd REQUIRED)

# otherwise, building it as part of ISMRMRD itself
else()
    set(ISMRMRD_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/include")
    set(ISMRMRD_LIBRARIES ismrmrd)
endif()

include_directories(${ISMRMRD_INCLUDE_DIR})
add_executable(ismrmrd_c_example main.c)
target_link_libraries(ismrmrd_c_example ${ISMRMRD_LIBRARIES})
install(TARGETS ismrmrd_c_example DESTINATION bin)
