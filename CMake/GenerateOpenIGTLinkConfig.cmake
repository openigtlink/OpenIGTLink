# Generate the OpenIGTLinkConfig.cmake file in the build tree.  Also configure
# one for installation.  The file tells external projects how to use
# OpenIGTLink.

#-----------------------------------------------------------------------------
# Settings specific to the build tree.

# Generate CMake lines that will define the OpenIGTLink_SOURCE_DIR in the OpenIGTLinkConfig.cmake.
# We want this to happen only in the OpenIGTLinkConfig.cmake of the build dir, not in the
# installed or relocatable one.
SET(OpenIGTLink_CONFIG_CODE "
# The OpenIGTLink source tree.
# For backward compatibility issues we still need to define this variable, although
# it is highly probable that it will cause more harm than being useful. 
# Use OpenIGTLink_INCLUDE_DIRS instead, since OpenIGTLink_SOURCE_DIR may point to non-existent directory
IF(NOT OpenIGTLink_LEGACY_REMOVE)
  SET(OpenIGTLink_SOURCE_DIR \"${OpenIGTLink_SOURCE_DIR}\")
ENDIF(NOT OpenIGTLink_LEGACY_REMOVE)"
)

# The "use" file.
SET(OpenIGTLink_USE_FILE ${OpenIGTLink_BINARY_DIR}/UseOpenIGTLink.cmake)

# The build settings file.
SET(OpenIGTLink_BUILD_SETTINGS_FILE ${OpenIGTLink_BINARY_DIR}/OpenIGTLinkBuildSettings.cmake)

# Library directory.
SET(OpenIGTLink_LIBRARY_DIRS_CONFIG ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# Determine the include directories needed.
SET(OpenIGTLink_INCLUDE_DIRS_CONFIG
  ${OpenIGTLink_INCLUDE_DIRS}
)

#-----------------------------------------------------------------------------
# Configure OpenIGTLinkConfig.cmake for the build tree.
CONFIGURE_FILE(${OpenIGTLink_SOURCE_DIR}/OpenIGTLinkConfig.cmake.in
               ${OpenIGTLink_BINARY_DIR}/OpenIGTLinkConfig.cmake @ONLY IMMEDIATE)

#-----------------------------------------------------------------------------
# Settings specific to the install tree.

# store old OpenIGTLink_LIBRARY_TARGETS_FILE
SET(OpenIGTLink_LIBRARY_TARGETS_FILE_BUILDTREE ${OpenIGTLink_LIBRARY_TARGETS_FILE})

# The library dependencies file.
SET(OpenIGTLink_LIBRARY_TARGETS_FILE "\${OpenIGTLink_INSTALL_PREFIX}/${OpenIGTLink_INSTALL_PACKAGE_DIR}/OpenIGTLinkTargets.cmake")

# The "use" file.
SET(OpenIGTLink_USE_FILE \${OpenIGTLink_INSTALL_PREFIX}/${OpenIGTLink_INSTALL_PACKAGE_DIR}/UseOpenIGTLink.cmake)

# The build settings file.
SET(OpenIGTLink_BUILD_SETTINGS_FILE \${OpenIGTLink_INSTALL_PREFIX}/${OpenIGTLink_INSTALL_PACKAGE_DIR}/OpenIGTLinkBuildSettings.cmake)

# Include directories.
SET(OpenIGTLink_INCLUDE_DIRS_CONFIG \${OpenIGTLink_INSTALL_PREFIX}/${OpenIGTLink_INSTALL_INCLUDE_DIR})
FOREACH(DIR ${OpenIGTLink_INCLUDE_RELATIVE_DIRS})
  LIST(APPEND OpenIGTLink_INCLUDE_DIRS_CONFIG \${OpenIGTLink_INSTALL_PREFIX}/${OpenIGTLink_INSTALL_INCLUDE_DIR}/${DIR})
ENDFOREACH(DIR)
IF(OpenIGTLink_INCLUDE_DIRS_SYSTEM)
  LIST(APPEND OpenIGTLink_INCLUDE_DIRS_CONFIG ${OpenIGTLink_INCLUDE_DIRS_SYSTEM})
ENDIF()

# Link directories.
SET(OpenIGTLink_LIBRARY_DIRS_CONFIG "\${OpenIGTLink_INSTALL_PREFIX}/${OpenIGTLink_INSTALL_LIB_DIR}")

#-----------------------------------------------------------------------------
# Configure OpenIGTLinkConfig.cmake for the install tree.

# Construct the proper number of GET_FILENAME_COMPONENT(... PATH)
# calls to compute the installation prefix.
STRING(REGEX REPLACE "/" ";" OpenIGTLink_INSTALL_PACKAGE_DIR_COUNT
  "${OpenIGTLink_INSTALL_PACKAGE_DIR}")
SET(OpenIGTLink_CONFIG_CODE "
# Compute the installation prefix from this OpenIGTLinkConfig.cmake file location.
GET_FILENAME_COMPONENT(OpenIGTLink_INSTALL_PREFIX \"\${CMAKE_CURRENT_LIST_FILE}\" PATH)")
FOREACH(p ${OpenIGTLink_INSTALL_PACKAGE_DIR_COUNT})
  SET(OpenIGTLink_CONFIG_CODE
    "${OpenIGTLink_CONFIG_CODE}\nGET_FILENAME_COMPONENT(OpenIGTLink_INSTALL_PREFIX \"\${OpenIGTLink_INSTALL_PREFIX}\" PATH)"
    )
ENDFOREACH(p)


CONFIGURE_FILE(${OpenIGTLink_SOURCE_DIR}/OpenIGTLinkConfig.cmake.in
               ${OpenIGTLink_BINARY_DIR}/Utilities/OpenIGTLinkConfig.cmake @ONLY IMMEDIATE)

# restore old OpenIGTLink_LIBRARY_TARGETS_FILE
SET(OpenIGTLink_LIBRARY_TARGETS_FILE ${OpenIGTLink_LIBRARY_TARGETS_FILE_BUILDTREE})

