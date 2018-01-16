# - The OpenH264 library
# Once done this will define
#
#  OpenH264_ROOT - A list of search hints
#
#  OpenH264_FOUND - found OpenH264
#  OpenH264_INCLUDE_DIR - the OpenH264 include directory
#  OpenH264_LIBRARY_DIR - the OpenH264 library directory
#  A more complete find module cmake file could be found at: 
#  https://github.com/BelledonneCommunications/linphone-cmake-builder/blob/master/builders/openh264.cmake

# Dependencies
#libfind_package(OpenH264 NASM)

if (UNIX AND NOT ANDROID)
  find_package(PkgConfig QUIET)
  pkg_check_modules(PC_OpenH264 QUIET openh264)
endif (UNIX AND NOT ANDROID)

SET( OpenH264_PATH_HINTS 
    ${OpenH264_ROOT} 
    ${OpenH264_INCLUDE_DIR}
    ${OpenH264_LIBRARY_DIR}
    )

unset(OpenH264_INCLUDE_DIR CACHE)    
find_path(OpenH264_INCLUDE_DIR NAMES codec_api.h codec_app_def.h codec_def.h
  PATH_SUFFIXES include/wels codec/api/svc
  HINTS ${OpenH264_PATH_HINTS}
  )
unset(OpenH264_LIBRARY_DIR CACHE) 
find_path(OpenH264_LIBRARY_DIR NAMES openh264${CMAKE_STATIC_LIBRARY_SUFFIX} libopenh264.a  
  PATH_SUFFIXES lib
  HINTS ${OpenH264_PATH_HINTS}
  )  


include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenH264 DEFAULT_MSG OpenH264_INCLUDE_DIR OpenH264_LIBRARY_DIR)

mark_as_advanced(OpenH264_INCLUDE_DIR OpenH264_LIBRARY_DIR)
