# Build the info application
message("Building info application")
include_directories(${CMAKE_BINARY_DIR}/include
                    ${CMAKE_SOURCE_DIR}/include
		    ${CMAKE_SOURCE_DIR}/libsrc)

add_executable(ismrmrd_info ismrmrd_info.cpp)
target_link_libraries(ismrmrd_info ismrmrd)
install(TARGETS ismrmrd_info DESTINATION bin)

if (NOT WIN32)
  add_executable(ismrmrd_test_xml
    ismrmrd_test_xml.cpp)
  target_link_libraries(ismrmrd_test_xml ismrmrd)
  install(TARGETS ismrmrd_test_xml DESTINATION bin)
endif()

if (HDF5_FOUND)
    add_executable(ismrmrd_read_timing_test read_timing_test.cpp)
    target_link_libraries(ismrmrd_read_timing_test ismrmrd ${HDF5_C_LIBRARIES})


    if (WIN32 AND (CMAKE_VERSION VERSION_GREATER_EQUAL 3.21))
     install(TARGETS ismrmrd_read_timing_test DESTINATION bin 
            RUNTIME_DEPENDENCIES
            PRE_EXCLUDE_REGEXES "api-ms-" "ex-ms"
            POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
            DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/.."
            )
    else ()
        install(TARGETS ismrmrd_read_timing_test DESTINATION bin)
        endif()

    find_package(Boost 1.43 COMPONENTS program_options random)
    if (WIN32)
        add_definitions(-DBOOST_ALL_NO_LIB)
        add_definitions(-DBOOST_ALL_DYN_LINK)
    endif()
    find_package(FFTW3 COMPONENTS single)

    if(FFTW3_FOUND AND Boost_FOUND)
        message("FFTW3 and Boost Found... building utilities")

        if(WIN32)
            link_directories(${Boost_LIBRARY_DIRS})
        endif()

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

        install(TARGETS ismrmrd_generate_cartesian_shepp_logan DESTINATION bin)

        # Shepp-Logan phantom
        add_executable(ismrmrd_recon_cartesian_2d
            recon_cartesian_2d.cpp)
		target_link_libraries( ismrmrd_recon_cartesian_2d
            ismrmrd
            ${Boost_PROGRAM_OPTIONS_LIBRARY}
            ${FFTW3_LIBRARIES})

        install(TARGETS ismrmrd_recon_cartesian_2d DESTINATION bin)

        add_executable(ismrmrd_hdf5_to_stream ismrmrd_hdf5_to_stream.cpp)
        target_link_libraries(ismrmrd_hdf5_to_stream ismrmrd ${Boost_PROGRAM_OPTIONS_LIBRARY})
        install(TARGETS ismrmrd_hdf5_to_stream DESTINATION bin)

        add_executable(ismrmrd_stream_to_hdf5 ismrmrd_stream_to_hdf5.cpp)
        target_link_libraries(ismrmrd_stream_to_hdf5 ismrmrd ${Boost_PROGRAM_OPTIONS_LIBRARY})
        install(TARGETS ismrmrd_stream_to_hdf5 DESTINATION bin)

        add_executable(ismrmrd_stream_recon_cartesian_2d stream_recon_cartesian_2d.cpp)
        target_link_libraries(ismrmrd_stream_recon_cartesian_2d ismrmrd ${FFTW3_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARY})
        

        if (WIN32)       
            install(TARGETS ismrmrd_stream_recon_cartesian_2d DESTINATION bin 
            RUNTIME_DEPENDENCIES
            PRE_EXCLUDE_REGEXES "api-ms-" "ex-ms"
            POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
            DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/.."
            )
            else()
            install(TARGETS ismrmrd_stream_recon_cartesian_2d DESTINATION bin)
        endif()


    else()
        message("FFTW3 or Boost NOT Found, cannot build utilities")
    endif()
else ()
    message("HDF5 NOT Found, cannot build utilities")
endif ()
