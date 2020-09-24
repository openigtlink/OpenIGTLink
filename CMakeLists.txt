PROJECT(OpenIGTLink)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

INCLUDE(ExternalProject)

IF(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)
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
# Create the link to the VideoStreaming libraries
OPTION(OpenIGTLink_USE_H264 "Link the OpenH264 tree." OFF)
OPTION(OpenIGTLink_USE_VP9 "Link the VP9 tree." OFF)
OPTION(OpenIGTLink_USE_X265 "Link the X265 tree." OFF)
OPTION(OpenIGTLink_USE_OpenHEVC "Link the OpenHEVC tree." OFF)
OPTION(OpenIGTLink_USE_AV1 "Link the AV1 tree." OFF)

#-----------------------------------------------------------------------------
# Configure WebSocket
OPTION(OpenIGTLink_USE_WEBSOCKET "Build the Websocket." OFF)

#-----------------------------------------------------------------------------
# OpenIGTLink build configuration options.
OPTION(BUILD_SHARED_LIBS "Build OpenIGTLink with shared libraries." OFF)
SET(OpenIGTLink_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

IF(NOT OpenIGTLink_BUILD_SHARED_LIBS)
  OPTION(BUILD_GENERATE_PIC "Generate position independent code (for static)." ON)
  SET(OpenIGTLink_BUILD_GENERATE_PIC ${BUILD_GENERATE_PIC})
ENDIF()

#-----------------------------------------------------------------------------
if(NOT DEFINED OpenIGTLink_BUILD_DOCUMENTATION)
  option(BUILD_DOCUMENTATION "Build OpenIGTLink Documentation" OFF)
  set(OpenIGTLink_BUILD_DOCUMENTATION ${BUILD_DOCUMENTATION})
endif()

#-----------------------------------------------------------------------------
if(NOT DEFINED OpenIGTLink_BUILD_EXAMPLES)
  option(BUILD_EXAMPLES "Build OpenIGTLink example programs." OFF)
  set(OpenIGTLink_BUILD_EXAMPLES ${BUILD_EXAMPLES})
endif()

#-----------------------------------------------------------------------------
if(NOT DEFINED OpenIGTLink_BUILD_TESTING)
  option(BUILD_TESTING "Build the testing tree." ON)
  set(OpenIGTLink_BUILD_TESTING ${BUILD_TESTING})
endif()

#-----------------------------------------------------------------------------
if(NOT DEFINED OpenIGTLink_USE_GTEST)
  option(USE_GTEST "Use googletest for testing" ON)
  set(OpenIGTLink_USE_GTEST ${USE_GTEST})
endif()
#link to google test library tend to fail https://github.com/openigtlink/OpenIGTLink/issues/122
IF(OpenIGTLink_BUILD_SHARED_LIBS)
  SET(OpenIGTLink_USE_GTEST OFF)
ENDIF()

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

SET(OpenIGTLink_CMAKE_DIR "${OpenIGTLink_SOURCE_DIR}/CMake")
SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH} ${OpenIGTLink_SOURCE_DIR}/CMake/Modules")

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

SET(OpenIGTLink_DIR "${OpenIGTLink_BINARY_DIR}" CACHE INTERNAL "OpenIGTLink dir to be used by subprojects")

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
    SET(OpenIGTLink_REQUIRED_C_FLAGS "${OpenIGTLink_REQUIRED_C_FLAGS} -w -fno-tree-vectorize")
    SET(OpenIGTLink_REQUIRED_CXX_FLAGS "${OpenIGTLink_REQUIRED_CXX_FLAGS} -fno-tree-vectorize")
  ENDIF()

  # If the library is built as a static library, pass -fPIC option to the compiler
  IF(OpenIGTLink_BUILD_GENERATE_PIC)
    SET(OpenIGTLink_REQUIRED_C_FLAGS "${OpenIGTLink_REQUIRED_C_FLAGS} -fPIC")
    SET(OpenIGTLink_REQUIRED_CXX_FLAGS "${OpenIGTLink_REQUIRED_CXX_FLAGS} -fPIC")
  ENDIF()

  # pthread
  IF(CMAKE_HAVE_THREADS_LIBRARY)
    SET(OpenIGTLink_REQUIRED_LINK_FLAGS "${OpenIGTLink_REQUIRED_LINK_FLAGS} ${CMAKE_THREAD_LIBS_INIT}")
  ENDIF()
ENDIF()


