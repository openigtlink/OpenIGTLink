# Add support for OpenIGTLink version 3
IF(${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" AND BUILD_VIDEOSTREAM)
  LIST(APPEND OpenIGTLink_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/Source/VideoStreaming)
  LIST(APPEND OpenIGTLink_SOURCES
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtl_video.c
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtl_videometa.c
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlVideoMessage.cxx
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VideoStreamIGTLinkServer.cxx
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VideoStreamIGTLinkReceiver.cxx
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlCodecCommonClasses.cxx
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlVideoMetaMessage.cxx
    )
  LIST(APPEND OpenIGTLink_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming
    )
  IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
    LIST(APPEND OpenIGTLink_INCLUDE_FILES
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtl_video.h
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtl_videometa.h
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlVideoMessage.h
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VideoStreamIGTLinkServer.h
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VideoStreamIGTLinkReceiver.h
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlCodecCommonClasses.h
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlVideoMetaMessage.h
      )
  ENDIF()
  IF(USE_H264)
    INCLUDE(${OpenIGTLink_SOURCE_DIR}/SuperBuild/External_openh264.cmake)
  	IF(NOT ${OpenH264_DIR} STREQUAL "")
			LIST(APPEND OpenIGTLink_SOURCES
				${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H264Decoder.cxx
				${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H264Encoder.cxx
				)
			IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
				LIST(APPEND OpenIGTLink_INCLUDE_FILES
					${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H264Decoder.h
					${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H264Encoder.h)
			ENDIF()
			LIST(APPEND OpenIGTLink_INCLUDE_DIRS
    		${OpenH264_DIR}
    		${OpenH264_DIR}/codec/api/svc
    	)
		ELSE()
			MESSAGE("H264_LIBRARY no found.  You could specify now , or it will be downloaded during the openigtlink build, but build of the codec should be done by the user.")
		ENDIF()
	ENDIF()
	
	IF(USE_OPENHEVC OR USE_X265 OR USE_VP9)
  	INCLUDE(${OpenIGTLink_SOURCE_DIR}/SuperBuild/External_yasm.cmake)
  	LIST(APPEND OpenIGTLink_DEPENDENCIES yasm)
  ENDIF()	
	
  IF(USE_VP9)
    INCLUDE(${OpenIGTLink_SOURCE_DIR}/SuperBuild/External_VP9.cmake)
  	IF(EXISTS ${VP9_LIBRARY_DIR})
  		LIST(APPEND OpenIGTLink_INCLUDE_DIRS
    		${VP9_SOURCE_DIR}
    		${VP9_LIBRARY_DIR}
    	)
			LIST(APPEND OpenIGTLink_SOURCES
				${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VP9Decoder.cxx
				${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VP9Encoder.cxx
				)
			IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
				LIST(APPEND OpenIGTLink_INCLUDE_FILES
					${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VP9Decoder.h
					${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VP9Encoder.h
				)
			ENDIF()
			IF(NOT ${VP9_LIBRARY_DIR} EQUAL "")
				LIST(APPEND OpenIGTLink_INCLUDE_DIRS
    		"${VP9_LIBRARY_DIR}" )
    		LINK_DIRECTORIES("${VP9_LIBRARY_DIR}/lib")
			ENDIF()	
		ELSE()
			MESSAGE("VP9_SOURCE_DIR or VP9_LIBRARY_DIR no found")
		ENDIF()
  ENDIF()

  IF(USE_X265)
  	INCLUDE(${OpenIGTLink_SOURCE_DIR}/SuperBuild/External_X265.cmake)
    LIST(APPEND OpenIGTLink_SOURCES
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H265Encoder.cxx
      )
    LIST(APPEND OpenIGTLink_INCLUDE_DIRS
    		${X265_INCLUDE_DIR}/source
    	)
    IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
      LIST(APPEND OpenIGTLink_INCLUDE_FILES
        ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H265Encoder.h
        )
    ENDIF()
    LIST(APPEND OpenIGTLink_INCLUDE_DIRS
    		${X265_INCLUDE_DIR}
    		${X265_LIBRARY_DIR}
    	)
  ENDIF()
	
  IF(USE_OPENHEVC)
  	INCLUDE(${OpenIGTLink_SOURCE_DIR}/SuperBuild/External_openHEVC.cmake)
  	LIST(APPEND OpenIGTLink_INCLUDE_DIRS
    		${OPENHEVC_SOURCE_DIR}
    		${OPENHEVC_LIBRARY_DIR}
    	)
    LIST(APPEND OpenIGTLink_SOURCES
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H265Decoder.cxx
      )
    IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
      LIST(APPEND OpenIGTLink_INCLUDE_FILES
        ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H265Decoder.h
        )
    ENDIF()
  ENDIF()
ENDIF()

IF(WIN32) # for Windows
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND USE_H264)
		LIST(APPEND LINK_LIBS
    	${OpenH264_DIR}/openh264.lib
    )
	ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND USE_VP9)
    #To do, library name depends on the compiler setting, could be vpxmt.lib and vpxmtd also. Make sure the setting matches.
    SET(LINK_VP9_LIBRARY optimized ${VP9_LIBRARY_DIR}\\$(Platform)\\Release\\vpxmd.lib debug ${VP9_LIBRARY_DIR}\\$(Platform)\\Debug\\vpxmdd.lib)
    LIST(APPEND LINK_LIBS
      ${LINK_VP9_LIBRARY}
    )
  ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND USE_X265)
    #To do, library name depends on the compiler setting, could be vpxmt.lib and vpxmtd also. Make sure the setting matches.
    SET(LINK_X265_LIBRARY ${X265_LIBRARY_DIR}\\${CMAKE_BUILD_TYPE}\\x265-static.lib)
    LIST(APPEND LINK_LIBS
      ${LINK_X265_LIBRARY}
    )
  ENDIF()
ELSE() # for POSIX-compatible OSs
	IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND USE_H264)
		LIST(APPEND LINK_LIBS
		  ${OpenH264_DIR}/libopenh264.a
		)
	ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND USE_VP9)
    LIST(APPEND LINK_LIBS
      ${VP9_LIBRARY_DIR}/libvpx.a
    )
  ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND USE_X265)
    LIST(APPEND LINK_LIBS
      ${X265_LIBRARY_DIR}/libx265.a
    )
  ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND USE_OPENHEVC)
  	SET(LINK_OPENHEVC_LIBRARY ${OPENHEVC_LIBRARY_DIR}/${CMAKE_BUILD_TYPE}/libLibOpenHevcWrapper.a )
  	LIST(APPEND LINK_LIBS
    	${LINK_OPENHEVC_LIBRARY}      
    )
  ENDIF()
ENDIF()
