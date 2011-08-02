PROJECT(Bind)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(OpenIGTLink REQUIRED)

include(${OpenIGTLink_USE_FILE})

ADD_EXECUTABLE(BindClient   BindClient.cxx)
TARGET_LINK_LIBRARIES(BindClient OpenIGTLink)

#ADD_EXECUTABLE(BindServer   BindServer.cxx)
#TARGET_LINK_LIBRARIES(BindServer OpenIGTLink)


