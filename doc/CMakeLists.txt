find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(API_DOC_DIR ${CMAKE_CURRENT_BINARY_DIR}/html/api)
    set(DOXYFILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${DOXYFILE} @ONLY)

    add_custom_target(apidoc
        COMMAND ${CMAKE_COMMAND} -E make_directory ${API_DOC_DIR}
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
        COMMENT "Generating API documentation using Doxygen" VERBATIM)
else(DOXYGEN_FOUND)
    MESSAGE("Doxygen not found. Will not be able to build documentation")
endif(DOXYGEN_FOUND)

set(SPHINX_MAKEFILE ${CMAKE_CURRENT_BINARY_DIR}/Makefile.sphinx)
configure_file(Makefile.sphinx.in ${SPHINX_MAKEFILE} @ONLY)
add_custom_target(htmldoc
    COMMAND make -f ${SPHINX_MAKEFILE} html
    COMMENT "Generating HTML docs using Sphinx" VERBATIM)

add_custom_target(doc)
add_dependencies(doc apidoc htmldoc)
