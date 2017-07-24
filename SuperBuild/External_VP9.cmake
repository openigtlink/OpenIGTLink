cmake_minimum_required(VERSION 2.8.2)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
IF((EXISTS ${VP9_SOURCE_DIR}) AND (EXISTS ${VP9_LIBRARY_DIR}))
  IF(${OpenIGTLink_PROTOCOL_VERSION} LESS 3 OR (NOT ${BUILD_VIDEOSTREAM}))
    MESSAGE(FATAL_ERROR "Video streaming requires a build of OpenIGTLink with v3 support enabled. Please set the OpenIGTLink_PROTOCOL_VERSION_3 to true and activate the BUILD_VIDEOSTREAM.")
  ENDIF()
ELSE()
  # OpenIGTLink has not been built yet, so download and build it as an external project
  MESSAGE(STATUS "Downloading VP9 from https://github.com/webmproject/libvpx.git")              
	IF(NOT ${VP9_SOURCE_DIR} EQUAL "")
  	include_directories("${VP9_SOURCE_DIR}")
  ENDIF()
  SET (VP9_SOURCE_DIR "${CMAKE_BINARY_DIR}/Deps/VP9" CACHE PATH "VP9 source directory" FORCE)
  SET (VP9_LIBRARY_DIR "${CMAKE_BINARY_DIR}/Deps/VP9-bin" CACHE PATH "VP9 library directory"  FORCE)
  ExternalProject_Add(VP9
    SOURCE_DIR "${VP9_SOURCE_DIR}"
    BINARY_DIR "${VP9_LIBRARY_DIR}"
    #--Download step--------------
    GIT_REPOSITORY https://github.com/webmproject/libvpx.git
    GIT_TAG master
    #--Configure step-------------
    CONFIGURE_COMMAND ""
    #--Build step-----------------
    BUILD_COMMAND "" 
    #--Install step-----------------
    INSTALL_COMMAND ""
    )  
ENDIF()
