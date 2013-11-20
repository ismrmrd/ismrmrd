include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${PYTHON_INCLUDE_PATH} ${NUMPY_INCLUDE_DIRS})
include_directories(${HDF5_INCLUDE_DIR} ${HDF5_INCLUDE_DIR}/cpp ${Boost_INCLUDE_DIR})

if ( WIN32 )
    add_definitions(-Dismrmrd_EXPORTS)
endif ( WIN32 )

set_source_files_properties(ismrmrd_python.i PROPERTIES CPLUSPLUS ON)

swig_add_module(ismrmrd python ismrmrd_python.i ${CMAKE_SOURCE_DIR}/ismrmrd_hdf5.cpp)
swig_link_libraries(ismrmrd ${HDF5_LIBRARIES} ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})

install(TARGETS ${SWIG_MODULE_ismrmrd_REAL_NAME} DESTINATION python)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ismrmrd.py DESTINATION python)

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

    install(FILES ${XSD_PY_FILE} DESTINATION python)
    install(DIRECTORY ${XSD_PY_DIR} DESTINATION python)

else(PYXBGEN)
    message("Can't find pyxbgen executable. Not building ismrmrd_xsd.py")
endif(PYXBGEN)
