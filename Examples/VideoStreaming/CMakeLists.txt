PROJECT(VideoStreaming)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(OpenIGTLink REQUIRED)

include(${OpenIGTLink_USE_FILE})

add_executable( VideoCompressionExample VideoCompressionExample.cxx)
target_link_libraries( VideoCompressionExample OpenIGTLink)

add_executable( VideoServer VideoServer.cxx)
target_link_libraries( VideoServer OpenIGTLink)

add_executable( VideoClient VideoClient.cxx)
target_link_libraries( VideoClient OpenIGTLink)

add_executable( VideoMetaServer VideoMetaServer.cxx)
target_link_libraries( VideoMetaServer OpenIGTLink)

add_executable( VideoMetaClient VideoMetaClient.cxx)
target_link_libraries( VideoMetaClient OpenIGTLink)

if(0 EQUAL 1)
  add_executable( VideoStreamReceiver Evaluation/VideoStreamReceiver.cxx)
  target_link_libraries( VideoStreamReceiver OpenIGTLink)

  add_executable( VideoStreamServer Evaluation/VideoStreamServer.cxx)
  target_link_libraries( VideoStreamServer OpenIGTLink)

  add_executable( CodecEvaluation Evaluation/CodecEvaluation.cxx)
  target_link_libraries(CodecEvaluation OpenIGTLink)

  add_executable( UltrasonixEval Evaluation/UltrasonixEval.cxx)
  target_link_libraries(UltrasonixEval OpenIGTLink)

  add_executable( X265EncodeTest Evaluation/X265EncodeTest.cxx)
  target_link_libraries(X265EncodeTest OpenIGTLink)
endif()