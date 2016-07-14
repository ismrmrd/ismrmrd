cmake_minimum_required(VERSION 2.8)
find_package(Boost 1.43 COMPONENTS system thread program_options)
find_package(FFTW3 COMPONENTS single double threads)

if (NOT Boost_FOUND)
    message("Boost not found. Not building protocol demo")
    return()
endif (NOT Boost_FOUND)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${CMAKE_BINARY_DIR}/include ${CMAKE_SOURCE_DIR}/include)

add_executable(ismrmrd_protocol_demo main.cpp)
target_link_libraries(ismrmrd_protocol_demo ismrmrd ${Boost_LIBRARIES})
install(TARGETS ismrmrd_protocol_demo DESTINATION bin)

add_library(icpSession SHARED icpSession.cpp)
target_link_libraries(icpSession ismrmrd ${Boost_LIBRARIES})
install(TARGETS icpSession DESTINATION lib)

add_library(icpConnection SHARED icpConnection.cpp)
target_link_libraries(icpConnection ${Boost_LIBRARIES} icpSession)
install(TARGETS icpConnection DESTINATION lib)

add_library(icpClientCallbacks icpClientCallbacks.cpp)
target_link_libraries(icpClientCallbacks ismrmrd icpSession icpClient)
install(TARGETS icpClientCallbacks DESTINATION lib)

add_library(icpServerCallbacks icpServerCallbacks.cpp)
target_link_libraries(icpServerCallbacks ismrmrd ${FFTW3_LIBRARIES} icpSession icpServer )
install(TARGETS icpServerCallbacks DESTINATION lib)

add_library(icpClient icpClient.cpp)
target_link_libraries(icpClient ismrmrd icpSession icpClientCallbacks)
install(TARGETS icpClient DESTINATION lib)


add_library(icpServer icpServer.cpp)
target_link_libraries(icpServer ismrmrd icpSession icpServerCallbacks)
install(TARGETS icpServer DESTINATION lib)

add_executable(icpClientStarter icpClientStarter.cpp)
target_link_libraries(icpClientStarter ${Boost_LIBRARIES} icpConnection icpSession icpClient)
install(TARGETS icpClientStarter DESTINATION bin)

add_executable(icpServerStarter icpServerStarter.cpp)
target_link_libraries(icpServerStarter icpConnection icpSession icpServer)
install(TARGETS icpServerStarter DESTINATION bin)

