# - The OPENHEVC library
# Once done this will define
#
#  Open - A list of search hints
#
#  OpenHEVC_FOUND - found OpenHEVC
#  OpenHEVC_INCLUDE_DIR - the OpenHEVC include directory
#  OpenHEVC_LIBRARY - OpenHEVC library

SET( OpenHEVC_PATH_HINTS 
    ${OpenHEVC_ROOT} 
    ${OpenHEVC_INCLUDE_DIR}
    )
    
unset(OpenHEVC_INCLUDE_DIR CACHE)
find_path(OpenHEVC_INCLUDE_DIR NAMES openHevcWrapper.h 
  PATH_SUFFIXES gpac/modules/openhevc_dec
  HINTS ${OpenHEVC_PATH_HINTS} 
  )
unset(OpenHEVC_LIBRARY CACHE)
IF(WIN32)
  find_library(OpenHEVC_LIBRARY
     NAMES LibOpenHevcWrapper${CMAKE_STATIC_LIBRARY_SUFFIX}
     PATH_SUFFIXES ${CMAKE_BUILD_TYPE}
     HINTS ${OpenHEVC_PATH_HINTS}
     )
ELSE()
  find_library(OpenHEVC_LIBRARY
     NAMES libLibOpenHevcWrapper.a
     PATH_SUFFIXES ${CMAKE_BUILD_TYPE}
     HINTS ${OpenHEVC_PATH_HINTS}
     )
ENDIF()   
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenHEVC REQUIRED_VARS OpenHEVC_LIBRARY OpenHEVC_INCLUDE_DIR)

mark_as_advanced(OpenHEVC_INCLUDE_DIR OpenHEVC_LIBRARY)
