# Build the info application
message("Building info application")
include_directories(${CMAKE_BINARY_DIR}/include
                    ${CMAKE_SOURCE_DIR}/include
		    ${CMAKE_SOURCE_DIR}/libsrc)

add_executable(ismrmrd_info ismrmrd_info.cpp)
target_link_libraries(ismrmrd_info ismrmrd)
install(TARGETS ismrmrd_info DESTINATION bin)

add_executable(ismrmrd_read_timing_test read_timing_test.cpp)
target_link_libraries(ismrmrd_read_timing_test ismrmrd)
install(TARGETS ismrmrd_read_timing_test DESTINATION bin)

if (NOT WIN32)
  add_executable(ismrmrd_test_xml 
    ismrmrd_test_xml.cpp 
    ${CMAKE_SOURCE_DIR}/libsrc/pugixml.cpp )
  target_link_libraries(ismrmrd_test_xml ismrmrd)
  install(TARGETS ismrmrd_test_xml DESTINATION bin)
endif(NOT WIN32)

find_package(Boost 1.43 COMPONENTS program_options)
find_package(FFTW3 COMPONENTS single)

if(FFTW3_FOUND AND Boost_FOUND)
    message("FFTW3 and Boost Found, building utilities")
    
    if(WIN32)
        link_directories(${Boost_LIBRARY_DIRS})
    endif(WIN32)

    include_directories(
        ${CMAKE_SOURCE_DIR/include}
        ${Boost_INCLUDE_DIR}
        ${FFTW3_INCLUDE_DIR})

    # Shepp-Logan phantom
    add_executable(ismrmrd_generate_cartesian_shepp_logan
        generate_cartesian_shepp_logan.cpp
        ismrmrd_phantom.cpp)
    if(WIN32)
        target_link_libraries( ismrmrd_generate_cartesian_shepp_logan
            ismrmrd
            ${FFTW3_LIBRARIES})
    else(WIN32)
        target_link_libraries( ismrmrd_generate_cartesian_shepp_logan
            ismrmrd
            ${Boost_PROGRAM_OPTIONS_LIBRARY}
            ${FFTW3_LIBRARIES})
    endif(WIN32)
    install(TARGETS ismrmrd_generate_cartesian_shepp_logan DESTINATION bin)

    # Shepp-Logan phantom
    add_executable(ismrmrd_recon_cartesian_2d
	recon_cartesian_2d.cpp)
    if(WIN32)
        target_link_libraries( ismrmrd_recon_cartesian_2d
            ismrmrd
            ${FFTW3_LIBRARIES})
    else(WIN32)
        target_link_libraries( ismrmrd_recon_cartesian_2d
            ismrmrd
            ${Boost_PROGRAM_OPTIONS_LIBRARY}
            ${FFTW3_LIBRARIES})
    endif(WIN32)
    install(TARGETS ismrmrd_recon_cartesian_2d DESTINATION bin)

else(FFTW3_FOUND AND Boost_FOUND)
	message("FFTW3 or Boost NOT Found, cannot build utilities")
endif(FFTW3_FOUND AND Boost_FOUND)
