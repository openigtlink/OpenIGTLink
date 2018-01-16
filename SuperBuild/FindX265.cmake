# - The X265 library
# Once done this will define
#
#  X265_ROOT - A list of search hints
#
#  X265_FOUND - found X265
#  X265_INCLUDE_DIR - the X265 include directory
#  X265_LIBRARY_DIR - X265 library directory

#if (UNIX AND NOT ANDROID)
#  find_package(PkgConfig QUIET)
#  pkg_check_modules(PC_X265 QUIET X265)
#endif()

SET( X265_PATH_HINTS 
    ${X265_ROOT} 
    ${X265_INCLUDE_DIR}
    )
unset(X265_INCLUDE_DIR CACHE)
find_path(X265_INCLUDE_DIR NAMES x265.h 
  HINTS ${X265_PATH_HINTS} 
  PATH_SUFFIXES source
  )
set(X265_LIBRARY_DIR "" CACHE PATH "")
SET( X265_PATH_HINTS 
    ${X265_ROOT} 
    ${X265_LIBRARY_DIR}
    )
find_path(X265_LIBRARY_DIRECT_DIR x265-static${CMAKE_STATIC_LIBRARY_SUFFIX} | libx265.a
     HINTS  ${X265_PATH_HINTS}
     )   
if(NOT X265_LIBRARY_DIRECT_DIR)
    unset(X265_LIBRARY_DIRECT_DIR  CACHE) # don't expose the X265_LIBRARY_DIRECT_DIR to user, force the user to set the variable X265_LIBRARY_DIR
endif()     

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(X265 REQUIRED_VARS X265_LIBRARY_DIR X265_INCLUDE_DIR)

#mark_as_advanced(X265_INCLUDE_DIR X265_LIBRARY_DIR)
