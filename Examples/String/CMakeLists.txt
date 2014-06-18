PROJECT(String)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(OpenIGTLink REQUIRED)

include(${OpenIGTLink_USE_FILE})

ADD_EXECUTABLE(StringClient   StringClient.cxx)
TARGET_LINK_LIBRARIES(StringClient OpenIGTLink)

ADD_EXECUTABLE(StringServer   StringServer.cxx)
TARGET_LINK_LIBRARIES(StringServer OpenIGTLink)

ADD_EXECUTABLE(StringEchoServer   StringEchoServer.cxx)
TARGET_LINK_LIBRARIES(StringEchoServer OpenIGTLink)



