add_library(ismrmrd_xml SHARED 
  ismrmrd_xml.h
  ismrmrd_xml.cpp
  pugixml.cpp
  )


include_directories(${ISMRMRD_SOURCE_DIR})

add_executable(dump_ismrmrd_header
  dump_ismrmrd_header.cpp
)

target_link_libraries(dump_ismrmrd_header
  ismrmrd
  ismrmrd_xml)

if(WIN32)
    target_link_libraries(ismrmrd_xml optimized ${HDF5_hdf5_LIBRARY_RELEASE}
        optimized ${HDF5_hdf5_cpp_LIBRARY_RELEASE})
    target_link_libraries(ismrmrd_xml debug ${HDF5_hdf5_LIBRARY_DEBUG}
        debug ${HDF5_hdf5_cpp_LIBRARY_DEBUG})
    target_link_libraries(ismrmrd_xml ${Boost_LIBRARIES})
else(WIN32)
    target_link_libraries(ismrmrd_xml ${HDF5_LIBRARIES} ${Boost_THREAD_LIBRARY}
        ${Boost_SYSTEM_LIBRARY})
endif(WIN32)

install (FILES ismrmrd_xml.h
  DESTINATION ${ISMRMRD_INSTALL_INCLUDE_DIR})

install (TARGETS ismrmrd_xml DESTINATION ${ISMRMRD_INSTALL_LIB_DIR})

install (TARGETS dump_ismrmrd_header DESTINATION ${ISMRMRD_INSTALL_BIN_DIR})
