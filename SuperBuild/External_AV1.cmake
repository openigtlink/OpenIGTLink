cmake_minimum_required(VERSION 2.8.2)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

#TODO: yasm required for x86 builds?

IF(AV1_FOUND)
  IF(${OpenIGTLink_PROTOCOL_VERSION} LESS 3)
    MESSAGE(FATAL_ERROR "Video streaming requires a build of OpenIGTLink with v3 support enabled. Please set the OpenIGTLink_PROTOCOL_VERSION_3 to true.")
  ENDIF()
ELSE()

  SET(GIT_REPOSITORY "https://aomedia.googlesource.com/aom")
  SET(GIT_TAG "master")

  # OpenIGTLink has not been built yet, so download and build it as an external project
  MESSAGE(STATUS "Downloading AV1 ${GIT_TAG} from: ${GIT_REPOSITORY}")
  SET (AV1_INCLUDE_DIR "${CMAKE_BINARY_DIR}/Deps/AV1/" CACHE PATH "AV1 source directory" FORCE)
  SET (AV1_LIBRARY_DIR "${CMAKE_BINARY_DIR}/Deps/AV1-bin" CACHE PATH "AV1 library directory" FORCE)
  SET (AV1_BINARY_DIR "${CMAKE_BINARY_DIR}/Deps/AV1-bin" CACHE PATH "AV1 library directory" FORCE)
  ExternalProject_Add( AV1
    PREFIX "${CMAKE_BINARY_DIR}/Deps/AV1-prefix"
    SOURCE_DIR "${AV1_INCLUDE_DIR}"
    BINARY_DIR "${AV1_BINARY_DIR}"
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
    #--Build step-----------------
    BUILD_ALWAYS 1
    INSTALL_COMMAND ""
    DEPENDS ${AV1_DEPENDENCIES}
    )
ENDIF()

IF(WIN32) # for Windows
  IF(OpenIGTLink_BUILD_SHARED_LIBS)
    message(FATAL_ERROR "Windows DLL builds not supported yet")
  ELSE()
    SET(AV1_LIBRARY
      optimized ${AV1_LIBRARY_DIR}/Release/aom.lib
      debug ${AV1_LIBRARY_DIR}/Debug/aom.lib
      )
  ENDIF()
ELSE() # for other platforms
  IF(OpenIGTLink_BUILD_SHARED_LIBS)
    SET(AV1_LIBRARY
      ${AV1_LIBRARY_DIR}/libaom.so
      )
  ELSE()
    SET(AV1_LIBRARY
      ${AV1_LIBRARY_DIR}/libaom.a
      )
  ENDIF()
ENDIF()
