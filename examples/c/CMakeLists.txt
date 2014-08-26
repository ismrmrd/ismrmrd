cmake_minimum_required(VERSION 2.8)
project(ISMRMRD_C_EXAMPLE)

if (NOT DEFINED ENV{ISMRMRD_HOME})
    message(FATAL_ERROR "Environment variable $ISMRMRD_HOME not set")
endif()
# for finding Ismrmrd
list (APPEND CMAKE_MODULE_PATH "$ENV{ISMRMRD_HOME}/cmake")

find_package(Ismrmrd REQUIRED)

include_directories (
    ${ISMRMRD_INCLUDE_DIR}
)

add_executable(ismrmrd_c_demo main.c)
target_link_libraries(ismrmrd_c_demo ${ISMRMRD_LIBRARIES})
