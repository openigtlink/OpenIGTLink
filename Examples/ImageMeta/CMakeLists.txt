PROJECT(ImageMeta)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(OpenIGTLink REQUIRED)

include(${OpenIGTLink_USE_FILE})


## igtl::ImageMetaMessage examples
ADD_EXECUTABLE(ImageMetaServer   ImageMetaServer.cxx)
TARGET_LINK_LIBRARIES(ImageMetaServer OpenIGTLink)

ADD_EXECUTABLE(ImageMetaClient   ImageMetaClient.cxx)
TARGET_LINK_LIBRARIES(ImageMetaClient OpenIGTLink)

## igtl::LabelMetaMessage example
ADD_EXECUTABLE(LabelMetaServer   LabelMetaServer.cxx)
TARGET_LINK_LIBRARIES(LabelMetaServer OpenIGTLink)

ADD_EXECUTABLE(LabelMetaClient   LabelMetaClient.cxx)
TARGET_LINK_LIBRARIES(LabelMetaClient OpenIGTLink)
