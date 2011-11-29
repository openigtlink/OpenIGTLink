PROJECT(PolyData)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(OpenIGTLink REQUIRED)

include(${OpenIGTLink_USE_FILE})


## igtl::PolyDataMessage examples
ADD_EXECUTABLE(PolyDataServer   PolyDataServer.cxx)
TARGET_LINK_LIBRARIES(PolyDataServer OpenIGTLink)

ADD_EXECUTABLE(PolyDataClient   PolyDataClient.cxx)
TARGET_LINK_LIBRARIES(PolyDataClient OpenIGTLink)