# for the gnu compiler a -D_PTHREADS is needed on sun
# for the native compiler a -mt flag is needed on the sun
IF(CMAKE_USE_PTHREADS)
  IF(CMAKE_SYSTEM MATCHES "SunOS.*")
    IF(CMAKE_COMPILER_IS_GNUCXX)
      SET(OpenIGTLink_REQUIRED_CXX_FLAGS "${OpenIGTLink_REQUIRED_CXX_FLAGS} -D_PTHREADS")
    ELSE()
      SET(OpenIGTLink_REQUIRED_CXX_FLAGS "${OpenIGTLink_REQUIRED_CXX_FLAGS} -mt")
      SET(OpenIGTLink_REQUIRED_C_FLAGS "${OpenIGTLink_REQUIRED_C_FLAGS} -mt")
    ENDIF()
  ENDIF()
ENDIF()

OPTION(OpenIGTLink_ENABLE_VIDEOSTREAMING "Video stream feature activated." OFF)
IF(${OpenIGTLink_PROTOCOL_VERSION} GREATER "2")
  IF (OpenIGTLink_USE_H264 OR OpenIGTLink_USE_VP9 OR OpenIGTLink_USE_X265 OR OpenIGTLink_USE_OpenHEVC OR OpenIGTLink_USE_AV1)
    SET(OpenIGTLink_ENABLE_VIDEOSTREAMING  ON)
  ENDIF()
ENDIF()
#-----------------------------------------------------------------------------
# Run superbuild script instead of library script
IF (NOT (${CMAKE_VERSION} VERSION_LESS 3.4))
  #MESSAGE ("Cmake version is not less than 3.4, super build available")
  OPTION(OpenIGTLink_SUPERBUILD "Superbuild or not." OFF)
  IF(OpenIGTLink_SUPERBUILD)
    INCLUDE(SuperBuild/Superbuild.cmake)
    RETURN()
  ENDIF()
ELSE()
  message(WARNING "CMake >= 3.4.0 recommended to enable the superbuild option")
ENDIF()

#-----------------------------------------------------------------------------
# Add compiler flags OpenIGTLink needs to work on this platform
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenIGTLink_REQUIRED_C_FLAGS}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenIGTLink_REQUIRED_CXX_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenIGTLink_REQUIRED_LINK_FLAGS}")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${OpenIGTLink_REQUIRED_LINK_FLAGS}")
SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${OpenIGTLink_REQUIRED_LINK_FLAGS}")

#-----------------------------------------------------------------------------
# Configure Subdirectories
ADD_SUBDIRECTORY(Source/igtlutil)
ADD_SUBDIRECTORY(Source)
ADD_SUBDIRECTORY(Tools)

#-----------------------------------------------------------------------------
# Platform configuration tests.
INCLUDE(${OpenIGTLink_CMAKE_DIR}/OpenIGTLinkCMakeTests.cmake)
SET(OpenIGTLink_LIBRARY_TARGETS_FILE "${OpenIGTLink_BINARY_DIR}/OpenIGTLinkTargets.cmake")

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

# Create the OpenIGTLinkVersion.cmake file containing the OpenIGTLink version.
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${OpenIGTLink_BINARY_DIR}/OpenIGTLinkConfigVersion.cmake
  VERSION "${OpenIGTLink_VERSION_MAJOR}.${OpenIGTLink_VERSION_MINOR}.${OpenIGTLink_VERSION_PATCH}"
  COMPATIBILITY SameMajorVersion)

# Save the compiler settings so another project can import them.
# The functionality of this module has been dropped as of CMake 2.8.  It was
# deemed harmful (confusing users by changing their compiler). CMAKE_EXPORT_BUILD_SETTINGS should be removed
# INCLUDE(${CMAKE_ROOT}/Modules/CMakeExportBuildSettings.cmake)
# CMAKE_EXPORT_BUILD_SETTINGS(${OpenIGTLink_BINARY_DIR}/OpenIGTLinkBuildSettings.cmake)

INSTALL(FILES
  ${OpenIGTLink_BINARY_DIR}/UseOpenIGTLink.cmake
  ${OpenIGTLink_BINARY_DIR}/Utilities/OpenIGTLinkConfig.cmake
  ${OpenIGTLink_BINARY_DIR}/OpenIGTLinkConfigVersion.cmake
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
IF(OpenIGTLink_BUILD_TESTING)
  ENABLE_TESTING()
  ADD_SUBDIRECTORY(Testing)
  INCLUDE(${CMAKE_ROOT}/Modules/Dart.cmake)
ENDIF()

#-----------------------------------------------------------------------------
# Examples
IF(OpenIGTLink_BUILD_EXAMPLES)
 ADD_SUBDIRECTORY(Examples)
ENDIF()

#-----------------------------------------------------------------------------
# Build Doxygen documentation
IF(OpenIGTLink_BUILD_DOCUMENTATION)
  ADD_SUBDIRECTORY(Documents/Doxygen)
ENDIF()
