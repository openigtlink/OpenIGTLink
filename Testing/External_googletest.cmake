cmake_minimum_required(VERSION 2.8.2)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
IF(GTest_DIR)
  # x265 has been built already
  FIND_PACKAGE(GTest REQUIRED)

  MESSAGE(STATUS "Using GTest available at: ${GTest_DIR}")

  SET(OpenIGTLink_GTest_DIR "${GTest_DIR}" CACHE INTERNAL "Path to store gtest binaries")
ELSE()
	SET (OpenIGTLink_GTest_DIR "${CMAKE_BINARY_DIR}/Deps/gtest-bin" CACHE INTERNAL "Path to store gtest binaries")
	ExternalProject_Add(GTest
		PREFIX "${CMAKE_BINARY_DIR}/Deps/gtest-prefix"
		SOURCE_DIR        "${CMAKE_BINARY_DIR}/Deps/gtest"
		BINARY_DIR        "${CMAKE_BINARY_DIR}/Deps/gtest-bin"
		#--Download step--------------
		URL https://github.com/google/googletest/archive/release-1.7.0.zip
		#--Configure step-------------
		CMAKE_ARGS
			${PLATFORM_SPECIFIC_ARGS}
			-DBUILD_SHARED_LIBS:BOOL=OFF # make the google test library a static build
			-DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
			-DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
		#--Build step-----------------
		BUILD_ALWAYS 1
		INSTALL_COMMAND ""
	)
ENDIF()

IF(GMock_DIR)
  # GMock has been built already
  FIND_PACKAGE(GMock REQUIRED)

  MESSAGE(STATUS "Using GTest available at: ${GMock_DIR}")

  SET(OpenIGTLink_GMock_DIR "${GMock_DIR}" CACHE INTERNAL "Path to store gmock binaries")
ELSE()  
	SET (OpenIGTLink_GMock_DIR "${CMAKE_BINARY_DIR}/Deps/gmock-bin" CACHE INTERNAL "Path to store gmock binaries")
	ExternalProject_Add(GMock
		PREFIX "${CMAKE_BINARY_DIR}/Deps/gmock-prefix"
		SOURCE_DIR        "${CMAKE_BINARY_DIR}/Deps/gmock"
		BINARY_DIR        "${CMAKE_BINARY_DIR}/Deps/gmock-bin"
		URL https://github.com/google/googlemock/archive/release-1.7.0.zip
		#--Configure step-------------
		CMAKE_ARGS
			${PLATFORM_SPECIFIC_ARGS}
			-DBUILD_SHARED_LIBS:BOOL=OFF # make the google test library a static build
			-DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
			-DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
		#--Build step-----------------
		BUILD_ALWAYS 1
		INSTALL_COMMAND ""
	)
ENDIF()
