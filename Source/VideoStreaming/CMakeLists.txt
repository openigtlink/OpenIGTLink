#-----------------------------------------------------------------------------
# OpenH264
IF(LINK_H264)
  configure_file(${OpenIGTLink_SOURCE_DIR}/SuperBuild/CMakeListsOpenH264.txt.in
  ${PROJECT_BINARY_DIR}/OpenH264-download/CMakeLists.txt)
	#Here the downloading project is triggered                                                               
	execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" . 
									WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/OpenH264-download" )
	execute_process(COMMAND "${CMAKE_COMMAND}" --build . 
									WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/OpenH264-download" )                   
	if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")             
					execute_process(COMMAND "make" WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/OpenH264" )                     
	endif(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows") 
ENDIF()

# VP9 codec
IF(LINK_VP9)
  configure_file(${OpenIGTLink_SOURCE_DIR}/SuperBuild/CMakeListsVPX.txt.in
  ${PROJECT_BINARY_DIR}/OpenVPX-download/CMakeLists.txt)
  #Here the downloading project is triggered
  execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
                  WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/OpenVPX-download" )
  execute_process(COMMAND "${CMAKE_COMMAND}" --build .
              WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/OpenVPX-download" )
  if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    #execute_process(COMMAND "cd" "${PROJECT_BINARY_DIR}/OpenVPX/configure")
    execute_process(COMMAND "${PROJECT_BINARY_DIR}/OpenVPX/configure" --disable-examples --disable-tools --disable-docs --disable-vp8 --disable-libyuv --disable-unit_tests --disable-postproc WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/OpenVPX" )
    execute_process(COMMAND "make" WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/OpenVPX" )
  endif(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
ENDIF()


# VPX codec
IF(LINK_X265)
  set(X265_SOURCE_DIR "" CACHE STRING "X265 source directory" PARENT_SCOPE)
  set(X265_LIBRARY_DIR "" CACHE STRING "X265 library directory" PARENT_SCOPE)
  include_directories("${X265_SOURCE_DIR}")
  include_directories("${X265_LIBRARY_DIR}")
  LINK_DIRECTORIES("${X265_LIBRARY_DIR}")
ENDIF()

IF(LINK_OPENHEVC)
  set(OPENHEVC_LIBRARY_DIR "" CACHE STRING "HEVC decoder library directory" PARENT_SCOPE)
  set(OPENHEVC_SOURCE_DIR "" CACHE STRING "HEVC decoder source directory" PARENT_SCOPE)
  include_directories("${OPENHEVC_SOURCE_DIR}")
  include_directories("${OPENHEVC_LIBRARY_DIR}")
  LINK_DIRECTORIES("${OPENHEVC_LIBRARY_DIR}")
ENDIF()

IF(OpenIGTLink_PLATFORM_WIN32) # for Windows
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_H264)
		SET(TEMP_LINK_LIBS
    ${LINK_LIBS}
    "${PROJECT_BINARY_DIR}/OpenH264/openh264.lib"
    )
    SET(LINK_LIBS ${TEMP_LINK_LIBS})
	  SET(LINK_LIBS ${TEMP_LINK_LIBS} PARENT_SCOPE)    ## Set local and the parent scope. Use Cache internal might be a better solution 
  ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_VP9)
    SET(LINK_VP9_LIBRARY optimized ${PROJECT_BINARY_DIR}\\OpenVPX\\Win32\\Release\\vpxmt.lib debug ${PROJECT_BINARY_DIR}\\OpenVPX\\Win32\\Debug\\vpxmtd.lib)
    SET(TEMP_LINK_LIBS
      ${LINK_LIBS}
      ${LINK_VP9_LIBRARY}
    )
    SET(LINK_LIBS ${TEMP_LINK_LIBS})
	  SET(LINK_LIBS ${TEMP_LINK_LIBS} PARENT_SCOPE)   
  ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_X265)
    SET(LINK_X265_LIBRARY optimized ${X265_LIBRARY_DIR}\\Release\\x265-static.lib debug ${X265_LIBRARY_DIR}\\Debug\\x265-static.lib)
    SET(TEMP_LINK_LIBS
      ${LINK_LIBS}
      ${LINK_X265_LIBRARY}
    )
    SET(LINK_LIBS ${TEMP_LINK_LIBS})
	  SET(LINK_LIBS ${TEMP_LINK_LIBS} PARENT_SCOPE) 
  ENDIF()
	IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_OPENHEVC)
    SET(LINK_OPENHEVC_LIBRARY ${OPENHEVC_LIBRARY_DIR}\\libLibOpenHevcWrapper.dll)
		SET(TEMP_LINK_LIBS
      ${LINK_LIBS}
      ${LINK_OPENHEVC_LIBRARY}
    )
    SET(LINK_LIBS ${TEMP_LINK_LIBS})
	  SET(LINK_LIBS ${TEMP_LINK_LIBS} PARENT_SCOPE)
  ENDIF()  
ELSE() # for POSIX-compatible OSs
	IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_H264)
		SET(TEMP_LINK_LIBS
		  ${LINK_LIBS}
		  "${PROJECT_BINARY_DIR}/OpenH264/libopenh264.a"
		)
		SET(LINK_LIBS ${TEMP_LINK_LIBS})
	  SET(LINK_LIBS ${TEMP_LINK_LIBS} PARENT_SCOPE)
	ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_VP9)
    SET(TEMP_LINK_LIBS
      ${LINK_LIBS}
      "${PROJECT_BINARY_DIR}/OpenVPX/libvpx.a"
    )
    SET(LINK_LIBS ${TEMP_LINK_LIBS})
	  SET(LINK_LIBS ${TEMP_LINK_LIBS} PARENT_SCOPE)
  ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_X265)
    SET(LINK_X265_LIBRARY optimized ${X265_LIBRARY_DIR}/libx265.a debug ${X265_LIBRARY_DIR}/libx265.a)
    SET(TEMP_LINK_LIBS
      ${LINK_LIBS}
      ${LINK_X265_LIBRARY}
    )
    SET(LINK_LIBS ${TEMP_LINK_LIBS})
	  SET(LINK_LIBS ${TEMP_LINK_LIBS} PARENT_SCOPE)
  ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_OPENHEVC)
    SET(LINK_OPENHEVC_LIBRARY ${OPENHEVC_LIBRARY_DIR}/libLibOpenHevcWrapper.a)
    SET(TEMP_LINK_LIBS
      ${LINK_LIBS}
      ${LINK_OPENHEVC_LIBRARY}
    )
    SET(LINK_LIBS ${TEMP_LINK_LIBS})
	  SET(LINK_LIBS ${TEMP_LINK_LIBS} PARENT_SCOPE)
  ENDIF()	
