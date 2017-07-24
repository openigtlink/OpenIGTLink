PROJECT(OpenIGTLink)

CMAKE_MINIMUM_REQUIRED(VERSION 3.4)

INCLUDE(ExternalProject)

IF(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

OPTION(OpenIGTLink_SUPERBUILD "Superbuild or not." ON)
MARK_AS_ADVANCED(OpenIGTLink_SUPERBUILD)

#-----------------------------------------------------------------------------
# OpenIGTlink version number.
SET(OpenIGTLink_VERSION_MAJOR "3")
SET(OpenIGTLink_VERSION_MINOR "1")
SET(OpenIGTLink_VERSION_PATCH "0")

OPTION(OpenIGTLink_PROTOCOL_VERSION_2 "Build Library for Protocol Version 2" ON)
OPTION(OpenIGTLink_PROTOCOL_VERSION_3 "Build Library for Protocol Version 3" ON)

SET(OpenIGTLink_HEADER_VERSION "1")
IF(OpenIGTLink_PROTOCOL_VERSION_3)
  SET(OpenIGTLink_PROTOCOL_VERSION "3")
  SET(OpenIGTLink_HEADER_VERSION "2")
ELSEIF(OpenIGTLink_PROTOCOL_VERSION_2)
  SET(OpenIGTLink_PROTOCOL_VERSION "2")
ELSE()
  SET(OpenIGTLink_PROTOCOL_VERSION "1")
ENDIF()

#-----------------------------------------------------------------------------
# Configure VideoStreaming
OPTION(BUILD_VIDEOSTREAM "Build the Video Stream." OFF)
IF(BUILD_VIDEOSTREAM)
  OPTION(USE_H264 "Link the OpenH264 tree." OFF)
  OPTION(USE_VP9 "Link the VP9 tree." OFF)
  OPTION(USE_X265 "Link the X265 tree." OFF)
  OPTION(USE_OPENHEVC "Link the OpenHevc tree." OFF)
ENDIF()

#-----------------------------------------------------------------------------
# Configure WebSocket
OPTION(BUILD_WEBSOCKET "Build the Websocket." OFF)

#-----------------------------------------------------------------------------
# OpenIGTLink build configuration options.
OPTION(BUILD_SHARED_LIBS "Build OpenIGTLink with shared libraries." OFF)
SET(OpenIGTLink_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

IF(NOT OpenIGTLink_BUILD_SHARED_LIBS)
  OPTION(BUILD_GENERATE_PIC "Generate position independent code (for static)." ON)
  SET(OpenIGTLink_BUILD_GENERATE_PIC ${BUILD_GENERATE_PIC})
ENDIF()

OPTION(BUILD_EXAMPLES "Build OpenIGTLink example programs." OFF)
SET(OpenIGTLink_BUILD_EXAMPLES ${BUILD_EXAMPLES})

#-----------------------------------------------------------------------------
# Output directories.
IF(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OpenIGTLink_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")
ENDIF()
IF(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OpenIGTLink_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")
ENDIF()
IF(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OpenIGTLink_BINARY_DIR}/lib CACHE INTERNAL "Single output directory for building all archives.")
ENDIF()

SET(OpenIGTLink_LIBRARY_PATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
SET(OpenIGTLink_EXECUTABLE_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

SET(OpenIGTLink_CMAKE_DIR "${OpenIGTLink_SOURCE_DIR}/CMake")
LIST(APPEND CMAKE_MODULE_PATH "${OpenIGTLink_SOURCE_DIR}/CMake/Modules")

#-----------------------------------------------------------------------------
# Installation vars.
# OpenIGTLink_INSTALL_BIN_DIR          - binary dir (executables)
# OpenIGTLink_INSTALL_LIB_DIR          - library dir (libs)
# OpenIGTLink_INSTALL_DATA_DIR         - share dir (say, examples, data, etc)
# OpenIGTLink_INSTALL_INCLUDE_DIR      - include dir (headers)
# OpenIGTLink_INSTALL_PACKAGE_DIR      - package/export configuration files
# OpenIGTLink_INSTALL_NO_DEVELOPMENT   - do not install development files
# OpenIGTLink_INSTALL_NO_RUNTIME       - do not install runtime files
# OpenIGTLink_INSTALL_NO_DOCUMENTATION - do not install documentation files

IF(NOT OpenIGTLink_INSTALL_BIN_DIR)
  SET(OpenIGTLink_INSTALL_BIN_DIR "bin")
ENDIF()

IF(NOT OpenIGTLink_INSTALL_LIB_DIR)
  SET(OpenIGTLink_INSTALL_LIB_DIR "lib/igtl")
ENDIF()

IF(NOT OpenIGTLink_INSTALL_DATA_DIR)
  SET(OpenIGTLink_INSTALL_DATA_DIR "share/igtl")
ENDIF()

IF(NOT OpenIGTLink_INSTALL_INCLUDE_DIR)
  SET(OpenIGTLink_INSTALL_INCLUDE_DIR "include/igtl")
ENDIF()

IF(NOT igtlutil_INSTALL_INCLUDE_DIR)
  SET(igtlutil_INSTALL_INCLUDE_DIR "include/igtl/igtlutil")
ENDIF()

IF(NOT OpenIGTLink_INSTALL_PACKAGE_DIR)
  SET(OpenIGTLink_INSTALL_PACKAGE_DIR "${OpenIGTLink_INSTALL_LIB_DIR}/cmake/igtl-${OpenIGTLink_VERSION_MAJOR}.${OpenIGTLink_VERSION_MINOR}"
    CACHE INTERNAL "")
ENDIF()

IF(NOT OpenIGTLink_INSTALL_NO_DOCUMENTATION)
  SET(OpenIGTLink_INSTALL_NO_DOCUMENTATION 0)
ENDIF()

SET(OpenIGTLink_DIR "${CMAKE_BINARY_DIR}" CACHE INTERNAL "OpenIGTLink dir to be used by subprojects")

#-----------------------------------------------------------------------------
# OpenIGTLink requires special compiler flags on some platforms.
INCLUDE(FindThreads)

IF(CMAKE_COMPILER_IS_GNUCXX)
  # '-fno-tree-vectorize' option is added to avoid segmentation fault that occurs in
  # igtl_image_convert_byte_order() (Source/igtlutil/igtl_image.c) built with 'Release'
  # option on 64-bit Linux.
  INCLUDE(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG("-fno-tree-vectorize" OpenIGTLink_GNUCXX_TREE_VECTORIZE_SUPPORT)
  IF(OpenIGTLink_GNUCXX_TREE_VECTORIZE_SUPPORT)
    LIST(APPEND OpenIGTLink_REQUIRED_C_FLAGS -w -fno-tree-vectorize)
    LIST(APPEND OpenIGTLink_REQUIRED_CXX_FLAGS -ftemplate-depth-50 -fno-tree-vectorize)
  ENDIF()

  # If the library is built as a static library, pass -fPIC option to the compiler
  IF(OpenIGTLink_BUILD_GENERATE_PIC)
    LIST(APPEND OpenIGTLink_REQUIRED_C_FLAGS -fPIC)
    LIST(APPEND OpenIGTLink_REQUIRED_CXX_FLAGS -fPIC)
  ENDIF()

  # pthread
  IF(CMAKE_HAVE_THREADS_LIBRARY)
    LIST(APPEND OpenIGTLink_REQUIRED_LINK_FLAGS ${CMAKE_THREAD_LIBS_INIT})
  ENDIF()
ENDIF()


# for the gnu compiler a -D_PTHREADS is needed on sun
# for the native compiler a -mt flag is needed on the sun
IF(CMAKE_USE_PTHREADS)
  IF(CMAKE_SYSTEM MATCHES "SunOS.*")
    IF(CMAKE_COMPILER_IS_GNUCXX)
      LIST(APPEND OpenIGTLink_REQUIRED_CXX_FLAGS -D_PTHREADS)
    ELSE()
      LIST(APPEND OpenIGTLink_REQUIRED_CXX_FLAGS -mt)
      LIST(APPEND OpenIGTLink_REQUIRED_C_FLAGS -mt)
    ENDIF()
  ENDIF()
ENDIF()

#-----------------------------------------------------------------------------
# Run superbuild script instead of library script
IF(OpenIGTLink_SUPERBUILD)
  INCLUDE(SuperBuild/Superbuild.cmake)
  RETURN()
ENDIF()

#-----------------------------------------------------------------------------
# Platform configuration tests.
INCLUDE(${OpenIGTLink_CMAKE_DIR}/OpenIGTLinkCMakeTests.cmake)
SET(OpenIGTLink_LIBRARY_TARGETS_FILE "${CMAKE_BINARY_DIR}/OpenIGTLinkTargets.cmake")

#-----------------------------------------------------------------------------
# Generate Configuration Header files.
INCLUDE(${OpenIGTLink_SOURCE_DIR}/CMake/OpenIGTLinkConfigPlatform.cmake)

CONFIGURE_FILE(${OpenIGTLink_SOURCE_DIR}/igtlConfigure.h.in
  ${OpenIGTLink_BINARY_DIR}/igtlConfigure.h)

CONFIGURE_FILE(${OpenIGTLink_SOURCE_DIR}/igtl_typeconfig.h.in
  ${OpenIGTLink_BINARY_DIR}/igtl_typeconfig.h)

CONFIGURE_FILE(${OpenIGTLink_SOURCE_DIR}/igtlTypeConfig.h.in
  ${OpenIGTLink_BINARY_DIR}/igtlTypeConfig.h)

#-----------------------------------------------------------------------------
# Help other projects use OpenIGTLink.

# Copy the UseOpenIGTLink.cmake file to the binary tree for backward compatability.
CONFIGURE_FILE(${OpenIGTLink_SOURCE_DIR}/UseOpenIGTLink.cmake.in
  ${OpenIGTLink_BINARY_DIR}/UseOpenIGTLink.cmake COPYONLY IMMEDIATE)

# Create the OpenIGTLinkConfig.cmake file containing the OpenIGTLink configuration.
INCLUDE(${OpenIGTLink_SOURCE_DIR}/CMake/GenerateOpenIGTLinkConfig.cmake)

INSTALL(FILES
  ${OpenIGTLink_BINARY_DIR}/OpenIGTLinkBuildSettings.cmake
  ${OpenIGTLink_BINARY_DIR}/UseOpenIGTLink.cmake
  ${OpenIGTLink_BINARY_DIR}/Utilities/OpenIGTLinkConfig.cmake
  DESTINATION ${OpenIGTLink_INSTALL_PACKAGE_DIR}
  COMPONENT Development
  )

INSTALL(FILES
  ${OpenIGTLink_BINARY_DIR}/igtlConfigure.h
  DESTINATION ${OpenIGTLink_INSTALL_INCLUDE_DIR}
  COMPONENT Development
  )

INSTALL(FILES
  ${OpenIGTLink_BINARY_DIR}/igtl_typeconfig.h
  DESTINATION ${OpenIGTLink_INSTALL_INCLUDE_DIR}
  COMPONENT Development
  )

INSTALL(FILES
  ${OpenIGTLink_BINARY_DIR}/igtlTypeConfig.h
  DESTINATION ${OpenIGTLink_INSTALL_INCLUDE_DIR}
  COMPONENT Development
  )

#-----------------------------------------------------------------------------
# Add compiler flags OpenIGTLink needs to work on this platform
LIST(APPEND CMAKE_C_FLAGS ${OpenIGTLink_REQUIRED_C_FLAGS})
LIST(APPEND CMAKE_CXX_FLAGS ${OpenIGTLink_REQUIRED_CXX_FLAGS})
LIST(APPEND CMAKE_EXE_LINKER_FLAGS ${OpenIGTLink_REQUIRED_LINK_FLAGS})
LIST(APPEND CMAKE_SHARED_LINKER_FLAGS ${OpenIGTLink_REQUIRED_LINK_FLAGS})
LIST(APPEND CMAKE_MODULE_LINKER_FLAGS ${OpenIGTLink_REQUIRED_LINK_FLAGS})

#-----------------------------------------------------------------------------
# Configure Subdirectories
ADD_SUBDIRECTORY(Source/igtlutil)
ADD_SUBDIRECTORY(Source)
ADD_SUBDIRECTORY(Tools)

IF(OpenIGTLink_BUILD_EXAMPLES)
 ADD_SUBDIRECTORY(Examples)
ENDIF()

#-----------------------------------------------------------------------------
# Export targets
EXPORT(TARGETS OpenIGTLink
  FILE "${OpenIGTLink_LIBRARY_TARGETS_FILE}"
  )

# Export targets for install
INSTALL(EXPORT OpenIGTLink
  DESTINATION "${OpenIGTLink_INSTALL_PACKAGE_DIR}"
  FILE "OpenIGTLinkTargets.cmake"
  )

#-----------------------------------------------------------------------------
# Tests
OPTION(BUILD_TESTING "Build the testing tree." ON)
SET(OpenIGTLink_BUILD_TESTING ${BUILD_TESTING})
IF(OpenIGTLink_BUILD_TESTING)
  ENABLE_TESTING()
  ADD_SUBDIRECTORY(Testing)
  INCLUDE(${CMAKE_ROOT}/Modules/Dart.cmake)
ENDIF()

#-----------------------------------------------------------------------------
# Doxygen
OPTION(BUILD_DOCUMENTATION "Build OpenIGTLink Documentation" OFF)

IF(BUILD_DOCUMENTATION)
  ADD_SUBDIRECTORY(Documents/Doxygen)
ENDIF()