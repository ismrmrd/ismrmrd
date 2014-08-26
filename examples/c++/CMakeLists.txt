cmake_minimum_required(VERSION 2.8)
project(ISMRMRD_C++_EXAMPLES)

if (NOT DEFINED ENV{ISMRMRD_HOME})
    message(FATAL_ERROR "Environment variable $ISMRMRD_HOME not set")
endif()
# for finding Ismrmrd
list (APPEND CMAKE_MODULE_PATH "$ENV{ISMRMRD_HOME}/cmake")

find_package(Ismrmrd REQUIRED)

include_directories (
    ${ISMRMRD_INCLUDE_DIR}
)

add_executable(ismrmrd_basic_test basic_test.cpp)
target_link_libraries(ismrmrd_basic_test ${ISMRMRD_LIBRARIES})

#find_package(FFTW3 COMPONENTS single)
#if(FFTW3_FOUND)
#    message("FFTW3 Found, building examples")
#    add_executable(ismrmrd_create_dataset test_create_dataset.cpp)
#    target_link_libraries(ismrmrd_create_dataset ismrmrd ${FFTW3_LIBRARIES})
#    add_executable(ismrmrd_recon_dataset test_recon_dataset.cpp)
#    target_link_libraries(ismrmrd_recon_dataset ismrmrd ${FFTW3_LIBRARIES})
#    install(TARGETS ismrmrd_create_dataset
#                              ismrmrd_recon_dataset
#              DESTINATION bin)
#else(FFTW3_FOUND)
#       message("FFTW3 NOT Found, cannot build examples")
#endif(FFTW3_FOUND)

if(0)

find_package(FFTW3 COMPONENTS single)

if(FFTW3_FOUND)
    message("FFTW3 Found, building examples")
    include_directories(${ISMRMRD_SOURCE_DIR} ${ISMRMRD_SCHEMA_SOURCE_DIR}
        ${FFTW3_INCLUDE_DIR})
    if(WIN32)
        # building with xsd sources on Windows
        set_source_files_properties(${XSDS_SOURCES} PROPERTIES GENERATED TRUE)
        add_executable(ismrmrd_create_dataset test_create_dataset.cpp ${XSDS_SOURCES})
        target_link_libraries(ismrmrd_create_dataset ${XERCESC_LIBRARIES} ismrmrd ${FFTW3_LIBRARIES})
        add_executable(ismrmrd_recon_dataset test_recon_dataset.cpp ${XSDS_SOURCES})
        target_link_libraries(ismrmrd_recon_dataset ${XERCESC_LIBRARIES} ismrmrd ${FFTW3_LIBRARIES})
        add_executable(ismrmrd_publication_recon publication_recon_dataset.cpp ${XSDS_SOURCES})
        target_link_libraries(ismrmrd_publication_recon ${XERCESC_LIBRARIES} ismrmrd ${FFTW3_LIBRARIES})
        add_executable(read_timing_test read_timing_test.cpp ${XSDS_SOURCES})
        target_link_libraries(read_timing_test ${XERCESC_LIBRARIES} ismrmrd ${FFTW3_LIBRARIES})
    else(WIN32)
        # linking with ismrmrd_xsd on Linux / OSX
        add_executable(ismrmrd_create_dataset test_create_dataset.cpp)
        target_link_libraries(ismrmrd_create_dataset ismrmrd ismrmrd_xsd ${FFTW3_LIBRARIES})
        add_executable(ismrmrd_recon_dataset test_recon_dataset.cpp)
        target_link_libraries(ismrmrd_recon_dataset ismrmrd ismrmrd_xsd ${FFTW3_LIBRARIES})
        add_executable(ismrmrd_publication_recon publication_recon_dataset.cpp)
        target_link_libraries(ismrmrd_publication_recon ismrmrd ismrmrd_xsd ${FFTW3_LIBRARIES})
        add_executable(read_timing_test read_timing_test.cpp)
        target_link_libraries(read_timing_test ismrmrd ismrmrd_xsd ${FFTW3_LIBRARIES})
    endif(WIN32)
    install(TARGETS ismrmrd_create_dataset DESTINATION ${ISMRMRD_INSTALL_BIN_DIR})
    install(TARGETS ismrmrd_recon_dataset DESTINATION ${ISMRMRD_INSTALL_BIN_DIR})
    install(TARGETS ismrmrd_publication_recon DESTINATION ${ISMRMRD_INSTALL_BIN_DIR})
    install(TARGETS read_timing_test DESTINATION ${ISMRMRD_INSTALL_BIN_DIR})
else(FFTW3_FOUND)
       message("FFTW3 NOT Found, cannot build examples")
endif(FFTW3_FOUND)

endif(0)
