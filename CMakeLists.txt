PROJECT( OpenIGTLink )

cmake_minimum_required(VERSION 2.4)

if(COMMAND cmake_policy)
     cmake_policy(SET CMP0003 NEW)
     endif(COMMAND cmake_policy)

#-----------------------------------------------------------------------------
# OpenIGTlink version number.
SET(OpenIGTLink_VERSION_MAJOR "1")
SET(OpenIGTLink_VERSION_MINOR "10")
SET(OpenIGTLink_VERSION_PATCH "10")

option(OpenIGTLink_PROTOCOL_VERSION_2 "Build Library for Protocol Version 2" ON)

IF(OpenIGTLink_PROTOCOL_VERSION_2)
  SET(OpenIGTLink_PROTOCOL_VERSION "2")
ELSE(OpenIGTLink_PROTOCOL_VERSION_2)
  SET(OpenIGTLink_PROTOCOL_VERSION "1")
ENDIF(OpenIGTLink_PROTOCOL_VERSION_2)


#-----------------------------------------------------------------------------
# OpenIGTLink build configuration options.

OPTION(BUILD_SHARED_LIBS "Build OpenIGTLink with shared libraries." OFF)
SET(OpenIGTLink_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

if(NOT OpenIGTLink_BUILD_SHARED_LIBS)
  OPTION(BUILD_GENERATE_PIC "Generate position independent code (for static)." ON)
  SET(OpenIGTLink_BUILD_GENERATE_PIC ${BUILD_GENERATE_PIC})
endif(NOT OpenIGTLink_BUILD_SHARED_LIBS)

OPTION(BUILD_EXAMPLES "Build OpenIGTLink example programs." OFF)
SET(OpenIGTLink_BUILD_EXAMPLES ${BUILD_EXAMPLES})

OPTION(BUILD_TESTING "Build the testing tree." ON)
SET(OpenIGTLink_BUILD_TESTING ${BUILD_TESTING})
IF(OpenIGTLink_BUILD_TESTING)
  ENABLE_TESTING()
  INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
ENDIF()

#-----------------------------------------------------------------------------
# Output directories.

IF(NOT LIBRARY_OUTPUT_PATH)
  SET (LIBRARY_OUTPUT_PATH ${OpenIGTLink_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")
ENDIF(NOT LIBRARY_OUTPUT_PATH)
IF(NOT EXECUTABLE_OUTPUT_PATH)
  SET (EXECUTABLE_OUTPUT_PATH ${OpenIGTLink_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")
ENDIF(NOT EXECUTABLE_OUTPUT_PATH)
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

SET(OpenIGTLink_LIBRARY_PATH "${LIBRARY_OUTPUT_PATH}")
SET(OpenIGTLink_EXECUTABLE_PATH "${EXECUTABLE_OUTPUT_PATH}")

SET(OpenIGTLink_CMAKE_DIR "${OpenIGTLink_SOURCE_DIR}/CMake")
SET(CMAKE_MODULE_PATH "${OpenIGTLink_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})




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
ENDIF(NOT OpenIGTLink_INSTALL_BIN_DIR)

IF(NOT OpenIGTLink_INSTALL_LIB_DIR)
  SET(OpenIGTLink_INSTALL_LIB_DIR "lib/igtl")
ENDIF(NOT OpenIGTLink_INSTALL_LIB_DIR)

IF(NOT OpenIGTLink_INSTALL_DATA_DIR)
  SET(OpenIGTLink_INSTALL_DATA_DIR "share/igtl")
ENDIF(NOT OpenIGTLink_INSTALL_DATA_DIR)

IF(NOT OpenIGTLink_INSTALL_INCLUDE_DIR)
  SET(OpenIGTLink_INSTALL_INCLUDE_DIR "include/igtl")
ENDIF(NOT OpenIGTLink_INSTALL_INCLUDE_DIR)

IF(NOT OpenIGTLink_INSTALL_PACKAGE_DIR)
  SET(OpenIGTLink_INSTALL_PACKAGE_DIR ${OpenIGTLink_INSTALL_LIB_DIR}
    CACHE INTERNAL "")
ENDIF(NOT OpenIGTLink_INSTALL_PACKAGE_DIR)

IF(NOT OpenIGTLink_INSTALL_NO_DOCUMENTATION)
  SET(OpenIGTLink_INSTALL_NO_DOCUMENTATION 0)
ENDIF(NOT OpenIGTLink_INSTALL_NO_DOCUMENTATION)

SET(OpenIGTLink_DIR "${CMAKE_BINARY_DIR}" CACHE INTERNAL "OpenIGTLink dir to be used by subprojects")

#-----------------------------------------------------------------------------
# Include directories

SET(OpenIGTLink_INCLUDE_DIRS_BUILD_TREE ${OpenIGTLink_INCLUDE_DIRS_BUILD_TREE}
  ${OpenIGTLink_BINARY_DIR}
  ${OpenIGTLink_SOURCE_DIR}/Source
  ${OpenIGTLink_SOURCE_DIR}/Source/igtlutil
)

INCLUDE_DIRECTORIES(
  ${OpenIGTLink_INCLUDE_DIRS_BUILD_TREE}
  )

#SET(OpenIGTLink_LIBRARY_PATH  ${OpenIGTLink_LIBRARY_PATH}
#  ${OpenIGTLink_BINARY_DIR}/Source
#  ${OpenIGTLink_BINARY_DIR}/Source/igtlutil
#  )

#-----------------------------------------------------------------------------
# OpenIGTLink requires special compiler flags on some platforms.


INCLUDE (FindThreads)

IF(CMAKE_COMPILER_IS_GNUCXX)

  # '-fno-tree-vectorize' option is added to avoid segmentation fault that occurs in
  # igtl_image_convert_byte_order() (Source/igtlutil/igtl_image.c) built with 'Release'
  # option on 64-bit Linux. 
  INCLUDE (CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG("-fno-tree-vectorize" OpenIGTLink_GNUCXX_TREE_VECTORIZE_SUPPORT)
  IF (OpenIGTLink_GNUCXX_TREE_VECTORIZE_SUPPORT)
    SET(OpenIGTLink_REQUIRED_C_FLAGS "${OpenIGTLink_REQUIRED_C_FLAGS} -w -fno-tree-vectorize")
    SET(OpenIGTLink_REQUIRED_CXX_FLAGS "${OpenIGTLink_REQUIRED_CXX_FLAGS} -ftemplate-depth-50 -fno-tree-vectorize")
  ENDIF (OpenIGTLink_GNUCXX_TREE_VECTORIZE_SUPPORT)

  # If the library is built as a static library, pass -fPIC option to the compiler
  IF(OpenIGTLink_BUILD_GENERATE_PIC)
    SET(OpenIGTLink_REQUIRED_C_FLAGS "${OpenIGTLink_REQUIRED_C_FLAGS} -fPIC")
    SET(OpenIGTLink_REQUIRED_CXX_FLAGS "${OpenIGTLink_REQUIRED_CXX_FLAGS} -fPIC")
  ENDIF(OpenIGTLink_BUILD_GENERATE_PIC)

  # pthread
  IF(CMAKE_HAVE_THREADS_LIBRARY)
    SET(OpenIGTLink_REQUIRED_LINK_FLAGS "${OpenIGTLink_REQUIRED_LINK_FLAGS} ${CMAKE_THREAD_LIBS_INIT}")
  ENDIF(CMAKE_HAVE_THREADS_LIBRARY)


#  IF(APPLE)
#    # -no-cpp-precomp and -Wno-long-double were compiler flags present
#    # only in Apple's gcc and not in the FSF gcc. The flags are obsolete
#    # and totally removed in gcc 4.2 and later. I believe they are only
#    # needed with gcc 3.3 and earlier.
#    INCLUDE(${OpenIGTLink_SOURCE_DIR}/CMake/itkCheckCXXAcceptsFlags.cmake)
#    itkCHECK_CXX_ACCEPTS_FLAGS("-no-cpp-precomp" CXX_HAS_CPP_PRECOMP_FLAG)
#    IF(CXX_HAS_CPP_PRECOMP_FLAG)    
#      SET(OpenIGTLink_REQUIRED_C_FLAGS "${OpenIGTLink_REQUIRED_C_FLAGS} -no-cpp-precomp")
#      SET(OpenIGTLink_REQUIRED_CXX_FLAGS "${OpenIGTLink_REQUIRED_CXX_FLAGS} -no-cpp-precomp")
#    ENDIF(CXX_HAS_CPP_PRECOMP_FLAG)    
#    itkCHECK_CXX_ACCEPTS_FLAGS("-Wno-long-double" CXX_HAS_LONGDOUBLE_FLAG)
#    IF(CXX_HAS_LONGDOUBLE_FLAG)    
#      SET(OpenIGTLink_REQUIRED_C_FLAGS "${OpenIGTLink_REQUIRED_C_FLAGS} -Wno-long-double")
#      SET(OpenIGTLink_REQUIRED_CXX_FLAGS "${OpenIGTLink_REQUIRED_CXX_FLAGS} -Wno-long-double")
#    ENDIF(CXX_HAS_LONGDOUBLE_FLAG)    
#
#    OPTION(OpenIGTLink_USE_64BITS_APPLE_TRUNCATION_WARNING "Turn on warnings on 64bits to 32bits truncations." OFF)
#    MARK_AS_ADVANCED(OpenIGTLink_USE_64BITS_APPLE_TRUNCATION_WARNING)
#
#  ENDIF(APPLE)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


# for the gnu compiler a -D_PTHREADS is needed on sun
# for the native compiler a -mt flag is needed on the sun
IF(CMAKE_USE_PTHREADS)
  IF(CMAKE_SYSTEM MATCHES "SunOS.*")
    IF(CMAKE_COMPILER_IS_GNUCXX)
      SET(OpenIGTLink_REQUIRED_CXX_FLAGS "${OpenIGTLink_REQUIRED_CXX_FLAGS} -D_PTHREADS")
    ELSE(CMAKE_COMPILER_IS_GNUCXX)
      SET(OpenIGTLink_REQUIRED_CXX_FLAGS "${OpenIGTLink_REQUIRED_CXX_FLAGS} -mt")
      SET(OpenIGTLink_REQUIRED_C_FLAGS "${OpenIGTLink_REQUIRED_C_FLAGS} -mt")
    ENDIF(CMAKE_COMPILER_IS_GNUCXX)
  ENDIF(CMAKE_SYSTEM MATCHES "SunOS.*")
ENDIF(CMAKE_USE_PTHREADS)


#-----------------------------------------------------------------------------
# Platform configuration tests.

# Socket tests etc.
INCLUDE(${OpenIGTLink_CMAKE_DIR}/OpenIGTLinkCMakeTests.cmake)



#-----------------------------------------------------------------------------
# Generate Configuration Header files.

include (${OpenIGTLink_SOURCE_DIR}/OpenIGTLinkConfigPlatform.cmake)

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
INCLUDE (${OpenIGTLink_SOURCE_DIR}/GenerateOpenIGTLinkConfig.cmake)

# Save the compiler settings so another project can import them.
INCLUDE(${CMAKE_ROOT}/Modules/CMakeExportBuildSettings.cmake)
CMAKE_EXPORT_BUILD_SETTINGS(${OpenIGTLink_BINARY_DIR}/OpenIGTLinkBuildSettings.cmake)

# Save library dependencies.
#EXPORT_LIBRARY_DEPENDENCIES(${OpenIGTLink_BINARY_DIR}/OpenIGTLinkLibraryDepends.cmake)

INSTALL(FILES
  ${OpenIGTLink_BINARY_DIR}/OpenIGTLinkBuildSettings.cmake
#  ${OpenIGTLink_BINARY_DIR}/OpenIGTLinkLibraryDepends.cmake
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
# Add compiler flags OpenIGTLink needs to work on this platform.  This must be
# done AFTER the call to CMAKE_EXPORT_BUILD_SETTINGS.

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenIGTLink_REQUIRED_C_FLAGS}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenIGTLink_REQUIRED_CXX_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenIGTLink_REQUIRED_LINK_FLAGS}")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${OpenIGTLink_REQUIRED_LINK_FLAGS}")
SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${OpenIGTLink_REQUIRED_LINK_FLAGS}")


#-----------------------------------------------------------------------------
# Configure Subdirectories

ADD_SUBDIRECTORY( Source    )
ADD_SUBDIRECTORY( Tools     )

IF(OpenIGTLink_BUILD_TESTING)
  ADD_SUBDIRECTORY( Testing )
ENDIF(OpenIGTLink_BUILD_TESTING)

IF(OpenIGTLink_BUILD_EXAMPLES)
 ADD_SUBDIRECTORY( Examples )
ENDIF(OpenIGTLink_BUILD_EXAMPLES)



#-----------------------------------------------------------------------------
# Doxygen 

OPTION(BUILD_DOCUMENTATION "Build OpenIGTLink Documentation" OFF)

IF(BUILD_DOCUMENTATION)

  ADD_SUBDIRECTORY( Documents/Doxygen )
               
ENDIF(BUILD_DOCUMENTATION)


