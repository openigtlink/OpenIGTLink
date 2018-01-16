# - The AV1 library
# Once done this will define
#
#  Open - A list of search hints
#
#  AV1_FOUND - found AV1
#  AV1_INCLUDE_DIR - the AV1 include directory
#  AV1_LIBRARY - AV1 library

SET( AV1_PATH_HINTS 
    ${AV1_ROOT} 
    ${AV1_INCLUDE_DIR}
    ${AV1_LIBRARY_DIR}
    )
    
unset(AV1_INCLUDE_DIR CACHE)
find_path(AV1_INCLUDE_DIR NAMES aom.h aom_encoder.h aom_decoder.h 
  PATH_SUFFIXES aom
  HINTS ${AV1_PATH_HINTS}  
  )
unset(AV1_LIBRARY CACHE)
IF(WIN32)
  IF(OpenIGTLink_BUILD_SHARED_LIBS)
    message(FATAL_ERROR "Windows DLL builds not supported yet")
  ENDIF()
  find_library(AV1_LIBRARY
     NAMES aom.lib
     PATH_SUFFIXES ${CMAKE_BUILD_TYPE}
     HINTS ${AV1_PATH_HINTS}
     )
ELSE()
  IF(OpenIGTLink_BUILD_SHARED_LIBS)
    find_library(AV1_LIBRARY
       NAMES libaom.so
       PATH_SUFFIXES ${CMAKE_BUILD_TYPE}
       HINTS ${AV1_PATH_HINTS}
       )
  ELSE()
    find_library(AV1_LIBRARY
       NAMES libaom.a
       PATH_SUFFIXES ${CMAKE_BUILD_TYPE}
       HINTS ${AV1_PATH_HINTS}
       )
  ENDIF()     
ENDIF()   
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(AV1 REQUIRED_VARS AV1_LIBRARY AV1_INCLUDE_DIR)

mark_as_advanced(AV1_INCLUDE_DIR AV1_LIBRARY)
