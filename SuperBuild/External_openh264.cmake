cmake_minimum_required(VERSION 2.8.2)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
IF(EXISTS ${H264_LIBRARY_DIR})
  # H264 has been built already
  IF(${OpenIGTLink_PROTOCOL_VERSION} LESS 3 OR (NOT ${BUILD_VIDEOSTREAM}))
    MESSAGE(FATAL_ERROR "Video streaming requires a build of OpenIGTLink with v3 support enabled. Please set the OpenIGTLink_PROTOCOL_VERSION_3 to true and activate the BUILD_VIDEOSTREAM.")
  ENDIF()
ELSE()
  # OpenIGTLink has not been built yet, so download and build it as an external project
  MESSAGE(STATUS "Downloading openh264 from https://github.com/cisco/openh264.git.")  
  SET (H264_SOURCE_DIR "${CMAKE_BINARY_DIR}/Deps/openh264" CACHE PATH "H264 source directory" FORCE)
  SET (H264_LIBRARY_DIR "${CMAKE_BINARY_DIR}/Deps/openh264-bin" CACHE PATH "H264 library directory" FORCE)   							
	configure_file(${OpenIGTLink_SOURCE_DIR}/SuperBuild/CMakeListsOpenH264Download.txt.in
  ${PROJECT_BINARY_DIR}/Deps/openh264-download/CMakeLists.txt)
	#Here the downloading project is triggered                                                               
	execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" -DH264_SOURCE_DIR:STRING=${H264_SOURCE_DIR} -DH264_LIBRARY_DIR:STRING=${H264_LIBRARY_DIR} . 
									WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/Deps/openh264-download" )
	execute_process(COMMAND "${CMAKE_COMMAND}" --build . 
									WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/Deps/openh264-download" )                    
	if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")             
					execute_process(COMMAND "make" WORKING_DIRECTORY "${H264_SOURCE_DIR}" )  
					execute_process(COMMAND make install PREFIX=${H264_LIBRARY_DIR} WORKING_DIRECTORY "${H264_SOURCE_DIR}" )                       
	endif(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows") 							
ENDIF()
