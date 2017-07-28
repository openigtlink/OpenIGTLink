cmake_minimum_required(VERSION 2.8.2)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
IF((EXISTS ${VP9_SOURCE_DIR}) AND (EXISTS ${VP9_LIBRARY_DIR}))
  IF(${OpenIGTLink_PROTOCOL_VERSION} LESS 3 OR (NOT ${BUILD_VIDEOSTREAM}))
    MESSAGE(FATAL_ERROR "Video streaming requires a build of OpenIGTLink with v3 support enabled. Please set the OpenIGTLink_PROTOCOL_VERSION_3 to true and activate the BUILD_VIDEOSTREAM.")
  ENDIF()
ELSE()
  # OpenIGTLink has not been built yet, so download and build it as an external project
  MESSAGE(STATUS "Downloading VP9 from https://github.com/webmproject/libvpx.git")              
	SET (VP9_SOURCE_DIR "${CMAKE_BINARY_DIR}/Deps/VP9" CACHE PATH "VP9 source directory" FORCE)
  SET (VP9_LIBRARY_DIR "${CMAKE_BINARY_DIR}/Deps/VP9-bin" CACHE PATH "VP9 library directory" FORCE)   							
	configure_file(${OpenIGTLink_SOURCE_DIR}/SuperBuild/CMakeListsVP9Download.txt.in
  ${PROJECT_BINARY_DIR}/Deps/VP9-download/CMakeLists.txt)
	#Here the downloading project is triggered                                                               
	execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" -DH264_SOURCE_DIR:STRING=${VP9_SOURCE_DIR} -DH264_LIBRARY_DIR:STRING=${VP9_LIBRARY_DIR} . 
									WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/Deps/VP9-download" )
	execute_process(COMMAND "${CMAKE_COMMAND}" --build . 
									WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/Deps/VP9-download" )                    
	if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")             
		execute_process(COMMAND "${VP9_SOURCE_DIR}/configure" --disable-examples --disable-tools --disable-docs --disable-vp8 --disable-libyuv --disable-unit_tests --disable-postproc WORKING_DIRECTORY "${VP9_LIBRARY_DIR}" )
    execute_process(COMMAND "make" WORKING_DIRECTORY "${VP9_LIBRARY_DIR}" )                       
	endif(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows") 
ENDIF()
