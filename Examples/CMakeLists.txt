cmake_minimum_required(VERSION 2.4)

#
# Examples
#
SET(EXAMPLE_DIRS
  Tracker
  Imager
  Status
  Receiver
  Thread
  )

if (${OpenIGTLink_PROTOCOL_VERSION} GREATER 1)
  SET(EXAMPLE_DIRS
    ${EXAMPLE_DIRS}
    ImageMeta
    Point
    TrackingData
    QuaternionTrackingData
    ImageDatabaseServer
    String
    Bind
    PolyData
    Capability
    Trajectory
    SessionManager
    TrackingDataUDPTransfer
    #SampleUDPProgam
    )
endif (${OpenIGTLink_PROTOCOL_VERSION} GREATER 1)

IF(${OpenIGTLink_PROTOCOL_VERSION} GREATER 2)
  IF(OpenIGTLink_USE_H264 OR OpenIGTLink_USE_VP9 OR (OpenIGTLink_USE_X265 AND OpenIGTLink_USE_OpenHEVC) OR OpenIGTLink_USE_AV1)
    SET(EXAMPLE_DIRS
      ${EXAMPLE_DIRS}
      VideoStreaming
      )
  ENDIF()
ENDIF()


IF(OpenIGTLink_USE_WEBSOCKET)
  SET(EXAMPLE_DIRS
    ${EXAMPLE_DIRS}
    WebSocket
  )
ENDIF()

## Imager program isn't supported by QNX
if(NOT OpenIGTLink_PLATFORM_QNX)
  SUBDIRS(
    ${EXAMPLE_DIRS}
    )
else(NOT OpenIGTLink_PLATFORM_QNX)
  SUBDIRS(
      Tracker
      Status
      Receiver
      )
endif(NOT OpenIGTLink_PLATFORM_QNX)



