PROJECT(Imager)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(OpenIGTLink REQUIRED)

include(${OpenIGTLink_USE_FILE})

ADD_EXECUTABLE(ImagerClient   ImagerClient.cxx)
TARGET_LINK_LIBRARIES(ImagerClient OpenIGTLink)

ADD_EXECUTABLE(ImagerClient2   ImagerClient2.cxx)
TARGET_LINK_LIBRARIES(ImagerClient2 OpenIGTLink)

ADD_EXECUTABLE(ImagerServer   ImagerServer.cxx)
TARGET_LINK_LIBRARIES(ImagerServer OpenIGTLink)

