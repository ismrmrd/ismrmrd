# - Find ISMRMRRD
#   ISMRMRD_FOUND       - True if ISMRMRD found.
#   ISMRMRD_INCLUDE_DIR - where to find ismrmrd.h, etc.

FIND_PATH( ISMRMRD_INCLUDE_DIR "ismrmrd.h"
           PATH_SUFFIXES "ismrmrd" )

# handle the QUIETLY and REQUIRED arguments and set TINYXML_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE( "FindPackageHandleStandardArgs" )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( "Ismrmrd" DEFAULT_MSG ISMRMRD_INCLUDE_DIR)

MARK_AS_ADVANCED( ISMRMRD_INCLUDE_DIR)