ENDIF()

  
IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_H264)
	SET(TEMP_OpenIGTLink_INCLUDE_DIRS
	${OpenIGTLink_INCLUDE_DIRS}
  "${PROJECT_BINARY_DIR}/OpenH264/codec"
  "${PROJECT_BINARY_DIR}/OpenH264/codec/common/inc"
  "${PROJECT_BINARY_DIR}/OpenH264/codec/console/common/src"
	"${PROJECT_BINARY_DIR}/OpenH264/codec/console/common/inc"
	"${PROJECT_BINARY_DIR}/OpenH264/codec/console/dec/inc"
	"${PROJECT_BINARY_DIR}/OpenH264/codec/console/enc/src"
	"${PROJECT_BINARY_DIR}/OpenH264/codec/api/svc"
	"${PROJECT_BINARY_DIR}/OpenH264/codec/encoder/core/inc"
	"${PROJECT_BINARY_DIR}/OpenH264/codec/processing/interface"
	"${PROJECT_BINARY_DIR}/OpenH264/test"
	)
	SET(OpenIGTLink_INCLUDE_DIRS ${TEMP_OpenIGTLink_INCLUDE_DIRS})
	SET(OpenIGTLink_INCLUDE_DIRS ${TEMP_OpenIGTLink_INCLUDE_DIRS} PARENT_SCOPE)
ENDIF()

IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_VP9)
  SET(TEMP_OpenIGTLink_INCLUDE_DIRS
    ${OpenIGTLink_INCLUDE_DIRS}
    "${PROJECT_BINARY_DIR}/OpenVPX"
    "${PROJECT_BINARY_DIR}/OpenVPX/vpx"
    "${PROJECT_BINARY_DIR}/OpenVPX/vp9"
    "${PROJECT_BINARY_DIR}/OpenVPX/vp9/common"
  )
  SET(OpenIGTLink_INCLUDE_DIRS ${TEMP_OpenIGTLink_INCLUDE_DIRS})
	SET(OpenIGTLink_INCLUDE_DIRS ${TEMP_OpenIGTLink_INCLUDE_DIRS} PARENT_SCOPE)
