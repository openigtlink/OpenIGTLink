cmake_minimum_required(VERSION 2.8.2)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
include(${OpenIGTLink_SOURCE_DIR}/SuperBuild/FindOpenH264.cmake)
IF(OpenH264_FOUND)
  # H264 has been built already
  IF(${OpenIGTLink_PROTOCOL_VERSION} LESS 3)
    MESSAGE(FATAL_ERROR "Video streaming requires a build of OpenIGTLink with v3 support enabled. Please set the OpenIGTLink_PROTOCOL_VERSION_3 to true.")
  ENDIF()
ELSE()
  # OpenIGTLink has not been built yet, so download and build it as an external project
  MESSAGE(STATUS "Downloading openh264 from https://github.com/cisco/openh264.git.")  
  SET (OpenH264_INCLUDE_DIR "${CMAKE_BINARY_DIR}/Deps/openh264/codec/api/wels" CACHE PATH "H264 source directory" FORCE)
  SET (OpenH264_LIBRARY_DIR "${CMAKE_BINARY_DIR}/Deps/openh264" CACHE PATH "H264 source directory" FORCE)
  if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")     
    ExternalProject_Add(OpenH264
      PREFIX "${CMAKE_BINARY_DIR}/Deps/openh264-prefix"
      SOURCE_DIR "${CMAKE_BINARY_DIR}/Deps/openh264"
      GIT_REPOSITORY https://github.com/cisco/openh264.git
      GIT_TAG master
      CONFIGURE_COMMAND ""
      BUILD_ALWAYS 1
      BUILD_COMMAND     make
      BUILD_IN_SOURCE 1
      INSTALL_COMMAND   ""
      TEST_COMMAND      ""
    )  
  else()
    # ToDo: if it is a window os platform, make the build successful
    ExternalProject_Add(OpenH264
      PREFIX "${CMAKE_BINARY_DIR}/Deps/openh264-prefix"
      SOURCE_DIR "${CMAKE_BINARY_DIR}/Deps/openh264"
      GIT_REPOSITORY https://github.com/cisco/openh264.git
      GIT_TAG master
      CONFIGURE_COMMAND ""
      BUILD_ALWAYS 1
      BUILD_COMMAND     ""
      INSTALL_COMMAND   ""
      TEST_COMMAND      ""
    )     
  endif()                               
ENDIF()
