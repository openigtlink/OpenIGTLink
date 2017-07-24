# Add support for OpenIGTLink version 3
IF(${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" AND (LINK_H264 OR LINK_VP9 OR LINK_X265 OR LINK_OPENHEVC))
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
  LIST(APPENDOpenIGTLink_INCLUDE_DIRS
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
  IF(USE_H264 AND OpenH264_FOUND)
    LIST(APPEND OpenIGTLink_SOURCES
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H264Decoder.cxx
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H264Encoder.cxx
      ${H264_SOURCE_DIR}/test/api/sha1.c
      ${H264_SOURCE_DIR}/codec/console/common/src/read_config.cpp
      )
    IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
      LIST(APPEND OpenIGTLink_INCLUDE_FILES
        ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H264Decoder.h
        ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H264Encoder.h
        ${H264_SOURCE_DIR}/test/sha1.h
        ${H264_SOURCE_DIR}/codec/console/common/inc/read_config.h
      )
    ENDIF()
  ENDIF()

  IF(USE_VP9 AND VP9_FOUND)
    LIST(APPEND OpenIGTLink_SOURCES
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VP9Decoder.cxx
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VP9Encoder.cxx
      ${VP9_SOURCE_DIR}/tools_common.c
      ${VP9_SOURCE_DIR}/video_reader.c
      ${VP9_SOURCE_DIR}/ivfdec.c
      ${VP9_LIBRARY_DIR}/vpx_config.c
      )
    IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
      LIST(APPEND OpenIGTLink_INCLUDE_FILES
        ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VP9Decoder.h
        ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/VP9Encoder.h
        ${VP9_LIBRARY_DIR}/vpx_config.h
        ${VP9_SOURCE_DIR}/tools_common.h
        ${VP9_SOURCE_DIR}/video_reader.h
        ${VP9_SOURCE_DIR}/ivfdec.h
      )
    ENDIF()
  ENDIF()

  IF(USE_X265 AND X265_FOUND)
    LIST(APPEND OpenIGTLink_SOURCES
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H265Encoder.cxx
      )
    IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
      LIST(APPEND OpenIGTLink_INCLUDE_FILES
        ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/H265Encoder.h
        )
    ENDIF()
  ENDIF()

  IF(USE_OPENHEVC and OpenHEVC_FOUND)
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
