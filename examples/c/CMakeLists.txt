add_executable(ismrmrd_c_demo main.c)
target_link_libraries(ismrmrd_c_demo ismrmrd)

install(TARGETS ismrmrd_c_demo DESTINATION bin)
