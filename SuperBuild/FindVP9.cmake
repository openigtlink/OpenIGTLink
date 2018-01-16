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
  include(FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(VP9 REQUIRED_VARS VP9_LIBRARY_DIR VP9_INCLUDE_DIR)

  mark_as_advanced(VP9_INCLUDE_DIR VP9_LIBRARY_DIR)
else()
  SET(VP9_PATH_HINTS 
      ${VP9_ROOT} 
      ${VP9_INCLUDE_DIR}
      )
  
  unset(VP9_INCLUDE_DIR CACHE)
  find_path(VP9_INCLUDE_DIR NAMES vp8cx.h vpx_image.h 
    PATH_SUFFIXES vpx
    HINTS ${VP9_PATH_HINTS} 
    )
  
  if(NOT VP9_INCLUDE_DIR)
    MESSAGE(FATAL_ERROR "VP9 include files not found, specify the file path")
  endif()  
  
  set(VP9_LIBRARY_DIR "" CACHE PATH "")
  SET(VP9_PATH_HINTS 
      ${VP9_ROOT}
      ${VP9_LIBRARY_DIR}/Win32/Release 
      ${VP9_LIBRARY_DIR}/Win32/Debug 
      ${VP9_LIBRARY_DIR}/x64/Release 
      ${VP9_LIBRARY_DIR}/x64/Debug
      )

  find_path(VP9_LIBRARY_DIRECT_DIR vpxmdd.lib | vpxmd.lib
     HINTS  ${VP9_PATH_HINTS}
     )   
  if(NOT VP9_LIBRARY_DIRECT_DIR)
    unset(VP9_LIBRARY_DIRECT_DIR  CACHE) # don't expose the VP9_LIBRARY_DIRECT_DIR to user, force the user to set the variable VP9_LIBRARY_DIR
    MESSAGE(FATAL_ERROR "VP9 library file not found, specify the path where the vp9 project was build, if vp9 was built in source, then set the library path the same as include path")
  else()
    unset(VP9_LIBRARY_DIRECT_DIR  CACHE)
    add_library(VP9_lib STATIC IMPORTED GLOBAL)
    if(NOT "${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
      set_property(TARGET VP9_lib PROPERTY IMPORTED_LOCATION_RELEASE ${VP9_LIBRARY_DIR}/Win32/Release/vpxmd.lib)
      set_property(TARGET VP9_lib PROPERTY IMPORTED_LOCATION_DEBUG ${VP9_LIBRARY_DIR}/Win32/Debug/vpxmdd.lib)
    else()
      set_property(TARGET VP9_lib PROPERTY IMPORTED_LOCATION_RELEASE ${VP9_LIBRARY_DIR}/x64/Release/vpxmd.lib)
      set_property(TARGET VP9_lib PROPERTY IMPORTED_LOCATION_DEBUG ${VP9_LIBRARY_DIR}/x64/Debug/vpxmdd.lib)
    endif()     
  endif()
endif()
