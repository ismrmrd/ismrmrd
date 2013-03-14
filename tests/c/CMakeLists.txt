enable_testing()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../..)

add_executable(t_cstructs t_cstructs.c)
add_test(t_cstructs ${CMAKE_CURRENT_BINARY_DIR}/t_cstructs)

add_executable(t_cquaternions ../../ismrmrd.h t_cquaternions.c)
add_test(t_cquaternions ${CMAKE_CURRENT_BINARY_DIR}/t_cquaternions)
