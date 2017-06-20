PROJECT(WebSocket)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(OpenIGTLink REQUIRED)

include(${OpenIGTLink_USE_FILE})

## igtl::WebSocket examples
ADD_EXECUTABLE(WebSocketServer   WebSocketServer.cxx)

target_link_libraries(WebSocketServer OpenIGTLink ${Boost_LIBRARIES})

ADD_EXECUTABLE(WebSocketClient   WebSocketClient.cxx)
TARGET_LINK_LIBRARIES(WebSocketClient OpenIGTLink ${Boost_LIBRARIES})
