cmake_minimum_required(VERSION 2.8.2)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
SET(YASM_Proj_BUILT "0")
IF((EXISTS ${YASM_SOURCE_DIR}) AND (EXISTS ${YASM_LIBRARY_DIR}))
	IF(EXISTS "${YASM_LIBRARY_DIR}/${CMAKE_BUILD_TYPE}/yasm") 
		SET(YASM_Proj_BUILT "1")
	ENDIF()
ENDIF()
IF(YASM_Proj_BUILT EQUAL "1")
  # YASM has been built already
  MESSAGE(STATUS "Using YASM available at: ${YASM_LIBRARY_DIR}")
  #FIND_PACKAGE(yasm REQUIRED)
  #SET(YASM_LIBRARY_DIR "${yasm_DIR}" CACHE INTERNAL "Path to store yasm binaries")
ELSE()
  # yas has not been built yet, so download and build it as an external project
  SET(GIT_REPOSITORY "https://github.com/yasm/yasm.git")
  SET(GIT_TAG "master")
  IF(MSVC)
    LIST(APPEND PLATFORM_SPECIFIC_ARGS -DCMAKE_CXX_MP_FLAG:BOOL=ON)
  ENDIF()
  
  MESSAGE(STATUS "Downloading yasm ${GIT_TAG} from: ${GIT_REPOSITORY}")

  SET (YASM_SOURCE_DIR "${CMAKE_BINARY_DIR}/Deps/yasm" CACHE PATH "YASM source directory" FORCE)
  SET (YASM_LIBRARY_DIR "${CMAKE_BINARY_DIR}/Deps/yasm-bin" CACHE PATH "YASM library directory" FORCE)

  ExternalProject_Add( yasm
    PREFIX "${CMAKE_BINARY_DIR}/Deps/yasm-prefix"
    SOURCE_DIR "${YASM_SOURCE_DIR}"
    BINARY_DIR "${YASM_LIBRARY_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${GIT_REPOSITORY}"
    GIT_TAG ${GIT_TAG}
    #--Configure step-------------
    CMAKE_ARGS
      ${PLATFORM_SPECIFIC_ARGS}
      -DCMAKE_INSTALL_PREFIX:PATH="${CMAKE_BINARY_DIR}/Deps/yasm-install"
      -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:STRING=${YASM_LIBRARY_DIR}
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:STRING=${YASM_LIBRARY_DIR}
      -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${YASM_LIBRARY_DIR}
      -DBUILD_SHARED_LIBS:BOOL=${OpenIGTLink_BUILD_SHARED_LIBS} 
      -DBUILD_TESTING:BOOL=OFF 
      -DBUILD_EXAMPLES:BOOL=OFF
      -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
      -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
    #--Build step-----------------
    BUILD_ALWAYS 1
    INSTALL_COMMAND ""
    )
ENDIF()