cmake_minimum_required(VERSION 2.8.2)

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
include(${OpenIGTLink_SOURCE_DIR}/SuperBuild/FindVP9.cmake)

SET(VP9_DEPENDENCIES)

IF(VP9_FOUND)
  IF(${OpenIGTLink_PROTOCOL_VERSION} LESS 3)
    MESSAGE(FATAL_ERROR "Video streaming requires a build of OpenIGTLink with v3 support enabled. Please set the OpenIGTLink_PROTOCOL_VERSION_3 to true.")
  ENDIF()
ELSE()
  # OpenIGTLink has not been built yet, so download and build it as an external project
  MESSAGE(STATUS "Downloading VP9 from https://github.com/webmproject/libvpx.git")              
  if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows") 
    INCLUDE(${OpenIGTLink_SOURCE_DIR}/SuperBuild/External_yasm.cmake)
    IF(NOT YASM_FOUND)
      LIST(APPEND VP9_DEPENDENCIES YASM)
      message("VP9 dependencies modified." VP9_DEPENDENCIES)
    ENDIF()
    SET (VP9_INCLUDE_DIR "${CMAKE_BINARY_DIR}/Deps/VP9/vpx" CACHE PATH "VP9 source directory" FORCE)
    SET (VP9_LIBRARY_DIR "${CMAKE_BINARY_DIR}/Deps/VP9" CACHE PATH "VP9 library directory" FORCE)                 
    ExternalProject_Add(VP9
      PREFIX "${CMAKE_BINARY_DIR}/Deps/VP9-prefix"
      GIT_REPOSITORY https://github.com/webmproject/libvpx/
      GIT_TAG v1.6.1
      SOURCE_DIR        "${CMAKE_BINARY_DIR}/Deps/VP9"
      CONFIGURE_COMMAND "${CMAKE_BINARY_DIR}/Deps/VP9/configure" --disable-examples --as=yasm --enable-pic --disable-tools --disable-docs --disable-vp8 --disable-libyuv --disable-unit_tests --disable-postproc WORKING_DIRECTORY "${VP9_LIBRARY_DIR}"
      BUILD_ALWAYS 1
      BUILD_COMMAND PATH=${YASM_BINARY_DIR}:$ENV{PATH}; make
      BUILD_IN_SOURCE 1
      INSTALL_COMMAND   ""
      TEST_COMMAND      ""
      DEPENDS ${VP9_DEPENDENCIES}
    )
  else()
    if( ("${CMAKE_GENERATOR}" MATCHES "Visual Studio 14 2015") OR ("${CMAKE_GENERATOR}" MATCHES "Visual Studio 15 2017" ) OR 
        ("${CMAKE_GENERATOR}" MATCHES "Visual Studio 12 2013") OR ("${CMAKE_GENERATOR}" MATCHES "Visual Studio 16 2019" )
          )
      SET (VP9_INCLUDE_DIR "${CMAKE_BINARY_DIR}/Deps/VP9-Binary/include/vpx" CACHE PATH "VP9 source directory" FORCE)
      SET (BinaryURL "")
      if (${CMAKE_VS_PLATFORM_NAME} MATCHES "(Win64|x64|IA64)") 
        SET (VP9_LIBRARY_DIR "${CMAKE_BINARY_DIR}/Deps/VP9-Binary/lib/x64" CACHE PATH "VP9 library directory" FORCE)
      else()
        SET (VP9_LIBRARY_DIR "${CMAKE_BINARY_DIR}/Deps/VP9-Binary/lib/x86" CACHE PATH "VP9 library directory" FORCE)
      endif()
      if ("${CMAKE_GENERATOR}" MATCHES "Visual Studio 12 2013")
        SET (BinaryURL "https://github.com/ShiftMediaProject/libvpx/releases/download/v1.8.1-1/libvpx_v1.8.1-1_msvc12.zip")
      elseif("${CMAKE_GENERATOR}" MATCHES "Visual Studio 14 2015" )  
        SET (BinaryURL "https://github.com/ShiftMediaProject/libvpx/releases/download/v1.8.1-1/libvpx_v1.8.1-1_msvc14.zip")
      elseif("${CMAKE_GENERATOR}" MATCHES "Visual Studio 15 2017" )  
        SET (BinaryURL "https://github.com/ShiftMediaProject/libvpx/releases/download/v1.8.1-1/libvpx_v1.8.1-1_msvc15.zip")
      elseif("${CMAKE_GENERATOR}" MATCHES "Visual Studio 16 2019" )  
        SET (BinaryURL "https://github.com/ShiftMediaProject/libvpx/releases/download/v1.8.1-1/libvpx_v1.8.1-1_msvc16.zip")
        
      endif()  
      ExternalProject_Add(VP9
        URL               ${BinaryURL}
        SOURCE_DIR        "${CMAKE_BINARY_DIR}/Deps/VP9-Binary"
        CONFIGURE_COMMAND ""
        BUILD_ALWAYS 0
        BUILD_COMMAND ""
        INSTALL_COMMAND   ""
        TEST_COMMAND      ""
      )
    else()
      SET(OpenIGTLink_USE_VP9 OFF CACHE BOOL "" FORCE)
      IF (OpenIGTLink_USE_H264 OR OpenIGTLink_USE_VP9 OR OpenIGTLink_USE_X265 OR OpenIGTLink_USE_OpenHEVC OR OpenIGTLink_USE_AV1)
        SET(OpenIGTLink_ENABLE_VIDEOSTREAMING  OFF)
      ENDIF()
      message(WARNING "Only support for Visual Studio 14 2015")
    endif()
  endif()
ENDIF()
