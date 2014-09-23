message("Building info application")

find_package(Git)
if (GIT_FOUND)
  execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE ISMRMRD_GIT_SHA1)
  string(STRIP ${ISMRMRD_GIT_SHA1} ISMRMRD_GIT_SHA1)
else()
  set(ISMRMRD_GIT_SHA1 "NA")
endif()

configure_file(ismrmrd_version.in ismrmrd_version.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(ismrmrd_info ismrmrd_info.cpp)
install(TARGETS ismrmrd_info DESTINATION bin)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ismrmrd_version.h DESTINATION include)

