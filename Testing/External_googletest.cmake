cmake_minimum_required(VERSION 2.8.2)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
ExternalProject_Add(GTest
	PREFIX "${CMAKE_BINARY_DIR}/Deps/gtest-prefix"
  SOURCE_DIR        "${CMAKE_BINARY_DIR}/Deps/gtest"
  BINARY_DIR        "${CMAKE_BINARY_DIR}/Deps/gtest_DIR"
  #--Download step--------------
    URL https://github.com/google/googletest/archive/release-1.7.0.zip
  #--Configure step-------------
    CMAKE_ARGS
      ${PLATFORM_SPECIFIC_ARGS}
      -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:STRING=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:STRING=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
      -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
      -DBUILD_SHARED_LIBS:BOOL=OFF # make the google test library a static build
      -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
      -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
  #--Build step-----------------
  BUILD_ALWAYS 1
  INSTALL_COMMAND ""
)

ExternalProject_Add(GMock
	PREFIX "${CMAKE_BINARY_DIR}/Deps/gmock-prefix"
  SOURCE_DIR        "${CMAKE_BINARY_DIR}/Deps/gmock"
  BINARY_DIR        "${CMAKE_BINARY_DIR}/Deps/gmock_DIR"
  URL https://github.com/google/googlemock/archive/release-1.7.0.zip
  #--Configure step-------------
    CMAKE_ARGS
      ${PLATFORM_SPECIFIC_ARGS}
      -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:STRING=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:STRING=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
      -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
      -DBUILD_SHARED_LIBS:BOOL=OFF # make the google test library a static build
      -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
      -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
  #--Build step-----------------
  BUILD_ALWAYS 1
  INSTALL_COMMAND ""
)