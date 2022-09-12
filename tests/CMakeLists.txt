find_package(Boost 1.43 COMPONENTS unit_test_framework)

if (NOT Boost_UNIT_TEST_FRAMEWORK_FOUND)
    message("Boost Unit Test Framework not found. Not compiling tests")
    return()
endif ()

if (NOT Boost_USE_STATIC_LIBS)
    add_definitions(-DBOOST_TEST_DYN_LINK)
endif()


file(READ ${CMAKE_SOURCE_DIR}/schema/ismrmrd_example.xml ismrmrd_example)
string(REPLACE "\"" "\\\"" ismrmrd_example ${ismrmrd_example})
string(REPLACE "\n" "\\\n" ismrmrd_example ${ismrmrd_example})

file(READ ${CMAKE_SOURCE_DIR}/schema/ismrmrd_example_extended.xml ismrmrd_example_extended)
string(REPLACE "\"" "\\\"" ismrmrd_example_extended ${ismrmrd_example_extended})
string(REPLACE "\n" "\\\n" ismrmrd_example_extended ${ismrmrd_example_extended})

configure_file(embedded_xml.h.in embedded_xml.h)

include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/include ${Boost_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_executable(test_ismrmrd
    test_main.cpp
    test_acquisitions.cpp
    test_images.cpp
    test_ndarray.cpp
    test_flags.cpp
    test_channels.cpp
    test_xml_header.cpp
    test_quaternions.cpp)

target_link_libraries(test_ismrmrd ismrmrd ${Boost_LIBRARIES})

add_test(NAME check COMMAND test_ismrmrd )