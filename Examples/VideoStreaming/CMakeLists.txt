PROJECT(VideoStreaming)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(OpenIGTLink REQUIRED)

include(${OpenIGTLink_USE_FILE})

add_executable( VideoServer VideoServer.cxx)
target_link_libraries( VideoServer OpenIGTLink)

add_executable( VideoClient VideoClient.cxx)
target_link_libraries( VideoClient OpenIGTLink)

add_executable( VideoMetaServer VideoMetaServer.cxx)
target_link_libraries( VideoMetaServer OpenIGTLink)

add_executable( VideoMetaClient VideoMetaClient.cxx)
target_link_libraries( VideoMetaClient OpenIGTLink)
