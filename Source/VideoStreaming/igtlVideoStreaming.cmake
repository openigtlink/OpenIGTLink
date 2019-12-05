LIST(APPEND OpenIGTLink_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/Source/VideoStreaming)
LIST(APPEND OpenIGTLink_SOURCES
  ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtl_video.c
  ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtl_videometa.c
  ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlVideoMessage.cxx
  ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlVideoStreamIGTLinkServer.cxx
  ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlVideoStreamIGTLinkReceiver.cxx
  ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlCodecCommonClasses.cxx
  ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlVideoMetaMessage.cxx
  ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlI420Decoder.cxx
  ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlI420Encoder.cxx
  )
LIST(APPEND OpenIGTLink_INCLUDE_DIRS
  ${PROJECT_SOURCE_DIR}/Source/VideoStreaming
  )
LIST(APPEND OpenIGTLink_INCLUDE_FILES
  ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtl_video.h
  ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtl_videometa.h
  ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlVideoMessage.h
  ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlVideoStreamIGTLinkServer.h
  ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlVideoStreamIGTLinkReceiver.h
  ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlCodecCommonClasses.h
  ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlVideoMetaMessage.h
  ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlI420Decoder.h
  ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlI420Encoder.h
  )
IF(OpenIGTLink_USE_H264)
  INCLUDE(${OpenIGTLink_SOURCE_DIR}/SuperBuild/External_openh264.cmake)
  IF(EXISTS ${OpenH264_LIBRARY_DIR})
    LIST(APPEND OpenIGTLink_SOURCES
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlH264Decoder.cxx
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlH264Encoder.cxx
      )
    LIST(APPEND OpenIGTLink_INCLUDE_FILES
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlH264Decoder.h
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlH264Encoder.h
      )
    LIST(APPEND OpenIGTLink_INCLUDE_DIRS
      ${OpenH264_INCLUDE_DIR}
    )
  ELSE()
    MESSAGE("H264_LIBRARY no found.  You could specify OpenH264_INCLUDE_DIR or OpenH264_LIBRARY_DIR")
  ENDIF()
ENDIF()
IF(OpenIGTLink_USE_VP9)
  INCLUDE(${OpenIGTLink_SOURCE_DIR}/SuperBuild/External_VP9.cmake)
  IF((NOT "${VP9_LIBRARY_DIR}" STREQUAL "") AND (NOT "${VP9_INCLUDE_DIR}" STREQUAL ""))
    LIST(APPEND OpenIGTLink_INCLUDE_DIRS
      ${VP9_INCLUDE_DIR}
    )
    LIST(APPEND OpenIGTLink_SOURCES
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlVP9Decoder.cxx
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlVP9Encoder.cxx
    )
    LIST(APPEND OpenIGTLink_INCLUDE_FILES
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlVP9Decoder.h
      ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlVP9Encoder.h
    )
    LINK_DIRECTORIES("${VP9_LIBRARY_DIR}")
  ELSE()
    MESSAGE("VP9_INCLUDE_DIR or VP9_LIBRARY_DIR no found")
  ENDIF()
ENDIF()

IF(OpenIGTLink_USE_X265)
  INCLUDE(${OpenIGTLink_SOURCE_DIR}/SuperBuild/External_X265.cmake)
  LIST(APPEND OpenIGTLink_INCLUDE_DIRS
      ${X265_INCLUDE_DIR}
      ${X265_LIBRARY_DIR}
  )
  LIST(APPEND OpenIGTLink_SOURCES
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlH265Encoder.cxx
  )
  LIST(APPEND OpenIGTLink_INCLUDE_FILES
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlH265Encoder.h
  )
ENDIF()

IF(OpenIGTLink_USE_OpenHEVC)
  INCLUDE(${OpenIGTLink_SOURCE_DIR}/SuperBuild/External_openHEVC.cmake)
  LIST(APPEND OpenIGTLink_INCLUDE_DIRS
    ${OpenHEVC_INCLUDE_DIR}
  )
  LIST(APPEND OpenIGTLink_SOURCES
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlH265Decoder.cxx
  )
  LIST(APPEND OpenIGTLink_INCLUDE_FILES
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlH265Decoder.h
  )
ENDIF()

IF(OpenIGTLink_USE_AV1)
  INCLUDE(${OpenIGTLink_SOURCE_DIR}/SuperBuild/External_AV1.cmake)
  LIST(APPEND OpenIGTLink_INCLUDE_DIRS
    ${AV1_INCLUDE_DIR}
    ${AV1_LIBRARY_DIR}
  )
  LIST(APPEND OpenIGTLink_SOURCES
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlAV1Decoder.cxx
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlAV1Encoder.cxx
  )
  LIST(APPEND OpenIGTLink_INCLUDE_FILES
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlAV1Decoder.h
    ${PROJECT_SOURCE_DIR}/Source/VideoStreaming/igtlAV1Encoder.h
  )
ENDIF()

IF(WIN32) # for Windows
  IF(OpenIGTLink_USE_H264)
    LIST(APPEND LINK_LIBS
      ${OpenH264_LIBRARY_DIR}/openh264.lib
    )
  ENDIF()
  IF(OpenIGTLink_USE_VP9)
    #To do, library name depends on the compiler setting, could be vpxmt.lib and vpxmtd also. Make sure the setting matches.
    #SET(VP9_lib optimized ${VP9_LIBRARY_DIR}\\Release\\vpxmd.lib debug ${VP9_LIBRARY_DIR}\\Debug\\vpxmdd.lib)
    SET(VP9_lib ${VP9_LIBRARY_DIR}\\libvpx.lib)
    LIST(APPEND LINK_LIBS
      ${VP9_lib}
    )
  ENDIF()
  IF(OpenIGTLink_USE_X265)
    #To do, library name depends on the compiler setting, could be vpxmt.lib and vpxmtd also. Make sure the setting matches.
    SET(LINK_X265_LIBRARY optimized ${X265_LIBRARY_DIR}\\Release\\x265-static.lib debug ${X265_LIBRARY_DIR}\\Debug\\x265-static.lib)
    LIST(APPEND LINK_LIBS
      ${LINK_X265_LIBRARY}
    )
  ENDIF()
  IF(OpenIGTLink_USE_OpenHEVC)
    LIST(APPEND LINK_LIBS
      ${OpenHEVC_LIBRARY}
    )
  ENDIF()

  IF(OpenIGTLink_USE_AV1)
    LIST(APPEND LINK_LIBS
      ${AV1_LIBRARY}
    )
  ENDIF()
ELSE() # for POSIX-compatible OSs
  IF(OpenIGTLink_USE_H264)
    LIST(APPEND LINK_LIBS
     ${OpenH264_LIBRARY_DIR}/libopenh264.a
    )
  ENDIF()
  IF(OpenIGTLink_USE_VP9)
    LIST(APPEND LINK_LIBS
      ${VP9_LIBRARY_DIR}/libvpx.a
    )
  ENDIF()
  IF(OpenIGTLink_USE_X265)
    LIST(APPEND LINK_LIBS
      ${X265_LIBRARY_DIR}/libx265.a
    )
  ENDIF()
  IF(OpenIGTLink_USE_OpenHEVC)
    LIST(APPEND LINK_LIBS
      ${OpenHEVC_LIBRARY}
    )
  ENDIF()
  IF(OpenIGTLink_USE_AV1)
    LIST(APPEND LINK_LIBS
      ${AV1_LIBRARY}
    )
  ENDIF()
ENDIF()
