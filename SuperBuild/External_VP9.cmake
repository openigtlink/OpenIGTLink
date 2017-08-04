cmake_minimum_required(VERSION 2.8.2)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
include(${OpenIGTLink_SOURCE_DIR}/SuperBuild/findVP9.cmake)
IF(VP9_FOUND)
  IF(${OpenIGTLink_PROTOCOL_VERSION} LESS 3 OR (NOT ${BUILD_VIDEOSTREAM}))
    MESSAGE(FATAL_ERROR "Video streaming requires a build of OpenIGTLink with v3 support enabled. Please set the OpenIGTLink_PROTOCOL_VERSION_3 to true and activate the BUILD_VIDEOSTREAM.")
  ENDIF()
ELSE()
  # OpenIGTLink has not been built yet, so download and build it as an external project
  MESSAGE(STATUS "Downloading VP9 from https://github.com/webmproject/libvpx.git")              
	SET (VP9_SOURCE_DIR "${CMAKE_BINARY_DIR}/Deps/VP9" CACHE PATH "VP9 source directory" FORCE)
  SET (VP9_LIBRARY_DIR "${CMAKE_BINARY_DIR}/Deps/VP9" CACHE PATH "VP9 library directory" FORCE)   							
	if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows") 
    ExternalProject_Add(VP9
    	PREFIX "${CMAKE_BINARY_DIR}/Deps/VP9-prefix"
      GIT_REPOSITORY https://github.com/webmproject/libvpx/
      GIT_TAG master
      SOURCE_DIR        "${VP9_SOURCE_DIR}"
      CONFIGURE_COMMAND "${VP9_SOURCE_DIR}/configure" --disable-examples --disable-tools --disable-docs --disable-vp8 --disable-libyuv --disable-unit_tests --disable-postproc WORKING_DIRECTORY "${VP9_LIBRARY_DIR}"
      BUILD_ALWAYS 1
      BUILD_COMMAND make
      BUILD_IN_SOURCE 1
      INSTALL_COMMAND   ""
      TEST_COMMAND      ""
      DEPENDS YASM
    )
  else()
    # ToDo: if it is a window os platform, make the build successful
    ExternalProject_Add(VP9
    	PREFIX "${CMAKE_BINARY_DIR}/Deps/VP9-prefix"
      GIT_REPOSITORY https://github.com/webmproject/libvpx/
      GIT_TAG master
      SOURCE_DIR        "${VP9_SOURCE_DIR}"
      BINARY_DIR        "${VP9_LIBRARY_DIR}"
      CONFIGURE_COMMAND ""
      BUILD_ALWAYS 1
      BUILD_COMMAND     ""
      INSTALL_COMMAND   ""
      TEST_COMMAND      ""
      DEPENDS YASM
    )
  endif()
ENDIF()
