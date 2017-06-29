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
  set(H264_SOURCE_DIR "" CACHE PATH "H264 source directory")
  set(H264_LIBRARY_DIR "" CACHE PATH "H264 library directory")                    
	if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")             
					execute_process(COMMAND "make" WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/OpenH264" )             
        set(H264_SOURCE_DIR "${PROJECT_BINARY_DIR}/OpenH264" CACHE PATH "H264 source directory" FORCE)
        set(H264_LIBRARY_DIR "${PROJECT_BINARY_DIR}/OpenH264" CACHE PATH "H264 source directory" FORCE)          
	endif(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows") 
	IF(NOT ${H264_SOURCE_DIR} EQUAL "")
  	include_directories("${H264_SOURCE_DIR}")
  ENDIF()
  IF(NOT ${H264_LIBRARY_DIR} EQUAL "")	
    include_directories("${H264_LIBRARY_DIR}")
    LINK_DIRECTORIES("${H264_LIBRARY_DIR}")
  ENDIF()
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
  set(VP9_SOURCE_DIR "" CACHE PATH "VP9 source directory")
  set(VP9_LIBRARY_DIR "" CACHE PATH "VP9 library directory")            
  if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    #execute_process(COMMAND "cd" "${PROJECT_BINARY_DIR}/OpenVPX/configure")
    execute_process(COMMAND "${PROJECT_BINARY_DIR}/OpenVPX/configure" --disable-examples --disable-tools --disable-docs --disable-vp8 --disable-libyuv --disable-unit_tests --disable-postproc WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/OpenVPX" )
    execute_process(COMMAND "make" WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/OpenVPX" )
    set(VP9_SOURCE_DIR "${PROJECT_BINARY_DIR}/OpenVPX" CACHE PATH "VP9 source directory" FORCE)
    set(VP9_LIBRARY_DIR "${PROJECT_BINARY_DIR}/OpenVPX" CACHE PATH "VP9 library directory" FORCE) 
  endif(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
  IF(NOT ${VP9_SOURCE_DIR} EQUAL "")
  	include_directories("${VP9_SOURCE_DIR}")
  ENDIF()
  IF(NOT ${VP9_LIBRARY_DIR} EQUAL "")	
    include_directories("${VP9_LIBRARY_DIR}")
    LINK_DIRECTORIES("${VP9_LIBRARY_DIR}")
  ENDIF()
ENDIF()


# VPX codec
IF(LINK_X265)
  set(X265_SOURCE_DIR "" CACHE PATH "X265 source directory")
  set(X265_LIBRARY_DIR "" CACHE PATH "X265 library directory")
  IF(NOT ${X265_SOURCE_DIR} EQUAL "")
  	include_directories("${X265_SOURCE_DIR}")
  ENDIF()
  IF(NOT ${X265_LIBRARY_DIR} EQUAL "")	
    include_directories("${X265_LIBRARY_DIR}")
    LINK_DIRECTORIES("${X265_LIBRARY_DIR}")
  ENDIF()
ENDIF()

IF(LINK_OPENHEVC)
  set(OPENHEVC_LIBRARY_DIR "" CACHE PATH "HEVC decoder library directory")
  set(OPENHEVC_SOURCE_DIR "" CACHE PATH "HEVC decoder source directory")
  IF(NOT ${OPENHEVC_SOURCE_DIR} EQUAL "")
  	include_directories("${OPENHEVC_SOURCE_DIR}")
  ENDIF()
  IF(NOT ${OPENHEVC_LIBRARY_DIR} EQUAL "")	
    include_directories("${OPENHEVC_LIBRARY_DIR}")
    LINK_DIRECTORIES("${OPENHEVC_LIBRARY_DIR}")
  ENDIF()
ENDIF()

IF(OpenIGTLink_PLATFORM_WIN32) # for Windows
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_H264)
		SET(LINK_LIBS
    ${LINK_LIBS}
    ${H264_LIBRARY_DIR}/openh264.lib
    )
	  #SET(LINK_LIBS ${TEMP_LINK_LIBS} PARENT_SCOPE)    ## Set local and the parent scope. Use Cache internal might be a better solution 
  ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_VP9)
    #To do, library name depends on the compiler setting, could be vpxmt.lib and vpxmtd also. Make sure the setting matches.
    SET(LINK_VP9_LIBRARY optimized ${VP9_LIBRARY_DIR}\\Win32\\Release\\vpxmd.lib debug ${VP9_LIBRARY_DIR}\\Win32\\Debug\\vpxmdd.lib)
    SET(LINK_LIBS
      ${LINK_LIBS}
      ${LINK_VP9_LIBRARY}
    )
  ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_X265)
    SET(LINK_X265_LIBRARY optimized ${X265_LIBRARY_DIR}\\Release\\x265-static.lib debug ${X265_LIBRARY_DIR}\\Debug\\x265-static.lib)
    SET(LINK_LIBS
      ${LINK_LIBS}
      ${LINK_X265_LIBRARY}
    )
  ENDIF()
	IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_OPENHEVC)
    SET(LINK_OPENHEVC_LIBRARY ${OPENHEVC_LIBRARY_DIR}\\libLibOpenHevcWrapper.dll)
		SET(LINK_LIBS
      ${LINK_LIBS}
      ${LINK_OPENHEVC_LIBRARY}
    )
  ENDIF()  
ELSE() # for POSIX-compatible OSs
	IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_H264)
		SET(LINK_LIBS
		  ${LINK_LIBS}
		  ${H264_LIBRARY_DIR}/libopenh264.a
		)
	ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_VP9)
    SET(LINK_LIBS
      ${LINK_LIBS}
      ${VP9_LIBRARY_DIR}/libvpx.a
    )
  ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_X265)
    SET(LINK_X265_LIBRARY optimized ${X265_LIBRARY_DIR}/libx265.a debug ${X265_LIBRARY_DIR}/libx265.a)
    SET(LINK_LIBS
      ${LINK_LIBS}
      ${LINK_X265_LIBRARY}
    )
  ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_OPENHEVC)
    SET(LINK_OPENHEVC_LIBRARY ${OPENHEVC_LIBRARY_DIR}/libLibOpenHevcWrapper.a)
    SET(LINK_LIBS
      ${LINK_LIBS}
      ${LINK_OPENHEVC_LIBRARY}
    )
  ENDIF()	
