find_package(Boost REQUIRED)
find_package(FFTW3 COMPONENTS single)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/..  ${Boost_INCLUDE_DIR})

ADD_LIBRARY(ismrmrd_utilities SHARED ismrmrd_phantom.cpp)
INSTALL(TARGETS ismrmrd_utilities DESTINATION lib)

IF(FFTW3_FOUND)
	INCLUDE_DIRECTORIES(${FFTW3_INCLUDE_DIR})
	ADD_EXECUTABLE(ismrmrd_generate_cartesian_shepp_logan generate_cartesian_shepp_logan.cpp)
	TARGET_LINK_LIBRARIES(ismrmrd_generate_cartesian_shepp_logan ismrmrd_utilities ismrmrd ismrmrd_xsd ${FFTW3_LIBRARIES})
	INSTALL(TARGETS ismrmrd_generate_cartesian_shepp_logan DESTINATION bin)
ENDIF(FFTW3_FOUND)