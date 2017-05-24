# list of libraries
# Note: this part should be moved to OpenIGTLinkConfigPlatform.cmake
IF(OpenIGTLink_PLATFORM_WIN32) # for Windows
  SET(LINK_LIBS
    ws2_32
    wsock32
    )
	IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND BUILD_H264)
		SET(LINK_LIBS
		${LINK_LIBS}
		"${CMAKE_BINARY_DIR}/OpenH264/openh264.lib"
		)
	ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND BUILD_VPX)
	SET(LINK_VP9_LIBRARY optimized ${CMAKE_BINARY_DIR}/OpenVPX/Win32/Release/vpxmt.lib debug ${CMAKE_BINARY_DIR}/OpenVPX/Win32/Debug/vpxmtd.lib)
	SET(LINK_LIBS
      ${LINK_LIBS}
      ${LINK_VP9_LIBRARY}
    )
  ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_X265)
    SET(LINK_X265_LIBRARY optimized ${X265_LIBRARY_DIR}\\Release\\x265-static.lib debug ${X265_LIBRARY_DIR}\\Debug\\x265-static.lib)
    SET(LINK_OPENHEVC_LIBRARY ${OPENHEVC_LIBRARY_DIR}\\libLibOpenHevcWrapper.dll)
    SET(LINK_LIBS
      ${LINK_LIBS}
      ${LINK_X265_LIBRARY}
      #${LINK_OPENHEVC_LIBRARY}
    )
  ENDIF()
  #For debug under win32 system, the run time check mode should be set to /RTCu for multithreading purpose
  STRING(REPLACE "/RTC1" "/RTCu" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
  SET(OpenIGTLink_REQUIRED_CXX_FLAGS ${OpenIGTLink_REQUIRED_CXX_FLAGS})
ELSE() # for POSIX-compatible OSs
  SET(LINK_LIBS
    m
    pthread
    )
  IF(OpenIGTLink_PLATFORM_QNX)
    LIST(APPEND LINK_LIBS
      c
      socket
    )
  ENDIF()
	IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND BUILD_H264)
		SET(LINK_LIBS
		${LINK_LIBS}
		"${CMAKE_BINARY_DIR}/OpenH264/libopenh264.a"
		)
	ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND BUILD_VPX)
    SET(LINK_LIBS
    ${LINK_LIBS}
    "${CMAKE_BINARY_DIR}/OpenVPX/libvpx.a"
    )
  ENDIF()
  IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_X265)
    SET(LINK_X265_LIBRARY optimized ${X265_LIBRARY_DIR}/libx265.a debug ${X265_LIBRARY_DIR}/libx265.a)
    SET(LINK_OPENHEVC_LIBRARY ${OPENHEVC_LIBRARY_DIR}/libLibOpenHevcWrapper.a)
    SET(LINK_LIBS
      ${LINK_LIBS}
      ${LINK_X265_LIBRARY}
      ${LINK_OPENHEVC_LIBRARY}
    )
  ENDIF()
  IF(OpenIGTLink_PLATFORM_SUNOS)
    LIST(APPEND LINK_LIBS
      ${OpenIGTLink_STD_LINK_LIBRARIES}
      )
  ENDIF()
ENDIF()

ADD_SUBDIRECTORY( igtlutil )

SET(OpenIGTLink_INCLUDE_DIRS
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/igtlutil
  ${CMAKE_CURRENT_BINARY_DIR}/igtlutil
  )
  
IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND BUILD_H264)
	SET(OpenIGTLink_INCLUDE_DIRS
	${OpenIGTLink_INCLUDE_DIRS}
  "${CMAKE_BINARY_DIR}/OpenH264/codec"
  "${CMAKE_BINARY_DIR}/OpenH264/codec/common/inc"
  "${CMAKE_BINARY_DIR}/OpenH264/codec/console/common/src"
	"${CMAKE_BINARY_DIR}/OpenH264/codec/console/common/inc"
	"${CMAKE_BINARY_DIR}/OpenH264/codec/console/dec/inc"
	"${CMAKE_BINARY_DIR}/OpenH264/codec/console/enc/src"
	"${CMAKE_BINARY_DIR}/OpenH264/codec/api/svc"
	"${CMAKE_BINARY_DIR}/OpenH264/codec/encoder/core/inc"
	"${CMAKE_BINARY_DIR}/OpenH264/codec/processing/interface"
	"${CMAKE_BINARY_DIR}/OpenH264/test")
