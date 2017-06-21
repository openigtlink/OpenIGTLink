IF(BUILD_WEBSOCKET)
  # Download and unpack Websocket at configure time
  configure_file(${OpenIGTLink_SOURCE_DIR}/SuperBuild/CMakeListsWebSocket.txt.in
  ${PROJECT_BINARY_DIR}/WebSocket-download/CMakeLists.txt)
	#Here the downloading project is triggered                                                               
	execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" . 
									WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/WebSocket-download" )
	execute_process(COMMAND "${CMAKE_COMMAND}" --build . 
									WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/WebSocket-download" ) 		

  SET(OpenIGTLink_INCLUDE_DIRS
    ${OpenIGTLink_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/Source/WebSocket
		${PROJECT_BINARY_DIR}/websocketpp
    ${PROJECT_BINARY_DIR}/websocketpp/websocketpp
  )
  SET(OpenIGTLink_SOURCES
    ${OpenIGTLink_SOURCES}
    ${PROJECT_SOURCE_DIR}/Source/WebSocket/igtlWebServerSocket.cxx
    ${PROJECT_SOURCE_DIR}/Source/WebSocket/igtlWebClientSocket.cxx
  )
  
  SET(OpenIGTLink_INCLUDE_FILES
    ${OpenIGTLink_INCLUDE_FILES}
    ${PROJECT_SOURCE_DIR}/Source/WebSocket/igtlWebServerSocket.h
    ${PROJECT_SOURCE_DIR}/Source/WebSocket/igtlWebClientSocket.h
  )
ENDIF()
