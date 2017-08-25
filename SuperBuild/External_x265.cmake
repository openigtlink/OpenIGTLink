cmake_minimum_required(VERSION 3.7.2)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
include(${OpenIGTLink_SOURCE_DIR}/SuperBuild/findX265.cmake)
IF(X265_FOUND)
  MESSAGE(STATUS "Using X265 available at: ${X265_INCLUDE_DIR}")
ELSE()
  # x265 has not been built yet, so download and build it as an external project
  SET(GIT_REPOSITORY "https://github.com/videolan/x265.git")
  SET(GIT_TAG "2.5")

  IF(MSVC)
    LIST(APPEND PLATFORM_SPECIFIC_ARGS -DCMAKE_CXX_MP_FLAG:BOOL=ON)
  ENDIF()
  
  SET(Generator ${CMAKE_GENERATOR})
	IF("${CMAKE_GENERATOR}" STREQUAL "Xcode")
	 SET(Generator "Unix Makefiles") # xcode generator tend to fail, use unix makefiles instead
	ENDIF()
  
  MESSAGE(STATUS "Downloading x265 ${GIT_TAG} from: ${GIT_REPOSITORY}")
  SET (X265_INCLUDE_DIR "${CMAKE_BINARY_DIR}/Deps/x265" CACHE PATH "x265 source directory" FORCE)
  SET (X265_LIBRARY_DIR "${CMAKE_BINARY_DIR}/Deps/x265-bin" CACHE PATH "x265 library directory" FORCE)
  ExternalProject_Add(X265
    PREFIX "${CMAKE_BINARY_DIR}/Deps/x265-prefix"
    SOURCE_DIR "${X265_INCLUDE_DIR}"
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
      -DENABLE_CLI:BOOL=OFF #${X265_SOURCE_DIR}/source
    #--Build step----------------- 
    BUILD_ALWAYS 1
    INSTALL_COMMAND ""
    DEPENDS YASM
    )
ENDIF()