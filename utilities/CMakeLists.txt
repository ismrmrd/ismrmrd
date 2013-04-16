find_package(Boost COMPONENTS program_options REQUIRED)
find_package(FFTW3 COMPONENTS single)

SET_SOURCE_FILES_PROPERTIES(${XSDS_SOURCES} PROPERTIES GENERATED TRUE)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/..  ${Boost_INCLUDE_DIR})

ADD_LIBRARY(ismrmrd_utilities SHARED ismrmrd_phantom.cpp)
INSTALL(TARGETS ismrmrd_utilities DESTINATION lib)

IF(FFTW3_FOUND)
	INCLUDE_DIRECTORIES(${FFTW3_INCLUDE_DIR})
	ADD_EXECUTABLE(ismrmrd_generate_cartesian_shepp_logan generate_cartesian_shepp_logan.cpp ${XSDS_SOURCES})
if(WIN32)
	TARGET_LINK_LIBRARIES(ismrmrd_generate_cartesian_shepp_logan ismrmrd_utilities ismrmrd ${FFTW3_LIBRARIES} ${XERCESC_LIBRARIES})
else(WIN32)
	TARGET_LINK_LIBRARIES(ismrmrd_generate_cartesian_shepp_logan ismrmrd_utilities ismrmrd ${FFTW3_LIBRARIES} ${Boost_LIBRARIES} ${XERCESC_LIBRARIES})
endif(WIN32)
	INSTALL(TARGETS ismrmrd_generate_cartesian_shepp_logan DESTINATION bin)
ENDIF(FFTW3_FOUND)