cmake_minimum_required(VERSION 2.8.2)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
include(${OpenIGTLink_SOURCE_DIR}/SuperBuild/FindOpenHEVC.cmake)
INCLUDE(${OpenIGTLink_SOURCE_DIR}/SuperBuild/External_yasm.cmake)
SET(OpenHEVC_DEPENDENCIES)
IF(NOT YASM_FOUND)
  LIST(APPEND OpenHEVC_DEPENDENCIES YASM)
ENDIF()
IF(OpenHEVC_FOUND)
  # OpenHEVC has been built already
  MESSAGE(STATUS "Using OpenHEVC available at: ${OpenHEVC_INCLUDE_DIR}")
ELSE()
  # OpenHEVC has not been built yet, so download and build it as an external project
  SET(GIT_REPOSITORY "https://github.com/openigtlink/openHEVC.git")
  SET(GIT_TAG "master")

  IF(MSVC)
    LIST(APPEND PLATFORM_SPECIFIC_ARGS -DCMAKE_CXX_MP_FLAG:BOOL=ON)
  ENDIF()
  
  MESSAGE(STATUS "Downloading OpenHEVC ${GIT_TAG} from: ${GIT_REPOSITORY}")

  SET (OpenHEVC_INCLUDE_DIR "${CMAKE_BINARY_DIR}/Deps/OpenHEVC/gpac/modules/openhevc_dec" CACHE PATH "OpenHEVC source directory" FORCE)
  SET (OpenHEVC_LIBRARY_DIR "${CMAKE_BINARY_DIR}/Deps/OpenHEVC-bin")
  IF(WIN32) # for Windows
    SET(OpenHEVC_LIBRARY optimized ${OpenHEVC_LIBRARY_DIR}\\Release\\LibOpenHevcWrapper.lib debug ${OpenHEVC_LIBRARY_DIR}\\Debug\\LibOpenHevcWrapper.lib CACHE STRING "OpenHEVC library" FORCE)
  ELSE()
    IF (CMAKE_CONFIGURATION_TYPES) 
      SET(OpenHEVC_LIBRARY optimized ${OpenHEVC_LIBRARY_DIR}/Release/libLibOpenHevcWrapper.a debug ${OpenHEVC_LIBRARY_DIR}/Debug/libLibOpenHevcWrapper.a CACHE STRING "OpenHEVC library" FORCE)
    ELSE()
      SET(OpenHEVC_LIBRARY ${OpenHEVC_LIBRARY_DIR}/libLibOpenHevcWrapper.a)
    ENDIF()  
  ENDIF()
  ExternalProject_Add( OpenHEVC
    PREFIX "${CMAKE_BINARY_DIR}/Deps/OpenHEVC-prefix"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/Deps/OpenHEVC"
    BINARY_DIR "${OpenHEVC_LIBRARY_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${GIT_REPOSITORY}"
    GIT_TAG ${GIT_TAG}
    #--Configure step-------------
    CMAKE_ARGS
      ${PLATFORM_SPECIFIC_ARGS}
      -DCMAKE_INSTALL_PREFIX:PATH="${CMAKE_BINARY_DIR}/Deps/OpenHEVC-install"
      -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:STRING=${OpenHEVC_LIBRARY_DIR}
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:STRING=${OpenHEVC_LIBRARY_DIR}
      -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${OpenHEVC_LIBRARY_DIR}
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DBUILD_SHARED_LIBS:BOOL=${OpenIGTLink_BUILD_SHARED_LIBS} 
      -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
      -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
    #--Build step-----------------
    BUILD_ALWAYS 1
    INSTALL_COMMAND ""
    DEPENDS ${OpenHEVC_DEPENDENCIES}
    )
ENDIF()