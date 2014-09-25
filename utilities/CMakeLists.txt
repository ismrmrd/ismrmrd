find_package(Boost COMPONENTS program_options)
find_package(FFTW3 COMPONENTS single)

if(WIN32)
    link_directories(${Boost_LIBRARY_DIRS})
endif(WIN32)

if(FFTW3_FOUND)
    message("FFTW3 Found, building utilities")
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
    install(TARGETS ismrmrd_generate_cartesian_shepp_logan
			DESTINATION bin)

else(FFTW3_FOUND)
	message("FFTW3 NOT Found, cannot build utilities")
endif(FFTW3_FOUND)
