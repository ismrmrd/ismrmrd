cmake_minimum_required(VERSION 3.0)
project(ISMRMRD-C-EXAMPLE)

# if building this example as a standalone project
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    find_package(ISMRMRD REQUIRED)

# otherwise, building it as part of ISMRMRD itself
else()
    set(ISMRMRD_LIBRARIES ismrmrd)
endif()

# In previous ISMRMRD versions, we needed to use ISMRMRD_INCLUDE_DIR,
# but this is no longer necessary as the $(ISMRMRD_LIBRARIES} target
# has this as a property.
add_executable(ismrmrd_c_example main.c)
target_link_libraries(ismrmrd_c_example ${ISMRMRD_LIBRARIES})
install(TARGETS ismrmrd_c_example DESTINATION bin)
