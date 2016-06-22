# See if we need to link the socket library 
INCLUDE(CheckLibraryExists)
INCLUDE(CheckSymbolExists)
INCLUDE(CheckFunctionExists)

CHECK_LIBRARY_EXISTS("socket" getsockname "" OpenIGTLink_HAVE_LIBSOCKET)
IF( NOT OpenIGTLink_HAVE_GETSOCKNAME_WITH_SOCKLEN_T )
  IF(OpenIGTLink_HAVE_LIBSOCKET)
    SET(OpenIGTLink_GETSOCKNAME_LIBS "socket")
  ELSE()
    SET(OpenIGTLink_GETSOCKNAME_LIBS)
  ENDIF()

  MESSAGE(STATUS "Checking for getsockname with socklen_t")
  TRY_COMPILE(OpenIGTLink_HAVE_GETSOCKNAME_WITH_SOCKLEN_T
    ${OpenIGTLink_BINARY_DIR}/CMakeTmp/SocklenT
    ${OpenIGTLink_CMAKE_DIR}/igtlTestSocklenT.cxx
    CMAKE_FLAGS "-DLINK_LIBRARIES:STRING=${OpenIGTLink_GETSOCKNAME_LIBS}"
    OUTPUT_VARIABLE OUTPUT)

  IF(OpenIGTLink_HAVE_GETSOCKNAME_WITH_SOCKLEN_T)
    MESSAGE(STATUS "Checking for getsockname with socklen_t -- yes")
    SET(OpenIGTLink_HAVE_GETSOCKNAME_WITH_SOCKLEN_T 1 CACHE INTERNAL "Support for getsockname with socklen_t")
    WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeOutput.log
      "Determining if getsockname accepts socklen_t type  "
      "passed with the following output:\n"
      "${OUTPUT}\n" APPEND)
  ELSE()
    MESSAGE(STATUS "Checking for getsockname with socklen_t -- no")
    SET(OpenIGTLink_HAVE_GETSOCKNAME_WITH_SOCKLEN_T 0 CACHE INTERNAL "Support for getsockname with socklen_t")
    WRITE_FILE(${CMAKE_BINARY_DIR}/CMakeFiles/CMakeError.log
      "Determining if getsockname accepts socklen_t type  "
      "failed with the following output:\n"
      "${OUTPUT}\n" APPEND)
  ENDIF()
ENDIF()

# e.g. Mac OS X Snow Leopard does not have strnlen().
CHECK_FUNCTION_EXISTS(strnlen OpenIGTLink_HAVE_STRNLEN)

# e.g. IBM BlueGene/L doesn't have SO_REUSEADDR, because "setsockopt is not needed for
# BlueGene/L applications" according to the BlueGene/L Application Development handbook
CHECK_SYMBOL_EXISTS(SO_REUSEADDR "sys/types.h;sys/socket.h" OpenIGTLink_HAVE_SO_REUSEADDR)

SET(HAVE_SOCKETS TRUE)
# Cray Xt3/ Catamount doesn't have any socket support
# this could also be determined by doing something like
# check_symbol_exists(socket "sys/types.h;sys/socket.h" HAVE_SOCKETS)
IF(CMAKE_SYSTEM MATCHES Catamount)
  SET(HAVE_SOCKETS FALSE)
ENDIF(CMAKE_SYSTEM MATCHES Catamount)