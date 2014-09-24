find_package(Boost COMPONENTS program_options)
find_package(FFTW3 COMPONENTS single)

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
    target_link_libraries( ismrmrd_generate_cartesian_shepp_logan
        ismrmrd
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        ${FFTW3_LIBRARIES})
    install(TARGETS ismrmrd_generate_cartesian_shepp_logan
			DESTINATION bin)

else(FFTW3_FOUND)
	message("FFTW3 NOT Found, cannot build utilities")
endif(FFTW3_FOUND)
