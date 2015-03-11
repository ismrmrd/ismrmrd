find_package(Boost 1.43 COMPONENTS unit_test_framework)

if (NOT Boost_FOUND)
    message("Boost Unit Test Framework not found. Not compiling tests")
    return()
endif (NOT Boost_FOUND)

include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/include)

add_executable(test_ismrmrd test_main.cpp test_acquisitions.cpp test_quaternions.cpp)
target_link_libraries(test_ismrmrd ismrmrd ${Boost_LIBRARIES})
