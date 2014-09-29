find_package(SWIG)

if(SWIG_FOUND)

    include(${SWIG_USE_FILE})

    find_package(PythonLibs)
    if(PYTHONLIBS_FOUND)
        find_package(NumPy)
        if(NUMPY_FOUND)
            add_subdirectory(python)
        else(NUMPY_FOUND)
            message("NumPy not found. Not building Python bindings")
        endif(NUMPY_FOUND)
    else(PYTHONLIBS_FOUND)
        message("PythonLibs not found. Not building Python bindings")
    endif(PYTHONLIBS_FOUND)

endif(SWIG_FOUND)
