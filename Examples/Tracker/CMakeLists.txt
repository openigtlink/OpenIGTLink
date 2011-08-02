PROJECT(Tracker)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(OpenIGTLink REQUIRED)

include(${OpenIGTLink_USE_FILE})

ADD_EXECUTABLE(TrackerClient   TrackerClient.cxx)
TARGET_LINK_LIBRARIES(TrackerClient OpenIGTLink)
ADD_EXECUTABLE(TrackerServer   TrackerServer.cxx)
TARGET_LINK_LIBRARIES(TrackerServer OpenIGTLink)

ADD_EXECUTABLE(TrackerClient2   TrackerClient2.cxx)
TARGET_LINK_LIBRARIES(TrackerClient2 OpenIGTLink)
ADD_EXECUTABLE(TrackerServer2   TrackerServer2.cxx)
TARGET_LINK_LIBRARIES(TrackerServer2 OpenIGTLink)

ADD_EXECUTABLE(TrackerClient3   TrackerClient3.cxx)
TARGET_LINK_LIBRARIES(TrackerClient3 OpenIGTLink)


