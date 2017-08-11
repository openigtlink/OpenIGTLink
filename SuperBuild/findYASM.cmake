# - The YASM library
# Once done this will define
#
#  YASM_ROOT - A list of search hints
#
#  YASM_FOUND - found YASM
#  YASM_BINARY_DIR - YASM library directory

#if (UNIX AND NOT ANDROID)
#  find_package(PkgConfig QUIET)
#  pkg_check_modules(PC_YASM QUIET YASM)
#endif()

SET( YASM_PATH_HINTS 
		${YASM_ROOT} 
    ${CMAKE_BINARY_DIR}/Deps/YASM
    ${CMAKE_BINARY_DIR}/Deps/YASM-bin
    )
	
set(YASM_BINARY_DIR "")
find_path(YASM_BINARY_DIR
	 NAMES yasm
	 HINTS ${YASM_PATH_HINTS}
	 )
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(YASM REQUIRED_VARS YASM_BINARY_DIR)

mark_as_advanced(YASM_BINARY_DIR)