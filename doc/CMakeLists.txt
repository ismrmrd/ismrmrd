add_custom_target(doc)
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(API_DOC_DIR ${CMAKE_CURRENT_BINARY_DIR}/html/api)
    set(DOXYFILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${DOXYFILE} @ONLY)

    add_custom_target(apidoc
        COMMAND ${CMAKE_COMMAND} -E make_directory ${API_DOC_DIR}
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
        COMMENT "Generating API documentation using Doxygen" VERBATIM)
    add_dependencies(doc apidoc)
else()
    message(STATUS "Doxygen not found. Not able to build API documentation")
endif()

