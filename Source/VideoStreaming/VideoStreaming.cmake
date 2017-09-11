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
  	IF(OpenH264_FOUND)
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
    		${OpenH264_INCLUDE_DIR}
    		${OpenH264_INCLUDE_DIR}/codec/api/svc # build location
    		${OpenH264_INCLUDE_DIR}/include/wels # install location
    	)
		ELSE()
			MESSAGE("H264_LIBRARY no found.  You could specify now , or it will be downloaded during the openigtlink build")
		ENDIF()
	ENDIF()
  
  IF(USE_VP9)
    INCLUDE(${OpenIGTLink_SOURCE_DIR}/SuperBuild/External_VP9.cmake)
  	IF(EXISTS ${VP9_LIBRARY_DIR})
  		LIST(APPEND OpenIGTLink_INCLUDE_DIRS
    		${VP9_INCLUDE_DIR}
    		${VP9_INCLUDE_DIR}/vpx
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
			MESSAGE("VP9_INCLUDE_DIR or VP9_LIBRARY_DIR no found")
		ENDIF()
  ENDIF()

  IF(USE_X265)
  	INCLUDE(${OpenIGTLink_SOURCE_DIR}/SuperBuild/External_X265.cmake)
  	LIST(APPEND OpenIGTLink_INCLUDE_DIRS
    		${X265_INCLUDE_DIR}
    		${X265_INCLUDE_DIR}/source
    		${X265_LIBRARY_DIR}
    	)
    LIST(APPEND OpenIGTLink_SOURCES
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H265Encoder.cxx
      )
    IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
      LIST(APPEND OpenIGTLink_INCLUDE_FILES
        ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H265Encoder.h
        )
    ENDIF()
  ENDIF()
	
  IF(USE_OpenHEVC)
  	INCLUDE(${OpenIGTLink_SOURCE_DIR}/SuperBuild/External_openHEVC.cmake)
  	LIST(APPEND OpenIGTLink_INCLUDE_DIRS
    		${OpenHEVC_INCLUDE_DIR}
    		${OpenHEVC_INCLUDE_DIR}/gpac/modules/openhevc_dec
    		${OpenHEVC_LIBRARY_DIR}
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
    	${OpenH264_LIBRARY_DIR}/openh264.lib
    )
	ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND USE_VP9)
    #To do, library name depends on the compiler setting, could be vpxmt.lib and vpxmtd also. Make sure the setting matches.
    LIST(APPEND LINK_LIBS
      VP9_lib
    )
  ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND USE_X265)
    #To do, library name depends on the compiler setting, could be vpxmt.lib and vpxmtd also. Make sure the setting matches.
    SET(LINK_X265_LIBRARY optimized ${X265_LIBRARY_DIR}\\Release\\x265-static.lib debug ${X265_LIBRARY_DIR}\\Debug\\x265-static.lib)
    LIST(APPEND LINK_LIBS
      ${LINK_X265_LIBRARY}
    )
  ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND USE_OpenHEVC)
    #To do, library name depends on the compiler setting, could be vpxmt.lib and vpxmtd also. Make sure the setting matches.
    SET(LINK_OPENHEVC_LIBRARY optimized ${OpenHEVC_LIBRARY_DIR}\\Release\\LibOpenHevcWrapper.lib debug ${OpenHEVC_LIBRARY_DIR}\\Debug\\LibOpenHevcWrapper.lib)
    LIST(APPEND LINK_LIBS
      ${LINK_OPENHEVC_LIBRARY}
    )
  ENDIF()
ELSE() # for POSIX-compatible OSs
	IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND USE_H264)
		LIST(APPEND LINK_LIBS
		  ${OpenH264_LIBRARY_DIR}/libopenh264.a
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
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND USE_OpenHEVC)
  	SET(LINK_OPENHEVC_LIBRARY ${OpenHEVC_LIBRARY_DIR}/libLibOpenHevcWrapper.a)
  	IF (CMAKE_CONFIGURATION_TYPES) 
  		SET(LINK_OPENHEVC_LIBRARY optimized ${OpenHEVC_LIBRARY_DIR}/Release/libLibOpenHevcWrapper.a debug ${OpenHEVC_LIBRARY_DIR}/Debug/libLibOpenHevcWrapper.a)
  	ENDIF()	
  	LIST(APPEND LINK_LIBS
    	${LINK_OPENHEVC_LIBRARY}      
    )
  ENDIF()
ENDIF()
