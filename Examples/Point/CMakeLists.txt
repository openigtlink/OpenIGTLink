PROJECT(Point)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(OpenIGTLink REQUIRED)

include(${OpenIGTLink_USE_FILE})

ADD_EXECUTABLE(PointClient   PointClient.cxx)
TARGET_LINK_LIBRARIES(PointClient OpenIGTLink)

ADD_EXECUTABLE(PointServer   PointServer.cxx)
TARGET_LINK_LIBRARIES(PointServer OpenIGTLink)

ADD_EXECUTABLE(PointListServer   PointListServer.cxx)
TARGET_LINK_LIBRARIES(PointListServer OpenIGTLink)

ADD_EXECUTABLE(PointClient3   PointClient3.cxx)
TARGET_LINK_LIBRARIES(PointClient3 OpenIGTLink)

ADD_EXECUTABLE(PointServer3   PointServer3.cxx)
TARGET_LINK_LIBRARIES(PointServer3 OpenIGTLink)