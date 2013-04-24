include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${PYTHON_INCLUDE_PATH} ${PYTHON_NUMPY_INCLUDE_DIR})
include_directories(${HDF5_INCLUDE_DIR} ${HDF5_INCLUDE_DIR}/cpp ${Boost_INCLUDE_DIR})

set_source_files_properties(ismrmrd_python.i PROPERTIES CPLUSPLUS ON)

swig_add_module(ismrmrd python ismrmrd_python.i ../../ismrmrd_hdf5.cpp)
swig_link_libraries(ismrmrd ${HDF5_LIBRARIES} ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})

install(TARGETS ${SWIG_MODULE_ismrmrd_REAL_NAME} DESTINATION lib)

find_program(PYXBGEN pyxbgen
    HINTS ${PYTHON_HOME} ${PYTHON_LIBRARY}/..
    PATH_SUFFIXES bin)

if(PYXBGEN)
    set(SCHEMA "${CMAKE_SOURCE_DIR}/schema/ismrmrd.xsd")
    set(XSD_PY "${CMAKE_CURRENT_BINARY_DIR}/ismrmrd_xsd.py")
    add_custom_command(
        OUTPUT ${XSD_PY}
        COMMAND ${PYXBGEN} -u "${SCHEMA}" -m ismrmrd_xsd
        DEPENDS ${SCHEMA})
    add_custom_target(pyismrmrd_xsd ALL DEPENDS ${XSD_PY})
    install(FILES ${XSD_PY} DESTINATION lib)
else(PYXBGEN)
    message("Can't find pyxbgen executable. Not building ismrmrd_xsd.py")
endif(PYXBGEN)
