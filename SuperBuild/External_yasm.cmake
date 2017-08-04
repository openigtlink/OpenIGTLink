cmake_minimum_required(VERSION 2.8.2)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
include(${OpenIGTLink_SOURCE_DIR}/SuperBuild/findYASM.cmake)
IF(YASM_FOUND)
  # YASM has been built already
  MESSAGE(STATUS "Using YASM available at: ${YASM_BINARY_DIR}")
ELSE()
  # yas has not been built yet, so download and build it as an external project
  SET(GIT_REPOSITORY "https://github.com/yasm/yasm.git")
  SET(GIT_TAG "master")
  IF(MSVC)
    LIST(APPEND PLATFORM_SPECIFIC_ARGS -DCMAKE_CXX_MP_FLAG:BOOL=ON)
  ENDIF()
  
  MESSAGE(STATUS "Downloading yasm ${GIT_TAG} from: ${GIT_REPOSITORY}")

  SET (YASM_SOURCE_DIR "${CMAKE_BINARY_DIR}/Deps/yasm" CACHE PATH "YASM source directory" FORCE)
  SET (YASM_BINART_DIR "${CMAKE_BINARY_DIR}/Deps/yasm-bin" CACHE PATH "YASM library directory" FORCE)

  ExternalProject_Add( YASM
    PREFIX "${CMAKE_BINARY_DIR}/Deps/yasm-prefix"
    SOURCE_DIR "${YASM_SOURCE_DIR}"
    BINARY_DIR "${YASM_BINART_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${GIT_REPOSITORY}"
    GIT_TAG ${GIT_TAG}
    #--Configure step-------------
    CMAKE_ARGS
      ${PLATFORM_SPECIFIC_ARGS}
      -DCMAKE_INSTALL_PREFIX:PATH="${CMAKE_BINARY_DIR}/Deps/yasm-install"
      -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:STRING=${YASM_BINART_DIR}
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:STRING=${YASM_BINART_DIR}
      -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${YASM_BINART_DIR}
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