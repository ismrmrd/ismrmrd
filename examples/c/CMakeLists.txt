find_package(FFTW3 COMPONENTS single)

if(NOT FFTW3_FOUND)
    message("FFTW3 NOT Found, cannot build examples")
    return()
endif(NOT FFTW3_FOUND)

message("FFTW3 Found, building C examples")
include_directories(${ISMRMRD_SOURCE_DIR} ${ISMRMRD_SCHEMA_SOURCE_DIR} ${FFTW3_INCLUDE_DIR})
if(WIN32)
    # building with xsd sources on Windows
    set_source_files_properties(${XSDS_SOURCES} PROPERTIES GENERATED TRUE)
    add_executable(ismrmrd_c_demo main.c ${XSDS_SOURCES})
    target_link_libraries(ismrmrd_c_demo ${XERCESC_LIBRARIES} ismrmrd ${FFTW3_LIBRARIES})
else(WIN32)
    # linking with ismrmrd_xsd on Linux / OSX
    add_executable(ismrmrd_c_demo main.c)
    target_link_libraries(ismrmrd_c_demo ismrmrd ismrmrd_xsd ${FFTW3_LIBRARIES})
endif(WIN32)

install(TARGETS ismrmrd_c_demo DESTINATION ${ISMRMRD_INSTALL_BIN_DIR})