ENDIF()

  
IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_H264)
	SET(OpenIGTLink_INCLUDE_DIRS
	${OpenIGTLink_INCLUDE_DIRS}
  ${H264_SOURCE_DIR}/codec
  ${H264_SOURCE_DIR}/codec/common/inc
  ${H264_SOURCE_DIR}/codec/console/common/src
	${H264_SOURCE_DIR}/codec/console/common/inc
	${H264_SOURCE_DIR}/codec/console/dec/inc
	${H264_SOURCE_DIR}/codec/console/enc/src
	${H264_SOURCE_DIR}/codec/api/svc
	${H264_SOURCE_DIR}/codec/encoder/core/inc
	${H264_SOURCE_DIR}/codec/processing/interface
	${H264_SOURCE_DIR}/test
	)
ENDIF()

IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_VP9)
  SET(OpenIGTLink_INCLUDE_DIRS
    ${OpenIGTLink_INCLUDE_DIRS}
    "${VP9_SOURCE_DIR}"
    "${VP9_SOURCE_DIR}/vpx"
    "${VP9_SOURCE_DIR}/vp9"
    "${VP9_SOURCE_DIR}/vp9/common"
  )
ENDIF()

IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_X265)
  SET(OpenIGTLink_INCLUDE_DIRS
    ${OpenIGTLink_INCLUDE_DIRS}
    "${X265_SOURCE_DIR}"
    "${X265_LIBRARY_DIR}"
  )
ENDIF()

IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_OPENHEVC)
  SET(OpenIGTLink_INCLUDE_DIRS
    ${OpenIGTLink_INCLUDE_DIRS}
    "${OPENHEVC_SOURCE_DIR}"
	  "${OPENHEVC_SOURCE_DIR}/platform/arm" # To be removed
    "${OPENHEVC_LIBRARY_DIR}"
  )
ENDIF()

# Add support for OpenIGTLink version 3
IF( ${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" AND (LINK_H264 OR LINK_VP9 OR LINK_X265 OR LINK_OPENHEVC))
  SET(OpenIGTLink_INCLUDE_DIRS ${OpenIGTLink_INCLUDE_DIRS} "${PROJECT_SOURCE_DIR}/Source/VideoStreaming")
  SET(OpenIGTLink_SOURCES
    ${OpenIGTLink_SOURCES} 
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtl_video.c
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlVideoMessage.cxx
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VideoStreamIGTLinkServer.cxx
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VideoStreamIGTLinkReceiver.cxx
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlCodecCommonClasses.cxx
    )
	SET(OpenIGTLink_INCLUDE_DIRS
    ${OpenIGTLink_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming
  )
  IF( MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode" )
    SET(OpenIGTLink_INCLUDE_FILES
      ${OpenIGTLink_INCLUDE_FILES} 
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtl_video.h
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlVideoMessage.h
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VideoStreamIGTLinkServer.h
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VideoStreamIGTLinkReceiver.h
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlCodecCommonClasses.h
      )
  ENDIF()
  IF(LINK_H264)
    SET(OpenIGTLink_SOURCES
      ${OpenIGTLink_SOURCES}
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H264Decoder.cxx
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H264Encoder.cxx
      ${H264_SOURCE_DIR}/test/api/sha1.c
      ${H264_SOURCE_DIR}/codec/console/common/src/read_config.cpp
    )
    IF( MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode" )
      SET(OpenIGTLink_INCLUDE_FILES
      	${OpenIGTLink_INCLUDE_FILES} 
        ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H264Decoder.h
        ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H264Encoder.h
        ${H264_SOURCE_DIR}/test/sha1.h
        ${H264_SOURCE_DIR}/codec/console/common/inc/read_config.h
      )
    ENDIF()
  ENDIF()
  IF(LINK_VP9)
    SET(OpenIGTLink_SOURCES
    	${OpenIGTLink_SOURCES}
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VP9Decoder.cxx
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VP9Encoder.cxx
      ${VP9_SOURCE_DIR}/tools_common.c
      ${VP9_SOURCE_DIR}/video_reader.c
      ${VP9_SOURCE_DIR}/ivfdec.c
      ${VP9_SOURCE_DIR}/vpx_config.c
    )
    IF( MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode" )
      SET(OpenIGTLink_INCLUDE_FILES
      	${OpenIGTLink_INCLUDE_FILES}
        ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VP9Decoder.h
        ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VP9Encoder.h
        ${VP9_SOURCE_DIR}/vpx_config.h
        ${VP9_SOURCE_DIR}/tools_common.h
        ${VP9_SOURCE_DIR}/video_reader.h
        ${VP9_SOURCE_DIR}/ivfdec.h
      )
    ENDIF()
  ENDIF()
  IF(LINK_X265)
    SET(OpenIGTLink_SOURCES
      ${OpenIGTLink_SOURCES}
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H265Encoder.cxx
    )
    IF( MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode" )
      SET(OpenIGTLink_INCLUDE_FILES
        ${OpenIGTLink_INCLUDE_FILES}
        ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H265Encoder.h
        )
    ENDIF()
  ENDIF()
  IF(LINK_OPENHEVC)
    SET(OpenIGTLink_SOURCES
      ${OpenIGTLink_SOURCES}
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H265Decoder.cxx
    )
    IF( MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode" )
      SET(OpenIGTLink_INCLUDE_FILES
        ${OpenIGTLink_INCLUDE_FILES}
        ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H265Decoder.h
        )
    ENDIF()
  ENDIF()
ENDIF()
