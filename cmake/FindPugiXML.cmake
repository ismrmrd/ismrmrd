# Copyrigt 2012 Kamil Rytarowski <kamil AT mageia DOT org>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

find_path(PugiXML_INCLUDE_DIR pugixml.hpp /usr/include)

find_library(PugiXML_LIBRARY
  NAMES
  "pugixml"
  PATHS
  /usr/lib
  )

if(PugiXML_LIBRARY AND PugiXML_INCLUDE_DIR)
  set(PugiXML_FOUND 1)
  mark_as_advanced(PugiXML_LIBRARY PugiXML_INCLUDE_DIR)
else()
  set(PugiXML_FOUND 0)
endif()
  
if(NOT PugiXML_FOUND)
  if(PugiXML_FIND_REQUIRED)
    message(FATAL_ERROR "PugiXML was not found.")
  endif()
endif()