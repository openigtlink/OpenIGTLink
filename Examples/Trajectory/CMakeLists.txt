PROJECT(Trajectory)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(OpenIGTLink REQUIRED)

include(${OpenIGTLink_USE_FILE})

ADD_EXECUTABLE(TrajectoryClient   TrajectoryClient.cxx)
TARGET_LINK_LIBRARIES(TrajectoryClient OpenIGTLink)

ADD_EXECUTABLE(TrajectoryServer   TrajectoryServer.cxx)
TARGET_LINK_LIBRARIES(TrajectoryServer OpenIGTLink)


