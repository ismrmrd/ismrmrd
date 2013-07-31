find_package(SWIG)

if(SWIG_FOUND)

    include(${SWIG_USE_FILE})

    find_package(PythonLibs)
    if(PYTHONLIBS_FOUND)
        add_subdirectory(python)
    else(PYTHONLIBS_FOUND)
        message("PythonLibs not found. Not building Python bindings")
    endif(PYTHONLIBS_FOUND)

    find_package(JNI)
    if(JNI_FOUND)
        message(STATUS "JAVA Include Path: ${JAVA_INCLUDE_PATH}")
        add_subdirectory(java)
    else()
        message("JNI not found. Not building Java bindings")
    endif()

endif(SWIG_FOUND)
