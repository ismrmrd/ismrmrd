enable_testing()

SET_SOURCE_FILES_PROPERTIES(${XSDS_SOURCES} PROPERTIES GENERATED TRUE)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../.. ${CMAKE_CURRENT_BINARY_DIR}/schema)

add_executable(t_ndarraycontainer t_ndarraycontainer.cpp )
target_link_libraries(t_ndarraycontainer ${XERCESC_LIBRARIES} ismrmrd)
add_test(t_ndarraycontainer ${CMAKE_CURRENT_BINARY_DIR}/t_ndarraycontainer)

add_executable(t_image t_image.cpp ${XSDS_SOURCES})
target_link_libraries(t_image ${XERCESC_LIBRARIES} ismrmrd)
add_test(t_image ${CMAKE_CURRENT_BINARY_DIR}/t_image)
