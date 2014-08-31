add_library(ismrmrd_xml SHARED 
  ismrmrd_xml.h
  ismrmrd_xml.cpp
  ismrmrd_meta.h
  ismrmrd_meta.cpp
  pugixml.cpp
  )

set_target_properties(ismrmrd_xml PROPERTIES VERSION ${ISMRMRD_VERSION_STRING} SOVERSION ${ISMRMRD_VERSION_MAJOR})

add_executable(test_ismrmrd_xml
  test_ismrmrd_xml.cpp
  pugixml.cpp
)

add_executable(test_meta 
  test_meta.cpp
)

target_link_libraries(test_ismrmrd_xml
  ismrmrd_xml)

target_link_libraries(test_meta
  ismrmrd_xml)

install (FILES ismrmrd_xml.h ismrmrd_xml_export.h
  DESTINATION ${ISMRMRD_INSTALL_INCLUDE_DIR})

install (TARGETS ismrmrd_xml DESTINATION ${ISMRMRD_INSTALL_LIB_DIR})

install (TARGETS test_ismrmrd_xml DESTINATION ${ISMRMRD_INSTALL_BIN_DIR})
