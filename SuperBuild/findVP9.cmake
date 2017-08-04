# - The VP9 library
# Once done this will define
#
#  VP9_ROOT - A list of search hints
#
#  VP9_FOUND - found VP9
#  VP9_INCLUDE_DIR - the VP9 include directory
#  VP9_LIBRARY_DIR - VP9 library directory

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
	 NAMES vpxmdd.lib vpxmd.lib libvpx.a
	 PATH_SUFFIXES $(Platform)/${CMAKE_BUILD_TYPE}
	 HINTS ${VP9_PATH_HINTS}
	 )
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(VP9 REQUIRED_VARS VP9_LIBRARY_DIR VP9_INCLUDE_DIR)

mark_as_advanced(VP9_INCLUDE_DIR VP9_LIBRARY_DIR)
