IF(BUILD_WEBSOCKET)
  # Download and unpack Websocket at configure time
  configure_file(${OpenIGTLink_SOURCE_DIR}/SuperBuild/CMakeListsWebSocket.txt.in
  ${PROJECT_BINARY_DIR}/WebSocket-download/CMakeLists.txt)
	#Here the downloading project is triggered                                                               
	execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" . 
									WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/WebSocket-download" )
	execute_process(COMMAND "${CMAKE_COMMAND}" --build . 
									WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/WebSocket-download" ) 		
    
  set (Boost_FIND_REQUIRED TRUE)
  set (Boost_FIND_QUIETLY TRUE)
  set (Boost_DEBUG FALSE)
  set (Boost_USE_MULTITHREADED TRUE)
  set (Boost_ADDITIONAL_VERSIONS "1.39.0" "1.40.0" "1.41.0" "1.42.0" "1.43.0" "1.44.0" "1.46.1") # todo: someone who knows better spesify these!
  message("into Build WebSocket")
  set (WEBSOCKETPP_BOOST_LIBS system thread random)
  find_package (Boost 1.39.0 COMPONENTS "${WEBSOCKETPP_BOOST_LIBS}")
  if (Boost_FOUND)
    message("Boost found")
    # Boost is a project wide global dependency.
    include_directories (${Boost_INCLUDE_DIRS})
    link_directories (${Boost_LIBRARY_DIRS})
    message("${Boost_INCLUDE_DIRS}")
    message("${Boost_LIBRARIES}")
    foreach (include_dir ${Boost_INCLUDE_DIRS})

    endforeach ()
    message (STATUS "-- Library Directories")
    foreach (library_dir ${Boost_LIBRARY_DIRS})
    message (STATUS "       " ${library_dir})
    endforeach ()
    message (STATUS "-- Libraries")
    foreach (boost_lib ${Boost_LIBRARIES})
    message (STATUS "       " ${boost_lib})
    endforeach ()
    message ("")
  else ()
    message (FATAL_ERROR "Failed to find required dependency: boost")
  endif ()
  find_package(OpenSSL)
  find_package(ZLIB)
  SET(TEMP_LINK_LIBS
      ${LINK_LIBS}
      ${Boost_LIBRARIES}
  )
  SET(LINK_LIBS ${TEMP_LINK_LIBS})
  SET(LINK_LIBS ${TEMP_LINK_LIBS} PARENT_SCOPE) # update both locally and globally
  message(${Boost_LIBRARIES})
  SET(TEMP_OpenIGTLink_INCLUDE_DIRS
    ${OpenIGTLink_INCLUDE_DIRS}
    ${PROJECT_BINARY_DIR}/Source/WebSocket
    ${Boost_INCLUDE_DIRS}
  )
  SET(OpenIGTLink_INCLUDE_DIRS ${TEMP_OpenIGTLink_INCLUDE_DIRS})
  SET(OpenIGTLink_INCLUDE_DIRS ${TEMP_OpenIGTLink_INCLUDE_DIRS} PARENT_SCOPE) # update both locally and globally
  SET(TEMP_OpenIGTLink_SOURCES
    ${OpenIGTLink_SOURCES}
    ${PROJECT_SOURCE_DIR}/Source/WebSocket/igtlWebSocket.cxx
  )
  SET(OpenIGTLink_SOURCES ${TEMP_OpenIGTLink_SOURCES})
  SET(OpenIGTLink_SOURCES ${TEMP_OpenIGTLink_SOURCES} PARENT_SCOPE) # update both locally and globally
  
  SET(TEMP_OpenIGTLink_INCLUDE_FILES
    ${OpenIGTLink_INCLUDE_FILES}
    ${PROJECT_SOURCE_DIR}/Source/WebSocket/igtlWebSocket.h
  )
  SET(OpenIGTLink_INCLUDE_FILES ${TEMP_OpenIGTLink_INCLUDE_FILES})
  SET(OpenIGTLink_INCLUDE_FILES ${TEMP_OpenIGTLink_INCLUDE_FILES} PARENT_SCOPE) # update both locally and globally
  
  add_definitions (-D_WEBSOCKETPP_CPP11_STL_)
  if (ENABLE_CPP11)
    if (MSVC)
      add_definitions (-D_WEBSOCKETPP_CPP11_FUNCTIONAL_)
      add_definitions (-D_WEBSOCKETPP_CPP11_SYSTEM_ERROR_)
      add_definitions (-D_WEBSOCKETPP_CPP11_RANDOM_DEVICE_)
      add_definitions (-D_WEBSOCKETPP_CPP11_MEMORY_)
    else()
      add_definitions (-D_WEBSOCKETPP_CPP11_STL_)
    endif()
  endif ()
  #ADD_SUBDIRECTORY(${CMAKE_BINARY_DIR}/WebSocket/Source ${CMAKE_BINARY_DIR}/bin)
ENDIF()
