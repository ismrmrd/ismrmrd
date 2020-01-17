find_package(Boost 1.43 COMPONENTS unit_test_framework)

if (NOT Boost_UNIT_TEST_FRAMEWORK_FOUND)
    message("Boost Unit Test Framework not found. Not compiling tests")
    return()
endif ()

if (NOT Boost_USE_STATIC_LIBS)
    add_definitions(-DBOOST_TEST_DYN_LINK)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/include ${Boost_INCLUDE_DIR})

add_executable(test_ismrmrd
    test_main.cpp
    test_acquisitions.cpp
    test_images.cpp
    test_ndarray.cpp
    test_flags.cpp
    test_channels.cpp
    test_quaternions.cpp)

target_link_libraries(test_ismrmrd ismrmrd ${Boost_LIBRARIES})

add_custom_target(check COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_ismrmrd DEPENDS test_ismrmrd)