ENDIF()

IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_X265)
  SET(TEMP_OpenIGTLink_INCLUDE_DIRS
    ${OpenIGTLink_INCLUDE_DIRS}
    "${X265_SOURCE_DIR}"
    "${X265_LIBRARY_DIR}"
  )
  SET(OpenIGTLink_INCLUDE_DIRS ${TEMP_OpenIGTLink_INCLUDE_DIRS})
	SET(OpenIGTLink_INCLUDE_DIRS ${TEMP_OpenIGTLink_INCLUDE_DIRS} PARENT_SCOPE)
ENDIF()

IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_OPENHEVC)
  SET(TEMP_OpenIGTLink_INCLUDE_DIRS
    ${OpenIGTLink_INCLUDE_DIRS}
    "${OPENHEVC_SOURCE_DIR}"
	  "${OPENHEVC_SOURCE_DIR}/platform/arm" # To be removed
    "${OPENHEVC_LIBRARY_DIR}"
  )
  SET(OpenIGTLink_INCLUDE_DIRS ${TEMP_OpenIGTLink_INCLUDE_DIRS})
	SET(OpenIGTLink_INCLUDE_DIRS ${TEMP_OpenIGTLink_INCLUDE_DIRS} PARENT_SCOPE)
ENDIF()

# Add support for OpenIGTLink version 3
IF( ${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" AND (LINK_H264 OR LINK_VP9 OR LINK_X265 OR LINK_OPENHEVC))
  SET(OpenIGTLink_INCLUDE_DIRS ${OpenIGTLink_INCLUDE_DIRS} "${PROJECT_SOURCE_DIR}/Source/VideoStreaming")
	SET(OpenIGTLink_INCLUDE_DIRS ${OpenIGTLink_INCLUDE_DIRS} "${PROJECT_SOURCE_DIR}/Source/VideoStreaming" PARENT_SCOPE)
  SET(TEMP_OpenIGTLink_SOURCES
    ${OpenIGTLink_SOURCES} 
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtl_video.c
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlVideoMessage.cxx
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VideoStreamIGTLinkServer.cxx
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VideoStreamIGTLinkReceiver.cxx
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlCodecCommonClasses.cxx
    )
  SET(OpenIGTLink_SOURCES ${TEMP_OpenIGTLink_SOURCES})
	SET(OpenIGTLink_SOURCES ${TEMP_OpenIGTLink_SOURCES} PARENT_SCOPE) 
	SET(TEMP_OpenIGTLink_INCLUDE_DIRS
    ${OpenIGTLink_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming
  )
	SET(OpenIGTLink_INCLUDE_DIRS ${TEMP_OpenIGTLink_INCLUDE_DIRS})
	SET(OpenIGTLink_INCLUDE_DIRS ${TEMP_OpenIGTLink_INCLUDE_DIRS} PARENT_SCOPE) 
  IF( MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode" )
    SET(TEMP_OpenIGTLink_INCLUDE_FILES
      ${OpenIGTLink_INCLUDE_FILES} 
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtl_video.h
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlVideoMessage.h
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VideoStreamIGTLinkServer.h
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VideoStreamIGTLinkReceiver.h
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlCodecCommonClasses.h
      )
    SET(OpenIGTLink_INCLUDE_FILES ${TEMP_OpenIGTLink_INCLUDE_FILES})
	  SET(OpenIGTLink_INCLUDE_FILES ${TEMP_OpenIGTLink_INCLUDE_FILES} PARENT_SCOPE)   
  ENDIF()
  IF(LINK_H264)
    SET(TEMP_OpenIGTLink_SOURCES
      ${OpenIGTLink_SOURCES}
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H264Decoder.cxx
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H264Encoder.cxx
      ${PROJECT_BINARY_DIR}/OpenH264/test/api/sha1.c
      ${PROJECT_BINARY_DIR}/OpenH264/codec/console/common/src/read_config.cpp
    )
    SET(OpenIGTLink_SOURCES ${TEMP_OpenIGTLink_SOURCES})
	  SET(OpenIGTLink_SOURCES ${TEMP_OpenIGTLink_SOURCES} PARENT_SCOPE)  
    IF( MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode" )
      SET(TEMP_OpenIGTLink_INCLUDE_FILES
      	${OpenIGTLink_INCLUDE_FILES} 
        ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H264Decoder.h
        ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H264Encoder.h
        ${PROJECT_BINARY_DIR}/OpenH264/test/sha1.h
        ${PROJECT_BINARY_DIR}/OpenH264/codec/console/common/inc/read_config.h
      )
      SET(OpenIGTLink_INCLUDE_FILES ${TEMP_OpenIGTLink_INCLUDE_FILES})
	    SET(OpenIGTLink_INCLUDE_FILES ${TEMP_OpenIGTLink_INCLUDE_FILES} PARENT_SCOPE) 
    ENDIF()
  ENDIF()
  IF(LINK_VP9)
    SET(TEMP_OpenIGTLink_SOURCES
    	${OpenIGTLink_SOURCES}
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VP9Decoder.cxx
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VP9Encoder.cxx
      ${PROJECT_BINARY_DIR}/OpenVPX/tools_common.c
      ${PROJECT_BINARY_DIR}/OpenVPX/video_reader.c
      ${PROJECT_BINARY_DIR}/OpenVPX/ivfdec.c
      ${PROJECT_BINARY_DIR}/OpenVPX/vpx_config.c
    )
    SET(OpenIGTLink_SOURCES ${TEMP_OpenIGTLink_SOURCES})
	  SET(OpenIGTLink_SOURCES ${TEMP_OpenIGTLink_SOURCES} PARENT_SCOPE) 
    IF( MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode" )
      SET(TEMP_OpenIGTLink_INCLUDE_FILES
      	${OpenIGTLink_INCLUDE_FILES}
        ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VP9Decoder.h
        ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VP9Encoder.h
        ${PROJECT_BINARY_DIR}/OpenVPX/vpx_config.h
        ${PROJECT_BINARY_DIR}/OpenVPX/tools_common.h
        ${PROJECT_BINARY_DIR}/OpenVPX/video_reader.h
        ${PROJECT_BINARY_DIR}/OpenVPX/ivfdec.h
      )
      SET(OpenIGTLink_INCLUDE_FILES ${TEMP_OpenIGTLink_INCLUDE_FILES})
	    SET(OpenIGTLink_INCLUDE_FILES ${TEMP_OpenIGTLink_INCLUDE_FILES} PARENT_SCOPE) 
    ENDIF()
  ENDIF()
  IF(LINK_X265)
    SET(TEMP_OpenIGTLink_SOURCES
      ${OpenIGTLink_SOURCES}
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H265Encoder.cxx
    )
    SET(OpenIGTLink_SOURCES ${TEMP_OpenIGTLink_SOURCES})
	  SET(OpenIGTLink_SOURCES ${TEMP_OpenIGTLink_SOURCES} PARENT_SCOPE) 
    IF( MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode" )
      SET(TEMP_OpenIGTLink_INCLUDE_FILES
        ${OpenIGTLink_INCLUDE_FILES}
        ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H265Encoder.h
        )
      SET(OpenIGTLink_INCLUDE_FILES ${TEMP_OpenIGTLink_INCLUDE_FILES})
	    SET(OpenIGTLink_INCLUDE_FILES ${TEMP_OpenIGTLink_INCLUDE_FILES} PARENT_SCOPE) 
    ENDIF()
  ENDIF()
  IF(LINK_OPENHEVC)
    SET(TEMP_OpenIGTLink_SOURCES
      ${OpenIGTLink_SOURCES}
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H265Decoder.cxx
    )
    SET(OpenIGTLink_SOURCES ${TEMP_OpenIGTLink_SOURCES})
	  SET(OpenIGTLink_SOURCES ${TEMP_OpenIGTLink_SOURCES} PARENT_SCOPE) 
    IF( MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode" )
      SET(TEMP_OpenIGTLink_INCLUDE_FILES
        ${OpenIGTLink_INCLUDE_FILES}
        ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H265Decoder.h
        )
      SET(OpenIGTLink_INCLUDE_FILES ${TEMP_OpenIGTLink_INCLUDE_FILES})
	    SET(OpenIGTLink_INCLUDE_FILES ${TEMP_OpenIGTLink_INCLUDE_FILES} PARENT_SCOPE) 
    ENDIF()
  ENDIF()
ENDIF()
