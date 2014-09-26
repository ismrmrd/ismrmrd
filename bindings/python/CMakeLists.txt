include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${PYTHON_INCLUDE_PATH} ${NUMPY_INCLUDE_DIRS})
include_directories(${HDF5_INCLUDE_DIR} ${HDF5_INCLUDE_DIR}/cpp)

if ( WIN32 )
    add_definitions(-Dismrmrd_EXPORTS)
endif ( WIN32 )

swig_add_module(ismrmrd python ismrmrd_python.i
    ${CMAKE_SOURCE_DIR}/libsrc/ismrmrd.c
    ${CMAKE_SOURCE_DIR}/libsrc/dataset.c
)

swig_link_libraries(ismrmrd ${HDF5_LIBRARIES} ${PYTHON_LIBRARIES})

set(PYTHON_INSTALL_DIR share/ismrmrd/python)

install(TARGETS ${SWIG_MODULE_ismrmrd_REAL_NAME}
    DESTINATION ${PYTHON_INSTALL_DIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ismrmrd.py
    DESTINATION ${PYTHON_INSTALL_DIR})

find_program(PYXBGEN pyxbgen
    HINTS ${PYTHON_HOME} ${PYTHON_LIBRARY}/..
    PATH_SUFFIXES bin)

if(PYXBGEN)
    set(SCHEMA "${CMAKE_SOURCE_DIR}/schema/ismrmrd.xsd")
    set(XSD_PY_FILE "${CMAKE_CURRENT_BINARY_DIR}/ismrmrd_xsd.py")
    set(XSD_PY_DIR "${CMAKE_CURRENT_BINARY_DIR}/raw")

    add_custom_command(
        OUTPUT ${XSD_PY_FILE} ${XSD_PY_DIR}
        COMMAND ${PYXBGEN} -r -u "${SCHEMA}" -m ismrmrd_xsd
        COMMAND ${CMAKE_COMMAND} -E echo "import" "pyxb.utils.domutils" >> ${XSD_PY_FILE}
        COMMAND ${CMAKE_COMMAND} -E echo "'pyxb.utils.domutils.BindingDOMSupport.SetDefaultNamespace(Namespace)'" >> ${XSD_PY_FILE}
        DEPENDS ${SCHEMA})

    add_custom_target(pyismrmrd_xsd ALL DEPENDS ${XSD_PY_FILE})

    install(FILES ${XSD_PY_FILE}
        DESTINATION ${PYTHON_INSTALL_DIR})
    install(DIRECTORY ${XSD_PY_DIR}
        DESTINATION ${PYTHON_INSTALL_DIR})
else(PYXBGEN)
    message("Can't find pyxbgen executable. Not building ismrmrd_xsd.py")
endif(PYXBGEN)
