PROJECT(SampleUDPProgam)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(OpenIGTLink REQUIRED)

include(${OpenIGTLink_USE_FILE})

ADD_EXECUTABLE(ClientUDPTransfer   ClientUDPTransfer.cxx)
TARGET_LINK_LIBRARIES(ClientUDPTransfer)

ADD_EXECUTABLE(ServerUDPTransfer   ServerUDPTransfer.cxx)
TARGET_LINK_LIBRARIES(ServerUDPTransfer)