ENDIF()

IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND BUILD_VPX)
  SET(OpenIGTLink_INCLUDE_DIRS
  ${OpenIGTLink_INCLUDE_DIRS}
    "${CMAKE_BINARY_DIR}/OpenVPX"
    "${CMAKE_BINARY_DIR}/OpenVPX/vpx"
    "${CMAKE_BINARY_DIR}/OpenVPX/vp9"
    "${CMAKE_BINARY_DIR}/OpenVPX/vp9/common"
  )
ENDIF()

IF((${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" ) AND LINK_X265)
  SET(OpenIGTLink_INCLUDE_DIRS
    ${OpenIGTLink_INCLUDE_DIRS}
    "${X265_SOURCE_DIR}"
    "${X265_LIBRARY_DIR}"
    "${OPENHEVC_SOURCE_DIR}"
	"${OPENHEVC_SOURCE_DIR}/platform/arm"
    "${OPENHEVC_LIBRARY_DIR}"
  )
ENDIF()

SET(OpenIGTLink_SOURCES
  igtlutil/igtl_header.c
  igtlutil/igtl_image.c
  igtlutil/igtl_transform.c
  igtlutil/igtl_status.c
  igtlutil/igtl_util.c
  igtlutil/igtl_position.c
  igtlutil/igtl_capability.c
  igtlClientSocket.cxx
  igtlCapabilityMessage.cxx
  igtlConditionVariable.cxx
  igtlFastMutexLock.cxx
  igtlImageMessage.cxx
  igtlImageMessage2.cxx
  igtlLightObject.cxx
  igtlMath.cxx
  igtlMessageBase.cxx
  igtlMessageFactory.cxx
  igtlMultiThreader.cxx
  igtlMutexLock.cxx
  igtlOSUtil.cxx
  igtlObject.cxx
  igtlObjectFactoryBase.cxx
  igtlPositionMessage.cxx
  igtlServerSocket.cxx
  igtlSessionManager.cxx
  igtlSimpleFastMutexLock.cxx
  igtlSocket.cxx
  igtlStatusMessage.cxx
  igtlTimeStamp.cxx
  igtlTransformMessage.cxx
  )

SET(OpenIGTLink_INCLUDE_FILES)
IF( MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode" )
  LIST(APPEND OpenIGTLink_INCLUDE_FILES
    igtlutil/igtl_header.h
    igtlutil/igtl_image.h
    igtlutil/igtl_position.h
    igtlutil/igtl_transform.h
    igtlutil/igtl_types.h
    igtlutil/igtl_util.h
    igtlutil/igtl_capability.h
    igtlutil/igtl_win32header.h
    igtlMessageHandler.h
    igtlMessageHandlerMacro.h
    igtlCapabilityMessage.h
    igtlClientSocket.h
    igtlConditionVariable.h
    igtlCreateObjectFunction.h
    igtlFastMutexLock.h
    igtlImageMessage.h
    igtlImageMessage2.h
    igtlLightObject.h
    igtlMacro.h
    igtlMath.h
    igtlMessageBase.h
    igtlMessageFactory.h
    igtlMessageHeader.h
    igtlMultiThreader.h
    igtlMutexLock.h
    igtlObjectFactory.h
    igtlOSUtil.h
    igtlObject.h
    igtlObjectFactoryBase.h
    igtlPositionMessage.h
    igtlServerSocket.h
    igtlSessionManager.h
    igtlSimpleFastMutexLock.h
    igtlSmartPointer.h
    igtlSocket.h
    igtlStatusMessage.h
    igtlTimeStamp.h
    igtlTransformMessage.h
    igtlTypes.h
    igtlWin32Header.h
    igtlWindows.h
    igtlCommon.h
  )
ENDIF()

# Add support for OpenIGTLink version 2
IF (${OpenIGTLink_PROTOCOL_VERSION} GREATER "1" )
  LIST(APPEND OpenIGTLink_SOURCES
    igtlutil/igtl_colortable.c
    igtlutil/igtl_imgmeta.c
    igtlutil/igtl_lbmeta.c
    igtlutil/igtl_point.c
    igtlutil/igtl_tdata.c
    igtlutil/igtl_qtdata.c
    igtlutil/igtl_trajectory.c
    igtlutil/igtl_unit.c
    igtlutil/igtl_sensor.c
    igtlutil/igtl_string.c
    igtlutil/igtl_ndarray.c
    igtlutil/igtl_bind.c
    igtlutil/igtl_qtrans.c
    igtlutil/igtl_polydata.c
    igtlColorTableMessage.cxx
    igtlImageMetaMessage.cxx
    igtlLabelMetaMessage.cxx
    igtlPointMessage.cxx
    igtlTrackingDataMessage.cxx
    igtlPolyDataMessage.cxx
    igtlQuaternionTrackingDataMessage.cxx
    igtlTrajectoryMessage.cxx
    igtlStringMessage.cxx
    igtlUnit.cxx
    igtlSensorMessage.cxx
    igtlBindMessage.cxx
    igtlNDArrayMessage.cxx
    )
  IF( MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode" )
    LIST(APPEND OpenIGTLink_INCLUDE_FILES
      igtlutil/igtl_colortable.h
      igtlutil/igtl_imgmeta.h
      igtlutil/igtl_lbmeta.h
      igtlutil/igtl_point.h
      igtlutil/igtl_tdata.h
      igtlutil/igtl_qtdata.h
      igtlutil/igtl_trajectory.h
      igtlutil/igtl_unit.h
      igtlutil/igtl_sensor.h
      igtlutil/igtl_string.h
      igtlutil/igtl_ndarray.h
      igtlutil/igtl_bind.h
      igtlutil/igtl_qtrans.h
      igtlutil/igtl_polydata.h
      igtlColorTableMessage.h
      igtlImageMetaMessage.h
      igtlLabelMetaMessage.h
      igtlPointMessage.h
      igtlTrackingDataMessage.h
      igtlPolyDataMessage.h
      igtlQuaternionTrackingDataMessage.h
      igtlTrajectoryMessage.h
      igtlStringMessage.h
      igtlUnit.h
      igtlSensorMessage.h
      igtlBindMessage.h
      igtlNDArrayMessage.h
      )
  ENDIF()
ENDIF()

# Add support for OpenIGTLink version 3
IF( ${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" )
  LIST(APPEND OpenIGTLink_SOURCES
    igtlCommandMessage.cxx
    igtlQueryMessage.cxx
    igtlutil/igtl_command.c
    igtlutil/igtl_query.c
    igtlutil/igtl_video.c
    igtlVideoMessage.cxx
    igtlMessageRTPWrapper.cxx
    igtlGeneralSocket.cxx
    igtlUDPClientSocket.cxx
    igtlUDPServerSocket.cxx
    VideoStreamIGTLinkServer.cxx
    VideoStreamIGTLinkReceiver.cxx
    igtlCodecCommonClasses.cxx
    )
  IF( MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode" )
    LIST(APPEND OpenIGTLink_INCLUDE_FILES
      igtlCommandMessage.h
      igtlQueryMessage.h
      igtlutil/igtl_command.h
      igtlutil/igtl_query.h
      igtlVideoMessage.h
      igtlMessageRTPWrapper.h
      igtlGeneralSocket.h
      igtlUDPClientSocket.h
      igtlUDPServerSocket.h
      VideoStreamIGTLinkServer.h
      VideoStreamIGTLinkReceiver.h
      igtlutil/igtl_video.h
      igtlCodecCommonClasses.h
      )
  ENDIF()
  IF(BUILD_H264)
    LIST(APPEND OpenIGTLink_SOURCES
      H264Decoder.cxx
      H264Encoder.cxx
      ${CMAKE_BINARY_DIR}/OpenH264/test/api/sha1.c
      ${CMAKE_BINARY_DIR}/OpenH264/codec/console/common/src/read_config.cpp
    )
    IF( MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode" )
      LIST(APPEND OpenIGTLink_INCLUDE_FILES
        H264Decoder.h
        H264Encoder.h
        ${CMAKE_BINARY_DIR}/OpenH264/test/sha1.h
        ${CMAKE_BINARY_DIR}/OpenH264/codec/console/common/inc/read_config.h
      )
    ENDIF()
  ENDIF()
  IF(BUILD_VPX)
    LIST(APPEND OpenIGTLink_SOURCES
      VPXDecoder.cxx
      VPXEncoder.cxx
      ${CMAKE_BINARY_DIR}/OpenVPX/tools_common.c
      ${CMAKE_BINARY_DIR}/OpenVPX/video_reader.c
      ${CMAKE_BINARY_DIR}/OpenVPX/ivfdec.c
      ${CMAKE_BINARY_DIR}/OpenVPX/vpx_config.c
    )
    IF( MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode" )
      LIST(APPEND OpenIGTLink_INCLUDE_FILES
        VPXDecoder.h
        VPXEncoder.h
        ${CMAKE_BINARY_DIR}/OpenVPX/vpx_config.h
        ${CMAKE_BINARY_DIR}/OpenVPX/tools_common.h
        ${CMAKE_BINARY_DIR}/OpenVPX/video_reader.h
        ${CMAKE_BINARY_DIR}/OpenVPX/ivfdec.h
      )
    ENDIF()
  ENDIF()
  IF(LINK_X265)
    LIST(APPEND OpenIGTLink_SOURCES
      H265Decoder.cxx
      H265Encoder.cxx
    )
    IF( MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode" )
    LIST(APPEND OpenIGTLink_INCLUDE_FILES
      H265Decoder.h
      H265Encoder.h
    )
    ENDIF()
  ENDIF()
ENDIF()

ADD_LIBRARY(OpenIGTLink ${OpenIGTLink_SOURCES} ${OpenIGTLink_INCLUDE_FILES})
foreach(p IN LISTS OpenIGTLink_INCLUDE_DIRS)
  target_include_directories(OpenIGTLink PUBLIC 
    $<BUILD_INTERFACE:${p}>)
endforeach()
target_include_directories(OpenIGTLink PUBLIC 
  $<INSTALL_INTERFACE:${OpenIGTLink_INSTALL_INCLUDE_DIR}>)

TARGET_LINK_LIBRARIES(OpenIGTLink PUBLIC ${LINK_LIBS})

IF(MSVC)
  target_compile_options(OpenIGTLink PRIVATE /MP)
ENDIF()

SET_TARGET_PROPERTIES(OpenIGTLink PROPERTIES
  VERSION ${OpenIGTLink_VERSION_MAJOR}.${OpenIGTLink_VERSION_MINOR}.${OpenIGTLink_VERSION_PATCH}
  SOVERSION ${OpenIGTLink_VERSION_MAJOR}
  )

INSTALL(FILES ${OpenIGTLink_INCLUDE_FILES}
  DESTINATION ${OpenIGTLink_INSTALL_INCLUDE_DIR}
  COMPONENT Development)

INSTALL(TARGETS OpenIGTLink EXPORT OpenIGTLink
  RUNTIME DESTINATION ${OpenIGTLink_INSTALL_BIN_DIR} COMPONENT RuntimeLibraries
  LIBRARY DESTINATION ${OpenIGTLink_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${OpenIGTLink_INSTALL_LIB_DIR} COMPONENT Development)