find_package(Boost COMPONENTS program_options REQUIRED REQUIRED)
find_package(FFTW3 COMPONENTS single)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/..  ${Boost_INCLUDE_DIR})

ADD_LIBRARY(ismrmrd_utilities SHARED ismrmrd_phantom.cpp)
INSTALL(TARGETS ismrmrd_utilities DESTINATION lib)

IF(FFTW3_FOUND)

	#Process the XSD files
	SET(XSDS ${CMAKE_CURRENT_SOURCE_DIR}/../schema/ismrmrd.xsd)
	SET(XSD_ARGS cxx-tree --generate-serialization)
	WRAP_XSD(XSDS_SOURCES XSD_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/schema ${XSDS} OPTIONS ${XSD_ARGS})
	INCLUDE_DIRECTORIES(${XSD_INCLUDES} ${XSD_INCLUDE_DIR} ${XERCESC_INCLUDE_DIR})

	INCLUDE_DIRECTORIES(${FFTW3_INCLUDE_DIR})
	ADD_EXECUTABLE(ismrmrd_generate_cartesian_shepp_logan generate_cartesian_shepp_logan.cpp ${XSDS_SOURCES})
	TARGET_LINK_LIBRARIES(ismrmrd_generate_cartesian_shepp_logan ismrmrd_utilities ismrmrd ${FFTW3_LIBRARIES} ${Boost_LIBRARIES} ${XERCESC_LIBRARIES})
	INSTALL(TARGETS ismrmrd_generate_cartesian_shepp_logan DESTINATION bin)
ENDIF(FFTW3_FOUND)