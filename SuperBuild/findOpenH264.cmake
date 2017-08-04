# - The OpenH264 library
# Once done this will define
#
#  OpenH264_ROOT - A list of search hints
#
#  OpenH264_FOUND - found OpenH264
#  OpenH264_INCLUDE_DIR - the OpenH264 include directory
#  OpenH264_LIBRARIES - openh264 library
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
    ${CMAKE_BINARY_DIR}/Deps/openh264-bin
    )

set(OpenH264_DIR "")    
find_path(OpenH264_DIR NAMES codec_api.h codec_app_def.h codec_def.h openh264${CMAKE_STATIC_LIBRARY_SUFFIX} libopenh264.a
	PATH_SUFFIXES include/wels lib
	HINTS ${OpenH264_PATH_HINTS}
	)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenH264 DEFAULT_MSG OpenH264_DIR)

mark_as_advanced(OpenH264_DIR)
