cmake_minimum_required(VERSION 2.8.2)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
include(${OpenIGTLink_SOURCE_DIR}/SuperBuild/FindAV1.cmake)
IF(AV1_FOUND)
  # AV1 has been built already
  MESSAGE(STATUS "Using AV1 available at: ${AV1_INCLUDE_DIR}")
ELSE()
  # AV1 has not been built yet, so download and build it as an external project
  SET(GIT_REPOSITORY "https://aomedia.googlesource.com/aom")
  SET(GIT_TAG "master")

  MESSAGE(STATUS "Downloading AV1 ${GIT_TAG} from: ${GIT_REPOSITORY}")

  IF(WIN32) # for Windows
    SET(AV1_LIBRARY optimized ${AV1_LIBRARY_DIR}\\Release\\aom.lib debug ${AV1_LIBRARY_DIR}\\Debug\\aom.lib CACHE STRING "AV1 library" FORCE)
  ELSE()
    IF(OpenIGTLink_BUILD_SHARED_LIBS)
      IF (CMAKE_CONFIGURATION_TYPES) 
        SET(AV1_LIBRARY optimized ${AV1_LIBRARY_DIR}/Release/libaom.so debug ${AV1_LIBRARY_DIR}/Debug/libaom.so CACHE STRING "AV1 library" FORCE)
      ELSE()
        SET(AV1_LIBRARY ${AV1_LIBRARY_DIR}/libaom.so CACHE STRING "AV1 library" FORCE)
      ENDIF()
    ELSE()    
      IF (CMAKE_CONFIGURATION_TYPES) 
        SET(AV1_LIBRARY optimized ${AV1_LIBRARY_DIR}/Release/libaom.a debug ${AV1_LIBRARY_DIR}/Debug/libaom.a CACHE STRING "AV1 library" FORCE)
      ELSE()
        SET(AV1_LIBRARY ${AV1_LIBRARY_DIR}/libaom.a CACHE STRING "AV1 library" FORCE)
      ENDIF()  
    ENDIF()
  ENDIF()
  SET (AV1_INCLUDE_DIR "${CMAKE_BINARY_DIR}/Deps/AV1/aom" CACHE PATH "AV1 source directory" FORCE)
  SET (AV1_LIBRARY_DIR "${CMAKE_BINARY_DIR}/Deps/AV1-bin" CACHE PATH "AV1 library directory" FORCE)
  ExternalProject_Add(AV1
    PREFIX "${CMAKE_BINARY_DIR}/Deps/AV1-prefix"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/Deps/AV1"
    BINARY_DIR "${AV1_LIBRARY_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${GIT_REPOSITORY}"
    GIT_TAG ${GIT_TAG}
    #--Configure step-------------
    CMAKE_ARGS
      ${PLATFORM_SPECIFIC_ARGS}
      -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}/Deps/AV1-install
      -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:STRING=${AV1_LIBRARY_DIR}
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:STRING=${AV1_LIBRARY_DIR}
      -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${AV1_LIBRARY_DIR}
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DBUILD_SHARED_LIBS:BOOL=${OpenIGTLink_BUILD_SHARED_LIBS}
      -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
      -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
      -DAOM_TARGET_CPU:STRING=generic
      -DENABLE_EXAMPLES:BOOL=OFF
      -DCONFIG_UNIT_TESTS=0
    #--Build step-----------------
    BUILD_ALWAYS 1
    INSTALL_COMMAND ""
    )
ENDIF()