PROJECT(Thread)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(OpenIGTLink REQUIRED)

include(${OpenIGTLink_USE_FILE})

ADD_EXECUTABLE(SingleMethodExecute  SingleMethodExecute.cxx)
TARGET_LINK_LIBRARIES(SingleMethodExecute  OpenIGTLink)

ADD_EXECUTABLE(MultipleMethodExecute  MultipleMethodExecute.cxx)
TARGET_LINK_LIBRARIES(MultipleMethodExecute  OpenIGTLink)

ADD_EXECUTABLE(SpawnThread  SpawnThread.cxx)
TARGET_LINK_LIBRARIES(SpawnThread  OpenIGTLink)


