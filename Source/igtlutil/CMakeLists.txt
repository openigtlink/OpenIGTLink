PROJECT(igtlutil)

#-----------------------------------------------------------------------------
# Include directories
SET(igtlutil_INCLUDE_DIRS
  ${OpenIGTLink_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

IF(OpenIGTLink_BUILD_SHARED_LIBS)
  SET(igtlutil_BUILD_SHARED_LIBS ${OpenIGTLink_BUILD_SHARED_LIBS})
ENDIF()

IF(OpenIGTLink_BUILD_GENERATE_PIC)
  SET(igtlutil_BUILD_GENERATE_PIC ${OpenIGTLink_BUILD_GENERATE_PIC})
  SET(IGTLSTATIC 1)
ENDIF()

SET(igtlutil_HEADS
  igtl_header.h
  igtl_image.h
  igtl_position.h
  igtl_transform.h
  igtl_types.h
  igtl_util.h
  igtl_capability.h
  )

SET(igtlutil_SOURCES
  igtl_header.c
  igtl_image.c
  igtl_position.c
  igtl_transform.c
  igtl_util.c
  igtl_capability.c
  )

IF(${OpenIGTLink_PROTOCOL_VERSION} GREATER "1")
  SET(igtlutil_HEADS
    ${igtlutil_HEADS}
    igtl_colortable.h
    igtl_imgmeta.h
    igtl_lbmeta.h
    igtl_point.h
    igtl_tdata.h
    igtl_qtdata.h
    igtl_trajectory.h
    igtl_unit.h
    igtl_sensor.h
    igtl_string.h
    igtl_ndarray.h
    igtl_bind.h
    igtl_qtrans.h
    igtl_polydata.h
    )
  SET(igtlutil_SOURCES
    ${igtlutil_SOURCES}
    igtl_colortable.c
    igtl_imgmeta.c
    igtl_lbmeta.c
    igtl_point.c
    igtl_tdata.c
    igtl_qtdata.c
    igtl_trajectory.c
    igtl_unit.c
    igtl_sensor.c
    igtl_string.c
    igtl_ndarray.c
    igtl_bind.c
    igtl_qtrans.c
    igtl_polydata.c
    )
ENDIF()

IF(${OpenIGTLink_PROTOCOL_VERSION} GREATER "2")
  SET(igtlutil_HEADS
    ${igtlutil_HEADS}
    igtl_command.h
    )
  SET(igtlutil_SOURCES
    ${igtlutil_SOURCES}
    igtl_command.c
    )
ENDIF()

IF(MSVC OR ${CMAKE_GENERATOR} MATCHES "Xcode")
  ADD_LIBRARY(igtlutil
    ${igtlutil_SOURCES}
    ${igtlutil_HEADS}
    )
ELSE()
  ADD_LIBRARY(igtlutil
    ${igtlutil_SOURCES}
    )
ENDIF()

FOREACH(p IN LISTS igtlutil_INCLUDE_DIRS)
  target_include_directories(igtlutil PUBLIC $<BUILD_INTERFACE:${p}>)
ENDFOREACH()
target_include_directories(igtlutil PUBLIC $<INSTALL_INTERFACE:${igtlutil_INSTALL_INCLUDE_DIR}>)

IF(MSVC)
  target_compile_options(igtlutil PRIVATE /MP)
ENDIF()

SET_TARGET_PROPERTIES(igtlutil PROPERTIES
  VERSION ${OpenIGTLink_VERSION_MAJOR}.${OpenIGTLink_VERSION_MINOR}.${OpenIGTLink_VERSION_PATCH}
  SOVERSION ${OpenIGTLink_VERSION_MAJOR}
)

INSTALL(FILES ${igtlutil_HEADS}
  DESTINATION ${igtlutil_INSTALL_INCLUDE_DIR}
  COMPONENT Development)

INSTALL(TARGETS igtlutil EXPORT OpenIGTLink
  RUNTIME DESTINATION ${OpenIGTLink_INSTALL_BIN_DIR} COMPONENT RuntimeLibraries
  LIBRARY DESTINATION ${OpenIGTLink_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${OpenIGTLink_INSTALL_LIB_DIR} COMPONENT Development)