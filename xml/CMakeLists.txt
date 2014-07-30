add_library(ismrmrd_xml SHARED 
  ismrmrd_xml.h
  ismrmrd_xml.cpp
  )

install (FILES ismrmrd_xml.h
  DESTINATION include)

install (TARGETS ismrmrd_xml DESTINATION lib)

