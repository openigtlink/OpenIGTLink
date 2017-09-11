#---------------------------------------------------
SET(OpenIGTLink_DEPENDENCIES)

#---------------------------------------------------
# VideoStreaming dependencies
IF(BUILD_VIDEOSTREAM)
  IF(USE_H264) 
    INCLUDE(${CMAKE_CURRENT_LIST_DIR}/External_openh264.cmake)
    IF(NOT OpenH264_FOUND)
			LIST(APPEND OpenIGTLink_DEPENDENCIES OpenH264)
		ENDIF()
  ENDIF()
  
  IF(USE_VP9)
    INCLUDE(${CMAKE_CURRENT_LIST_DIR}/External_VP9.cmake)
    IF(NOT VP9_FOUND)
			LIST(APPEND OpenIGTLink_DEPENDENCIES VP9)
		ENDIF()
  ENDIF()
	
  IF(USE_X265)
    INCLUDE(${CMAKE_CURRENT_LIST_DIR}/External_X265.cmake)
    IF(NOT X265_FOUND)
			LIST(APPEND OpenIGTLink_DEPENDENCIES X265)
		ENDIF()	
  ENDIF()
  
  IF(USE_OpenHEVC)
 		INCLUDE(${CMAKE_CURRENT_LIST_DIR}/External_openHEVC.cmake)
    IF(NOT OpenHEVC_FOUND)
			LIST(APPEND OpenIGTLink_DEPENDENCIES OpenHEVC)
		ENDIF()
  ENDIF()

  IF(BUILD_WEBSOCKET)
    INCLUDE(${CMAKE_CURRENT_LIST_DIR}/External_WebSocket.cmake)
    LIST(APPEND OpenIGTLink_DEPENDENCIES websocket)
  ENDIF()
ENDIF()

ExternalProject_Add( OpenIGTLink-lib
	PREFIX "${CMAKE_BINARY_DIR}/OpenIGTLink-prefix"
	SOURCE_DIR "${CMAKE_BINARY_DIR}/OpenIGTLink"
	BINARY_DIR "${CMAKE_BINARY_DIR}/OpenIGTLink-bin"
	#--Download step--------------
	GIT_REPOSITORY "https://github.com/openigtlink/OpenIGTLink.git"
	GIT_TAG superbuild-findmodules
	#--Configure step-------------
	CMAKE_ARGS
		${PLATFORM_SPECIFIC_ARGS}
		-DOpenIGTLink_SUPERBUILD:BOOL=OFF
		-DCMAKE_LIBRARY_OUTPUT_DIRECTORY:STRING=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
		-DCMAKE_RUNTIME_OUTPUT_DIRECTORY:STRING=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
		-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
		-DBUILD_SHARED_LIBS:BOOL=${OpenIGTLink_BUILD_SHARED_LIBS} 
		-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		-DBUILD_TESTING:BOOL=${BUILD_TESTING}
		-DUSE_GTEST:BOOL=${USE_GTEST}
		-DBUILD_VIDEOSTREAM:BOOL=${BUILD_VIDEOSTREAM}
		-DUSE_H264:BOOL=${USE_H264}
		-DUSE_VP9:BOOL=${USE_VP9}
		-DUSE_X265:BOOL=${USE_X265}
		-DUSE_OpenHEVC:BOOL=${USE_OpenHEVC}
		-DBUILD_WEBSOCKET:BOOL=${BUILD_WEBSOCKET}
		-DBUILD_EXAMPLES:BOOL=${BUILD_EXAMPLES}
		-DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
		-DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
		-DOpenH264_INCLUDE_DIR:STRING=${OpenH264_INCLUDE_DIR}
		-DOpenH264_LIBRARY_DIR:STRING=${OpenH264_LIBRARY_DIR}
		-DX265_INCLUDE_DIR:STRING=${X265_INCLUDE_DIR}
		-DX265_LIBRARY_DIR:STRING=${X265_LIBRARY_DIR}
		-DOpenHEVC_INCLUDE_DIR:STRING=${OpenHEVC_INCLUDE_DIR}
		-DOpenHEVC_LIBRARY_DIR:STRING=${OpenHEVC_LIBRARY_DIR}
		-DVP9_INCLUDE_DIR:STRING=${VP9_INCLUDE_DIR}
		-DVP9_LIBRARY_DIR:STRING=${VP9_LIBRARY_DIR}
		-DYASM_BINARY_DIR:STRING=${YASM_BINARY_DIR}
	#--Build step-----------------
	BUILD_ALWAYS 1
	#--Install step-----------------
	INSTALL_COMMAND ""
	DEPENDS ${OpenIGTLink_DEPENDENCIES}
	)