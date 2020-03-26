# - The VP9 library
# Once done this will define
#
#  VP9_ROOT - A list of search hints
#
#  VP9_FOUND - found VP9
#  VP9_INCLUDE_DIR - the VP9 include directory
#  VP9_LIBRARY_DIR - VP9 library directory
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows") 
  SET( VP9_PATH_HINTS 
      ${VP9_ROOT} 
      ${VP9_INCLUDE_DIR}
      ${VP9_LIBRARY_DIR} 
      )
  unset(VP9_INCLUDE_DIR CACHE)
  find_path(VP9_INCLUDE_DIR NAMES vp8cx.h vpx_image.h 
    PATH_SUFFIXES vpx
    HINTS ${VP9_PATH_HINTS} 
    )
    
  unset(VP9_LIBRARY_DIR CACHE)
  find_path(VP9_LIBRARY_DIR
     NAMES libvpx.a
     PATH_SUFFIXES ${Platform}/${CMAKE_BUILD_TYPE}
     HINTS ${VP9_PATH_HINTS}
     )
else()
  SET(VP9_PATH_HINTS 
      ${VP9_ROOT} 
      ${VP9_INCLUDE_DIR}
      )
  find_path(VP9_INCLUDE_DIRECT_DIR NAMES vp8cx.h vpx_image.h
    PATH_SUFFIXES vpx
    HINTS ${VP9_PATH_HINTS} 
    )  
  SET(VP9_PATH_HINTS 
      ${VP9_ROOT} 
      )
      
  list(APPEND VP9_PATH_HINTS  ${VP9_LIBRARY_DIR}/lib)
  
  find_path(VP9_LIBRARY_DIRECT_DIR libvpx.lib
     HINTS  ${VP9_PATH_HINTS} 
     )     
  if(VP9_LIBRARY_DIRECT_DIR AND VP9_INCLUDE_DIRECT_DIR)
    #VP9 library and header files are all found
    SET(VP9_INCLUDE_DIR "${VP9_INCLUDE_DIRECT_DIR}" CACHE PATH "" FORCE)
    unset(VP9_LIBRARY_DIRECT_DIR  CACHE) 
    unset(VP9_INCLUDE_DIRECT_DIR  CACHE) 
  else()
    unset(VP9_INCLUDE_DIRECT_DIR  CACHE) 
    unset(VP9_LIBRARY_DIRECT_DIR  CACHE) # don't expose the VP9_LIBRARY_DIRECT_DIR to user, force the user to set the variable VP9_LIBRARY_DIR
    MESSAGE(WARNING "VP9 library file not found, specify the path where the vp9 project was build, if vp9 was built in source, then set the library path the same as include path")
    SET(VP9_LIBRARY_DIR "" CACHE PATH "" FORCE)
    SET(VP9_INCLUDE_DIR "" CACHE PATH "" FORCE)
  endif()
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(VP9 REQUIRED_VARS VP9_LIBRARY_DIR VP9_INCLUDE_DIR)
mark_as_advanced(VP9_INCLUDE_DIR VP9_LIBRARY_DIR)
 