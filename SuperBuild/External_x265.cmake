IF(x265_DIR)
  # x265 has been built already
  FIND_PACKAGE(x265 REQUIRED)

  MESSAGE(STATUS "Using x265 available at: ${x265_DIR}")

  SET(OpenIGTLink_x265_DIR "${x265_DIR}" CACHE INTERNAL "Path to store x265 binaries")
ELSE()
  # x265 has not been built yet, so download and build it as an external project
  SET(GIT_REPOSITORY "https://github.com/videolan/x265.git")
  SET(GIT_TAG "2.5")

  IF(MSVC)
    LIST(APPEND PLATFORM_SPECIFIC_ARGS -DCMAKE_CXX_MP_FLAG:BOOL=ON)
  ENDIF()
  
  MESSAGE(STATUS "Downloading x265 ${GIT_TAG} from: ${GIT_REPOSITORY}")

  SET (OpenIGTLink_x265_SRC_DIR "${CMAKE_BINARY_DIR}/Deps/x265")
  SET (OpenIGTLink_x265_DIR "${CMAKE_BINARY_DIR}/Deps/x265-bin" CACHE INTERNAL "Path to store x265 binaries")

  ExternalProject_Add( x265
    PREFIX "${CMAKE_BINARY_DIR}/Deps/x265-prefix"
    SOURCE_DIR "${OpenIGTLink_x265_SRC_DIR}"
    SOURCE_SUBDIR source
    BINARY_DIR "${OpenIGTLink_x265_DIR}"
    #--Download step--------------
    GIT_REPOSITORY "${GIT_REPOSITORY}"
    GIT_TAG ${GIT_TAG}
    #--Configure step-------------
    CMAKE_ARGS
      ${PLATFORM_SPECIFIC_ARGS}
      -DCMAKE_INSTALL_PREFIX:PATH="${CMAKE_BINARY_DIR}/Deps/x265-install"
      -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:STRING=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:STRING=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
      -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
      -DBUILD_SHARED_LIBS:BOOL=${OpenIGTLink_BUILD_SHARED_LIBS} 
      -DBUILD_TESTING:BOOL=OFF 
      -DBUILD_EXAMPLES:BOOL=OFF
      -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
      -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
    #--Build step-----------------
    BUILD_ALWAYS 1
    )
ENDIF()