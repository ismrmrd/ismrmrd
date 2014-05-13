enable_testing()

include_directories(${ISMRMRD_SOURCE_DIR} ${ISMRMRD_SCHEMA_SOURCE_DIR})

if(WIN32)
    # building with xsd sources on Windows
    set_source_files_properties(${XSDS_SOURCES} PROPERTIES GENERATED TRUE)
    add_executable(t_ndarraycontainer t_ndarraycontainer.cpp ${XSDS_SOURCES})
    target_link_libraries(t_ndarraycontainer ${XERCESC_LIBRARIES} ismrmrd)
    add_test(t_ndarraycontainer ${CMAKE_CURRENT_BINARY_DIR}/t_ndarraycontainer)
    add_executable(t_image t_image.cpp ${XSDS_SOURCES})
    target_link_libraries(t_image ${XERCESC_LIBRARIES} ismrmrd)
    add_test(t_image ${CMAKE_CURRENT_BINARY_DIR}/t_image)
else(WIN32)
    # linking with ismrmrd_xsd on Linux / OSX
    add_executable(t_ndarraycontainer t_ndarraycontainer.cpp)
    target_link_libraries(t_ndarraycontainer ismrmrd ismrmrd_xsd)
    add_test(t_ndarraycontainer ${CMAKE_CURRENT_BINARY_DIR}/t_ndarraycontainer)
    add_executable(t_image t_image.cpp)
    target_link_libraries(t_image ismrmrd ismrmrd_xsd)
    add_test(t_image ${CMAKE_CURRENT_BINARY_DIR}/t_image)
endif(WIN32)
