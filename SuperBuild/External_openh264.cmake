cmake_minimum_required(VERSION 2.8.2)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
IF((EXISTS ${H264_SOURCE_DIR}) AND (EXISTS ${H264_LIBRARY_DIR}))
  # H264 has been built already
  #FIND_PACKAGE(openh264)
  IF(${OpenIGTLink_PROTOCOL_VERSION} LESS 3 OR (NOT ${BUILD_VIDEOSTREAM}))
    MESSAGE(FATAL_ERROR "Video streaming requires a build of OpenIGTLink with v3 support enabled. Please set the OpenIGTLink_PROTOCOL_VERSION_3 to true and activate the BUILD_VIDEOSTREAM.")
  ENDIF()
ELSE()
  # OpenIGTLink has not been built yet, so download and build it as an external project
  MESSAGE(STATUS "Downloading openh264 from https://github.com/cisco/openh264.git.")              
  SET (H264_SOURCE_DIR "${CMAKE_BINARY_DIR}/Deps/openh264" CACHE PATH "H264 source directory" FORCE)
  SET (H264_LIBRARY_DIR "${CMAKE_BINARY_DIR}/Deps/openh264" CACHE PATH "H264 library directory" FORCE)
  ExternalProject_Add(openh264
    SOURCE_DIR "${H264_SOURCE_DIR}"
    BINARY_DIR "${H264_LIBRARY_DIR}"
    #--Download step--------------
    GIT_REPOSITORY https://github.com/cisco/openh264.git
    GIT_TAG master
    #--Configure step-------------
    CONFIGURE_COMMAND ""
    #--Build step-----------------
    BUILD_COMMAND "" 
    #--Install step-----------------
    INSTALL_COMMAND ""
    )  
ENDIF()
