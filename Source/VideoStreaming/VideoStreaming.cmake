# Add support for OpenIGTLink version 3
IF(${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" AND (USE_H264 OR USE_VP9 OR USE_X265 OR USE_OPENHEVC))
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
  	IF(EXISTS ${H264_LIBRARY_DIR})
			LIST(APPEND OpenIGTLink_SOURCES
				${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H264Decoder.cxx
				${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H264Encoder.cxx
				)
			IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
				LIST(APPEND OpenIGTLink_INCLUDE_FILES
					${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H264Decoder.h
					${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H264Encoder.h)
			ENDIF()
    	IF(NOT ${H264_LIBRARY_DIR} EQUAL "")
				LIST(APPEND OpenIGTLink_INCLUDE_DIRS
    		"${H264_LIBRARY_DIR}/include/wels/" )
    		LINK_DIRECTORIES("${H264_LIBRARY_DIR}/lib")
			ENDIF()	
		ELSE()
			MESSAGE("H264_LIBRARY_DIR no found.  You could specify now , or it will be downloaded during the openigtlink build, but build of the codec should be done by the user.")
		ENDIF()
	ENDIF()
	
  IF(USE_VP9)
    LIST(APPEND OpenIGTLink_DEPENDENCIES VP9)
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
  	INCLUDE(${OpenIGTLink_SOURCE_DIR}/SuperBuild/External_x265.cmake)
    LIST(APPEND OpenIGTLink_SOURCES
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H265Encoder.cxx
      )
    IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
      LIST(APPEND OpenIGTLink_INCLUDE_FILES
        ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H265Encoder.h
        )
    ENDIF()
  ENDIF()

  IF(USE_OPENHEVC)
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


IF(OpenIGTLink_PLATFORM_WIN32) # for Windows
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND USE_H264)
		LIST(APPEND LINK_LIBS
    	${H264_LIBRARY_DIR}/lib/openh264.lib
    )
	ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND USE_VP9)
    #To do, library name depends on the compiler setting, could be vpxmt.lib and vpxmtd also. Make sure the setting matches.
    SET(LINK_VP9_LIBRARY optimized ${VP9_LIBRARY_DIR}\\$(Platform)\\Release\\vpxmd.lib debug ${VP9_LIBRARY_DIR}\\$(Platform)\\Debug\\vpxmdd.lib)
    LIST(APPEND LINK_LIBS
      ${LINK_VP9_LIBRARY}
    )
  ENDIF()
ELSE() # for POSIX-compatible OSs
	IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND USE_H264)
		LIST(APPEND LINK_LIBS
		  ${H264_LIBRARY_DIR}/lib/libopenh264.a
		)
	ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND USE_VP9)
    LIST(APPEND LINK_LIBS
      ${VP9_LIBRARY_DIR}/libvpx.a
    )
  ENDIF()
ENDIF()
