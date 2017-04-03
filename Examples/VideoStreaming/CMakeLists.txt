PROJECT(VideoStreaming)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(OpenIGTLink REQUIRED)

include(${OpenIGTLink_USE_FILE})

add_executable( VideoStreamReceiver VideoStreamReceiver.cxx)
target_link_libraries( VideoStreamReceiver OpenIGTLink)

add_executable( VideoStreamServer VideoStreamServer.cxx)
target_link_libraries( VideoStreamServer OpenIGTLink)
