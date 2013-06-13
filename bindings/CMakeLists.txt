find_package(SWIG)

if(SWIG_FOUND)

    include(${SWIG_USE_FILE})

    find_package(PythonLibs)
    if(PYTHONLIBS_FOUND)
        add_subdirectory(python)
    else(PYTHONLIBS_FOUND)
        message("PythonLibs not found. Not building Python bindings")
    endif(PYTHONLIBS_FOUND)

    if(DEFINED JAVA_INCLUDE_DIR)
        add_subdirectory(java)
    else()
        message("JAVA_INCLUDE_DIR not defined. Not building Java bindings")
    endif()

endif(SWIG_FOUND)
