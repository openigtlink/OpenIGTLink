cmake_minimum_required(VERSION 3.7.2)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
include(${OpenIGTLink_SOURCE_DIR}/SuperBuild/FindX265.cmake)

IF(X265_FOUND)
  MESSAGE(STATUS "Using X265 available at: ${X265_INCLUDE_DIR}")
ELSE()
  INCLUDE(${OpenIGTLink_SOURCE_DIR}/SuperBuild/External_yasm.cmake)
  SET(X265_DEPENDENCIES)
  IF(NOT YASM_FOUND)
    LIST(APPEND X265_DEPENDENCIES YASM)
  ENDIF()
  # x265 has not been built yet, so download and build it as an external project
  SET(GIT_REPOSITORY "https://github.com/videolan/x265.git")
  SET(GIT_TAG "2.5")

  IF(MSVC)
    LIST(APPEND PLATFORM_SPECIFIC_ARGS -DCMAKE_CXX_MP_FLAG:BOOL=ON)
  ENDIF()
  
  SET(Generator ${CMAKE_GENERATOR})
  IF("${CMAKE_GENERATOR}" STREQUAL "Xcode")
    MESSAGE(FATAL_ERROR "Xcode build tend to fail for x265, use unix makefiles instead. And link the source and library directory manually in the cmake advance settings. see the link for details: https://bitbucket.org/multicoreware/x265/issues/303/build-x265-version20-on-xcode")
    #SET(Generator "Unix Makefiles") # xcode generator tend to fail, use unix makefiles instead, https://bitbucket.org/multicoreware/x265/issues/303/build-x265-version20-on-xcode
  ELSE()
    MESSAGE(STATUS "Downloading x265 ${GIT_TAG} from: ${GIT_REPOSITORY}")
    SET (X265_INCLUDE_DIR "${CMAKE_BINARY_DIR}/Deps/x265/source" CACHE PATH "x265 source directory" FORCE)
    SET (X265_LIBRARY_DIR "${CMAKE_BINARY_DIR}/Deps/x265-bin" CACHE PATH "x265 library directory" FORCE)
    ExternalProject_Add(X265
      PREFIX "${CMAKE_BINARY_DIR}/Deps/x265-prefix"
      SOURCE_DIR "${CMAKE_BINARY_DIR}/Deps/x265"
      SOURCE_SUBDIR source   # SOURCE_SUBDIR is only available for cmake version later than 3.7.2 
      BINARY_DIR "${X265_LIBRARY_DIR}"
      #--Download step--------------
      GIT_REPOSITORY "${GIT_REPOSITORY}"
      GIT_TAG ${GIT_TAG}
      #--Configure step-------------
      CMAKE_ARGS # cd ${X265_LIBRARY_DIR} && ${CMAKE_COMMAND} 
        ${PLATFORM_SPECIFIC_ARGS}
        -DCMAKE_GENERATOR:STRING=${Generator}
        -DCMAKE_INSTALL_PREFIX:PATH="${CMAKE_BINARY_DIR}/Deps/x265-install"
        -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:STRING=${X265_LIBRARY_DIR}
        -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:STRING=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${X265_LIBRARY_DIR}
        -DBUILD_SHARED_LIBS:BOOL=${OpenIGTLink_BUILD_SHARED_LIBS} 
        -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
        -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
        -DEXPORT_C_API:BOOL=ON
        -DENABLE_CLI:BOOL=OFF #${X265_INCLUDE_DIR}/source
      #--Build step----------------- 
      BUILD_ALWAYS 1
      INSTALL_COMMAND ""
      DEPENDS ${X265_DEPENDENCIES}
      )
  ENDIF()    
ENDIF()