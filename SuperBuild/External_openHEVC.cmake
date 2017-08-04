cmake_minimum_required(VERSION 2.8.2)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
include(${OpenIGTLink_SOURCE_DIR}/SuperBuild/findOpenHEVC.cmake)
IF(OPENHEVC_FOUND)
  # openHEVC has been built already
  MESSAGE(STATUS "Using openHEVC available at: ${OPENHEVC_SOURCE_DIR}")
  #FIND_PACKAGE(openHEVC REQUIRED)
  #SET(OPENHEVC_LIBRARY_DIR "${openHEVC_DIR}" CACHE INTERNAL "Path to store openHEVC library")
ELSE()
  # openHEVC has not been built yet, so download and build it as an external project
  SET(GIT_REPOSITORY "https://github.com/OpenHEVC/openHEVC.git")
  SET(GIT_TAG "hevc_rext")

  IF(MSVC)
    LIST(APPEND PLATFORM_SPECIFIC_ARGS -DCMAKE_CXX_MP_FLAG:BOOL=ON)
  ENDIF()
  
  MESSAGE(STATUS "Downloading openHEVC ${GIT_TAG} from: ${GIT_REPOSITORY}")

  SET (OPENHEVC_SOURCE_DIR "${CMAKE_BINARY_DIR}/Deps/openHEVC" CACHE PATH "openHEVC source directory" FORCE)
  SET (OPENHEVC_LIBRARY_DIR "${CMAKE_BINARY_DIR}/Deps/openHEVC-bin" CACHE PATH "openHEVC library directory" FORCE)

  ExternalProject_Add( OPENHEVC
    PREFIX "${CMAKE_BINARY_DIR}/Deps/openHEVC-prefix"
    SOURCE_DIR "${OPENHEVC_SOURCE_DIR}"
    BINARY_DIR "${OPENHEVC_LIBRARY_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${GIT_REPOSITORY}"
    GIT_TAG ${GIT_TAG}
    #--Configure step-------------
    CMAKE_ARGS
      ${PLATFORM_SPECIFIC_ARGS}
      -DCMAKE_INSTALL_PREFIX:PATH="${CMAKE_BINARY_DIR}/Deps/openHEVC-install"
      -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:STRING=${OPENHEVC_LIBRARY_DIR}
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:STRING=${OPENHEVC_LIBRARY_DIR}
      -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${OPENHEVC_LIBRARY_DIR}
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DBUILD_SHARED_LIBS:BOOL=${OpenIGTLink_BUILD_SHARED_LIBS} 
      -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
      -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
    #--Build step-----------------
    BUILD_ALWAYS 1
    INSTALL_COMMAND ""
    DEPENDS YASM
    )
ENDIF()