PROJECT(TrackingData)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(OpenIGTLink REQUIRED)

include(${OpenIGTLink_USE_FILE})

ADD_EXECUTABLE(TrackingDataClient   TrackingDataClient.cxx)
TARGET_LINK_LIBRARIES(TrackingDataClient OpenIGTLink)

ADD_EXECUTABLE(TrackingDataServer   TrackingDataServer.cxx)
TARGET_LINK_LIBRARIES(TrackingDataServer OpenIGTLink)



