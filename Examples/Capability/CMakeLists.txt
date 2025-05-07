PROJECT(Capability)

cmake_minimum_required(VERSION 3.5.0)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(OpenIGTLink REQUIRED)

include(${OpenIGTLink_USE_FILE})

ADD_EXECUTABLE(CapabilityClient   CapabilityClient.cxx)
TARGET_LINK_LIBRARIES(CapabilityClient OpenIGTLink)

ADD_EXECUTABLE(CapabilityServer   CapabilityServer.cxx)
TARGET_LINK_LIBRARIES(CapabilityServer OpenIGTLink)


