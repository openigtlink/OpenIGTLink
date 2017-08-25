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
      ${CMAKE_BINARY_DIR}/Deps/VP9
      ${CMAKE_BINARY_DIR}/Deps/VP9-bin
      )
  set(VP9_INCLUDE_DIR "")
  find_path(VP9_INCLUDE_DIR NAMES vp8cx.h vpx_image.h 
    PATH_SUFFIXES vpx
    HINTS ${VP9_PATH_HINTS} 
    )
    
  set(VP9_LIBRARY_DIR "")
  find_path(VP9_LIBRARY_DIR
     NAMES libvpx.a
     PATH_SUFFIXES ${Platform}/${CMAKE_BUILD_TYPE}
     HINTS ${VP9_PATH_HINTS}
     )
  include(FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(VP9 REQUIRED_VARS VP9_LIBRARY_DIR VP9_INCLUDE_DIR)

  mark_as_advanced(VP9_INCLUDE_DIR VP9_LIBRARY_DIR)
else()
  set(VP9_INCLUDE_DIR "" CACHE PATH "VP9 library directory")
  find_file(VP9_files NAMES vp8cx.h 
    PATH_SUFFIXES vpx
    HINTS ${VP9_INCLUDE_DIR} 
    )
  if(NOT VP9_files)
    MESSAGE(FATAL_ERROR "VP9 include files not found, specify the file path")
  endif()  
  
  SET(VP9_LIBRARY_DIR "" CACHE PATH "VP9 library directory")
  find_library(VP9_lib vpxmd.lib  
     HINTS ${VP9_LIBRARY_DIR}/Win32/Release ${VP9_LIBRARY_DIR}/Win32/Debug ${VP9_LIBRARY_DIR}/x64/Release ${VP9_LIBRARY_DIR}/x64/Debug
     )
  if(NOT VP9_lib)
    MESSAGE(FATAL_ERROR "VP9 library not found, specify the library path")
  else()
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
