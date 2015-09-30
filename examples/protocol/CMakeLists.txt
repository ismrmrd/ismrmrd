find_package(Boost 1.43 COMPONENTS system thread program_options)

if (NOT Boost_FOUND)
    message("Boost not found. Not building protocol demo")
    return()
endif (NOT Boost_FOUND)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${CMAKE_BINARY_DIR}/include ${CMAKE_SOURCE_DIR}/include)

add_executable(ismrmrd_protocol_demo main.cpp)

target_link_libraries(ismrmrd_protocol_demo ismrmrd ${Boost_LIBRARIES})

install(TARGETS ismrmrd_protocol_demo DESTINATION bin)
