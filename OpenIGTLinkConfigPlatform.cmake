#-----------------------------------------------------------------------------
# Check platform and generate igtlConfigure.h
#-----------------------------------------------------------------------------

#
# The platform is determined by CMAKE_SYSTEM_NAME variable.
# CMake set CMAKE_SYSTEM_NAME based on "uname -s" on unix or just set "Windows"
# on windows.
# The list of "uname -s" is available in CMake/Modules/CMakeDetermineSystem.cmake
#


# Thread parameters
STRING(FIND ${CMAKE_SYSTEM_NAME} "Windows" _win_string_pos)
# Use pthread in default except Windows
if(NOT _win_string_pos EQUAL -1)
  SET(OpenIGTLink_USE_WIN32_THREADS 1)
else()
  SET(OpenIGTLink_USE_PTHREADS 1)  
endif()

# Windows
if(NOT _win_string_pos EQUAL -1)
  SET(OpenIGTLink_PLATFORM_WIN32 1)
endif()

# Mac OS X
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  SET(OpenIGTLink_PLATFORM_MACOSX 1)
endif()

# Linux
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  SET(OpenIGTLink_PLATFORM_LINUX 1)
endif()

# Sun OS
if(CMAKE_SYSTEM_NAME MATCHES "SunOS")
#  SET(OpenIGTLink_USE_SPROC 1)
  SET(OpenIGTLink_PLATFORM_SUNOS 1)
  # Set OpenIGTLink_STD_LINK_LIBRARIES 

  SET(OpenIGTLink_STD_LINK_LIBRARIES
    ${OpenIGTLink_STD_LINK_LIBRARIES}
    rt
    nsl
    socket
  )
  IF(CMAKE_COMPILER_IS_GNUCXX)
     SET(OpenIGTLink_STD_LINK_LIBRARIES
       ${OpenIGTLink_STD_LINK_LIBRARIES}
       stdc++
     )
  ELSE(CMAKE_COMPILER_IS_GNUCXX)
    #FIND_LIBRARY(OpenIGTLink_SUNCC_CSTD_LIBRARY Cstd /opt/SUNWspro/lib)
    #IF(OpenIGTLink_SUNCC_CSTD_LIBRARY)
    #   SET(OpenIGTLink_STD_LINK_LIBRARIES
    #     ${OpenIGTLink_STD_LINK_LIBRARIES}
    #     Cstd
    #   )
    #ENDIF(OpenIGTLink_SUNCC_CSTD_LIBRARY)
    #FIND_LIBRARY(OpenIGTLink_SUNCC_CRUN_LIBRARY Crun /opt/SUNWspro/lib)
    #IF(OpenIGTLink_SUNCC_CRUN_LIBRARY)
    #   SET(OpenIGTLink_STD_LINK_LIBRARIES
    #     ${OpenIGTLink_STD_LINK_LIBRARIES}
    #     Crun
    #   )
    #ENDIF(OpenIGTLink_SUNCC_CRUN_LIBRARY)
  ENDIF(CMAKE_COMPILER_IS_GNUCXX)
endif(CMAKE_SYSTEM_NAME MATCHES "SunOS")

# QNX
if(CMAKE_SYSTEM_NAME STREQUAL "QNX")
  SET(OpenIGTLink_PLATFORM_QNX 1)
endif()


#-----------------------------------------------------------------------------
# Type Check 
# 

include(CheckTypeSize)
check_type_size(int         CMAKE_SIZEOF_INT)
check_type_size(long        CMAKE_SIZEOF_LONG)
check_type_size("void*"     CMAKE_SIZEOF_VOID_P)
check_type_size(char        CMAKE_SIZEOF_CHAR)
check_type_size(short       CMAKE_SIZEOF_SHORT)
check_type_size(float       CMAKE_SIZEOF_FLOAT)
check_type_size(double      CMAKE_SIZEOF_DOUBLE)
check_type_size("long long" CMAKE_SIZEOF_LONG_LONG)
check_type_size("__int64"   CMAKE_SIZEOF___INT64)
check_type_size("int64_t"   CMAKE_SIZEOF_INT64_T)

#ADD_DEFINITIONS(-DIGTL_SIZEOF_CHAR=${CMAKE_SIZEOF_CHAR})
#ADD_DEFINITIONS(-DIGTL_SIZEOF_DOUBLE=${CMAKE_SIZEOF_DOUBLE})
#ADD_DEFINITIONS(-DIGTL_SIZEOF_FLOAT=${CMAKE_SIZEOF_FLOAT})
#ADD_DEFINITIONS(-DIGTL_SIZEOF_INT=${CMAKE_SIZEOF_INT})
#ADD_DEFINITIONS(-DIGTL_SIZEOF_LONG=${CMAKE_SIZEOF_LONG})
#ADD_DEFINITIONS(-DIGTL_SIZEOF_SHORT=${CMAKE_SIZEOF_SHORT})
#ADD_DEFINITIONS(-DIGTL_SIZEOF_FLOAT=${CMAKE_SIZEOF_FLOAT})
#ADD_DEFINITIONS(-DIGTL_SIZEOF_DOUBLE=${CMAKE_SIZEOF_DOUBLE})

#IF(CMAKE_SIZEOF_LONG_LONG)
#  ADD_DEFINITIONS(-DIGTL_TYPE_USE_LONG_LONG=1)
#  ADD_DEFINITIONS(-DIGTL_SIZEOF_LONG_LONG=${CMAKE_SIZEOF_LONG_LONG})
#ELSE(CMAKE_SIZEOF_LONG_LONG)
#  IF(CMAKE_SIZEOF___INT64)
#    ADD_DEFINITIONS(-DIGTL_TYPE_USE___INT64=1)
#    ADD_DEFINITIONS(-DIGTL_SIZEOF___INT64=${CMAKE_SIZEOF___INT64})
#  ELSE(CMAKE_SIZEOF___INT64)
#    IF(CMAKE_SIZEOF_INT64_T)
#      ADD_DEFINITIONS(-DIGTL_TYPE_USE_INT64_T=1)
#      ADD_DEFINITIONS(-DIGTL_SIZEOF_INT64_T=${CMAKE_SIZEOF_INT64_T})
#    ENDIF(CMAKE_SIZEOF_INT64_T)
#  ENDIF(CMAKE_SIZEOF___INT64)
#ENDIF(CMAKE_SIZEOF_LONG_LONG)