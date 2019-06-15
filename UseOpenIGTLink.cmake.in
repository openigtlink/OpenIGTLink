#
# This file sets up include directories, link directories, and
# compiler settings for a project to use OpenIGTLink.  It should not be
# included directly, but rather through the OpenIGTLink_USE_FILE setting
# obtained from OpenIGTLinkConfig.cmake.
#

IF(OpenIGTLink_BUILD_SETTINGS_FILE AND NOT SKIP_OpenIGTLink_BUILD_SETTINGS_FILE)
  INCLUDE(${CMAKE_ROOT}/Modules/CMakeImportBuildSettings.cmake)
  CMAKE_IMPORT_BUILD_SETTINGS(${OpenIGTLink_BUILD_SETTINGS_FILE})
ENDIF()

# Add compiler flags needed to use OpenIGTLink.
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenIGTLink_REQUIRED_C_FLAGS}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenIGTLink_REQUIRED_CXX_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenIGTLink_REQUIRED_LINK_FLAGS}")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${OpenIGTLink_REQUIRED_LINK_FLAGS}")
SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${OpenIGTLink_REQUIRED_LINK_FLAGS}")

# Add include directories needed to use OpenIGTLink.
INCLUDE_DIRECTORIES(BEFORE ${OpenIGTLink_INCLUDE_DIRS})

# Load the OpenIGTLink targets
include(${OpenIGTLink_LIBRARY_TARGETS_FILE})

IF(OpenIGTLink_USE_WEBSOCKET)
  SET(CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}/websocketpp")
  find_package(websocketpp REQUIRED)
  INCLUDE(${OpenIGTLink_DIR}/UseWebSocket.cmake)
ENDIF()  