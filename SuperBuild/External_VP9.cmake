cmake_minimum_required(VERSION 2.8.2)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
include(${OpenIGTLink_SOURCE_DIR}/SuperBuild/findVP9.cmake)

SET(VP9_DEPENDENCIES)
IF(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows") # window os build doesn't need the yasm
        INCLUDE(${OpenIGTLink_SOURCE_DIR}/SuperBuild/External_yasm.cmake)
	IF(NOT YASM_FOUND)
	  LIST(APPEND VP9_DEPENDENCIES YASM)
	  message("VP9 dependencies modified." VP9_DEPENDENCIES)
	ENDIF()
ENDIF()

IF(VP9_FOUND)
  IF(${OpenIGTLink_PROTOCOL_VERSION} LESS 3)
    MESSAGE(FATAL_ERROR "Video streaming requires a build of OpenIGTLink with v3 support enabled. Please set the OpenIGTLink_PROTOCOL_VERSION_3 to true.")
  ENDIF()
ELSE()
  # OpenIGTLink has not been built yet, so download and build it as an external project
  MESSAGE(STATUS "Downloading VP9 from https://github.com/webmproject/libvpx.git")              
	if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows") 
    SET (VP9_INCLUDE_DIR "${CMAKE_BINARY_DIR}/Deps/VP9/vpx" CACHE PATH "VP9 source directory" FORCE)
    SET (VP9_LIBRARY_DIR "${CMAKE_BINARY_DIR}/Deps/VP9" CACHE PATH "VP9 library directory" FORCE)   							
    ExternalProject_Add(VP9
      PREFIX "${CMAKE_BINARY_DIR}/Deps/VP9-prefix"
      GIT_REPOSITORY https://github.com/webmproject/libvpx/
      GIT_TAG v1.6.1
      SOURCE_DIR        "${CMAKE_BINARY_DIR}/Deps/VP9"
      CONFIGURE_COMMAND "${CMAKE_BINARY_DIR}/Deps/VP9/configure" --disable-examples --as=yasm --disable-tools --disable-docs --disable-vp8 --disable-libyuv --disable-unit_tests --disable-postproc WORKING_DIRECTORY "${VP9_LIBRARY_DIR}"
      BUILD_ALWAYS 1
      BUILD_COMMAND PATH=${YASM_BINARY_DIR}:$ENV{PATH}; make
      BUILD_IN_SOURCE 1
      INSTALL_COMMAND   ""
      TEST_COMMAND      ""
      DEPENDS ${VP9_DEPENDENCIES}
    )
  endif()
ENDIF()
