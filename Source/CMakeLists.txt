# Build library dependency list
# Note: this part should be moved to OpenIGTLinkConfigPlatform.cmake
IF(WIN32)
  # for Windows
  LIST(APPEND LINK_LIBS
    ws2_32
    wsock32
    )
  #For debug under win32 system, the run time check mode should be set to /RTCu for multithreading purpose
  STRING(REPLACE "/RTC1" "/RTCu" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
  SET(OpenIGTLink_REQUIRED_CXX_FLAGS ${OpenIGTLink_REQUIRED_CXX_FLAGS})
ELSE()
  # for POSIX-compatible OSs
  LIST(APPEND LINK_LIBS
    m
    pthread
    )
  IF(OpenIGTLink_PLATFORM_QNX)
    LIST(APPEND LINK_LIBS
      c
      socket
      )
  ENDIF()
  IF(OpenIGTLink_PLATFORM_SUNOS)
    LIST(APPEND LINK_LIBS
      ${OpenIGTLink_STD_LINK_LIBRARIES}
      )
  ENDIF()
ENDIF()

#-----------------------------------------------------------------------------
# Include directories
SET(OpenIGTLink_INCLUDE_DIRS
  ${OpenIGTLink_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/igtlutil
  ${CMAKE_CURRENT_BINARY_DIR}/igtlutil
  )

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

Set(OpenIGTLink_INCLUDE_FILES
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

# Add support for OpenIGTLink version 2
IF(${OpenIGTLink_PROTOCOL_VERSION} GREATER "1")
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

# Add support for OpenIGTLink version 3
IF(${OpenIGTLink_PROTOCOL_VERSION} GREATER "2")
  LIST(APPEND OpenIGTLink_SOURCES
    igtlCommandMessage.cxx
    igtlQueryMessage.cxx
    igtlutil/igtl_command.c
    igtlutil/igtl_query.c
    igtlMessageRTPWrapper.cxx
    igtlGeneralSocket.cxx
    igtlUDPClientSocket.cxx
    igtlUDPServerSocket.cxx
    )
  LIST(APPEND OpenIGTLink_INCLUDE_FILES
    igtlCommandMessage.h
    igtlQueryMessage.h
    igtlutil/igtl_command.h
    igtlutil/igtl_query.h
    igtlMessageRTPWrapper.h
    igtlGeneralSocket.h
    igtlUDPClientSocket.h
    igtlUDPServerSocket.h
    )
ENDIF()

IF (NOT (${CMAKE_VERSION} VERSION_LESS 3.4))
  IF(OpenIGTLink_ENABLE_VIDEOSTREAMING)
    INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/VideoStreaming/igtlVideoStreaming.cmake)
  ENDIF()

  #-----------------------------------------------------------------------------
  # Configure WebSocket
  # Create the link to the WebSocket libraries
  IF(OpenIGTLink_USE_WEBSOCKET)
    FIND_PACKAGE(websocketpp REQUIRED)
    INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/WebSocket/UseWebSocket.cmake)
    # Copy the UseWebSocket.cmake file to the binary tree for setting up websocket build environment.
    CONFIGURE_FILE(${OpenIGTLink_SOURCE_DIR}/Source/WebSocket/UseWebSocket.cmake
      ${OpenIGTLink_BINARY_DIR}/UseWebSocket.cmake COPYONLY IMMEDIATE)
    LIST(APPEND OpenIGTLink_SOURCES
      ${CMAKE_CURRENT_SOURCE_DIR}/WebSocket/igtlWebServerSocket.cxx
      ${CMAKE_CURRENT_SOURCE_DIR}/WebSocket/igtlWebClientSocket.cxx
    )

    LIST(APPEND OpenIGTLink_INCLUDE_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/WebSocket/igtlWebServerSocket.h
      ${CMAKE_CURRENT_SOURCE_DIR}/WebSocket/igtlWebClientSocket.h
    )
    LIST(APPEND OpenIGTLink_INCLUDE_DIRS
      ${CMAKE_CURRENT_SOURCE_DIR}/WebSocket
      ${WEBSOCKETPP_INCLUDE_DIR}
      )
  ENDIF()
ELSE()
  message(WARNING "CMake >= 3.4.0 recommended to enable the the video stream library and websocket option")
ENDIF()
#-----------------------------------------------------------------------------
# Create the library
IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  ADD_LIBRARY(OpenIGTLink ${OpenIGTLink_SOURCES} ${OpenIGTLink_INCLUDE_FILES})
ELSE()
  ADD_LIBRARY(OpenIGTLink ${OpenIGTLink_SOURCES})
ENDIF()
foreach(p IN LISTS OpenIGTLink_INCLUDE_DIRS)
  target_include_directories(OpenIGTLink PUBLIC $<BUILD_INTERFACE:${p}>)
endforeach()
target_include_directories(OpenIGTLink PUBLIC $<INSTALL_INTERFACE:${OpenIGTLink_INSTALL_INCLUDE_DIR}>)

LIST(APPEND OpenIGTLink_DEPENDENCIES igtlutil)
IF (NOT (${CMAKE_VERSION} VERSION_LESS 3.4))
  IF(OpenIGTLink_ENABLE_VIDEOSTREAMING)
    IF(OpenIGTLink_USE_H264 AND (NOT OpenH264_FOUND))
      add_dependencies(OpenIGTLink OpenH264)
    ENDIF()
    IF(OpenIGTLink_USE_VP9 AND (NOT VP9_FOUND))
      add_dependencies(OpenIGTLink VP9)
    ENDIF()
    IF(OpenIGTLink_USE_X265 AND (NOT X265_FOUND))
      add_dependencies(OpenIGTLink X265)
    ENDIF()
    IF(OpenIGTLink_USE_OpenHEVC AND (NOT OpenHEVC_FOUND))
      add_dependencies(OpenIGTLink OpenHEVC)
    ENDIF()
    IF(OpenIGTLink_USE_AV1 AND (NOT AV1_FOUND))
      add_dependencies(OpenIGTLink AV1)
    ENDIF()
  ENDIF()
ENDIF()

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
