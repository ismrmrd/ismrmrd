find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(API_DOC_DIR ${CMAKE_CURRENT_BINARY_DIR}/html/api)
    set(DOXYFILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${DOXYFILE} @ONLY)

    add_custom_target(apidoc
        COMMAND ${CMAKE_COMMAND} -E make_directory ${API_DOC_DIR}
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
        COMMENT "Generating API documentation using Doxygen" VERBATIM)
else()
    message(STATUS "Doxygen not found. Not able to build API documentation")
endif()

find_program(SPHINXBUILD sphinx-build)
if(SPHINXBUILD)
    if(WIN32)
        set(SPHINX_MAKEBAT ${CMAKE_CURRENT_BINARY_DIR}/make.sphinx.bat)
        configure_file(make.sphinx.bat.in ${SPHINX_MAKEBAT} @ONLY)
        add_custom_target(htmldoc
            COMMAND ${SPHINX_MAKEBAT} html
            COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/html/.nojekyll
            COMMENT "Generating HTML docs using Sphinx" VERBATIM)
    else()
        set(SPHINX_MAKEFILE ${CMAKE_CURRENT_BINARY_DIR}/Makefile.sphinx)
        configure_file(Makefile.sphinx.in ${SPHINX_MAKEFILE} @ONLY)
        add_custom_target(htmldoc
            COMMAND make -f ${SPHINX_MAKEFILE} html
            COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/html/.nojekyll
            COMMENT "Generating HTML docs using Sphinx" VERBATIM)
    endif()
else()
    message(STATUS "sphinx-build not found. Not able to build HTML project documentation")
endif()

add_custom_target(doc)
add_dependencies(doc apidoc htmldoc)
