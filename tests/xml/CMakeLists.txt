add_executable(ismrmrd_test_xml
  test_xml.cpp
  pugixml.cpp
)

target_link_libraries(ismrmrd_test_xml ismrmrd)

add_executable(ismrmrd_test_meta 
  test_meta.cpp
  pugixml.cpp
)

target_link_libraries(ismrmrd_test_meta ismrmrd)

install (TARGETS ismrmrd_test_xml ismrmrd_test_meta DESTINATION bin)

