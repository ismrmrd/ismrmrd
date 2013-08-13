find_package(FFTW3 COMPONENTS single)

SET_SOURCE_FILES_PROPERTIES(${XSDS_SOURCES} PROPERTIES GENERATED TRUE )

IF(FFTW3_FOUND)
       MESSAGE("FFTW3 Found, building test applications")
       INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../.. ${FFTW3_INCLUDE_DIR})

       add_executable(ismrmrd_create_dataset test_create_dataset.cpp ${XSDS_SOURCES})
       target_link_libraries(ismrmrd_create_dataset ${XERCESC_LIBRARIES} ismrmrd ${FFTW3_LIBRARIES})
       INSTALL(TARGETS ismrmrd_create_dataset DESTINATION bin)

       add_executable(ismrmrd_recon_dataset test_recon_dataset.cpp ${XSDS_SOURCES})
       target_link_libraries(ismrmrd_recon_dataset ${XERCESC_LIBRARIES} ismrmrd ${FFTW3_LIBRARIES})
       INSTALL(TARGETS ismrmrd_recon_dataset DESTINATION bin)

       add_executable(ismrmrd_publication_recon publication_recon_dataset.cpp ${XSDS_SOURCES})
       target_link_libraries(ismrmrd_publication_recon ${XERCESC_LIBRARIES} ismrmrd ${FFTW3_LIBRARIES})
       INSTALL(TARGETS ismrmrd_publication_recon DESTINATION bin)

       add_executable(read_timing_test read_timing_test.cpp ${XSDS_SOURCES})
       target_link_libraries(read_timing_test ${XERCESC_LIBRARIES} ismrmrd ${FFTW3_LIBRARIES})
       INSTALL(TARGETS read_timing_test DESTINATION bin)

ELSE(FFTW3_FOUND)
       MESSAGE("FFTW3 NOT Found....cannot build test applications")
ENDIF(FFTW3_FOUND)

