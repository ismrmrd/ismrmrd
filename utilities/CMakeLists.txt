find_package(Boost COMPONENTS program_options)
find_package(FFTW3 COMPONENTS single)

if(FFTW3_FOUND)
	message("FFTW3 Found, building utilities")
	include_directories(${ISMRMRD_SOURCE_DIR} ${ISMRMRD_SCHEMA_SOURCE_DIR}
		${Boost_INCLUDE_DIR} ${FFTW3_INCLUDE_DIR})
	add_library(ismrmrd_utilities SHARED ismrmrd_phantom.cpp)
	install(TARGETS ismrmrd_utilities DESTINATION ${ISMRMRD_INSTALL_LIB_DIR})	
	
	if(WIN32)
		set_source_files_properties(${XSDS_SOURCES} PROPERTIES GENERATED TRUE)
		add_executable(ismrmrd_generate_cartesian_shepp_logan
			generate_cartesian_shepp_logan.cpp ${XSDS_SOURCES})
		target_link_libraries(ismrmrd_generate_cartesian_shepp_logan
			ismrmrd ismrmrd_utilities ${Boost_LIBRARIES} ${FFTW3_LIBRARIES}
			${XERCESC_LIBRARIES})
	else(WIN32)
		add_executable(ismrmrd_generate_cartesian_shepp_logan
			generate_cartesian_shepp_logan.cpp)
		target_link_libraries(ismrmrd_generate_cartesian_shepp_logan
			ismrmrd ismrmrd_utilities ismrmrd_xsd
			${Boost_PROGRAM_OPTIONS_LIBRARY} ${FFTW3_LIBRARIES})
	endif(WIN32)
		install(TARGETS ismrmrd_generate_cartesian_shepp_logan
			DESTINATION ${ISMRMRD_INSTALL_BIN_DIR})
else(FFTW3_FOUND)
	message("FFTW3 NOT Found, cannot build utilities")
endif(FFTW3_FOUND)
