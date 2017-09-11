# - The OPENHEVC library
# Once done this will define
#
#  Open - A list of search hints
#
#  OpenHEVC_FOUND - found OpenHEVC
#  OpenHEVC_INCLUDE_DIR - the OpenHEVC include directory
#  OpenHEVC_LIBRARY_DIR - OpenHEVC library directory

IF((EXISTS ${OpenHEVC_INCLUDE_DIR}) AND (EXISTS ${OpenHEVC_LIBRARY_DIR}))
	IF(EXISTS "${OpenHEVC_INCLUDE_DIR}/openHevcWrapper.h") 
		IF(WIN32)
		ELSE()
			IF(${CMAKE_BUILD_TYPE})
				IF(EXISTS "${OpenHEVC_LIBRARY_DIR}/${CMAKE_BUILD_TYPE}/libLibOpenHevcWrapper.a" )
					SET(OpenHEVC_Proj_BUILT "1")
				ENDIF()
			ELSE()
				IF(EXISTS "${OpenHEVC_LIBRARY_DIR}/libLibOpenHevcWrapper.a" )
					SET(OpenHEVC_Proj_BUILT "1")
				ENDIF()	
			ENDIF()
		ENDIF()		
	ENDIF()
ENDIF()

SET( OpenHEVC_PATH_HINTS 
		${OpenHEVC_ROOT} 
    ${OpenHEVC_INCLUDE_DIR}
    ${OpenHEVC_LIBRARY_DIR}
    )
    
unset(OpenHEVC_INCLUDE_DIR CACHE)
find_path(OpenHEVC_INCLUDE_DIR NAMES openHevcWrapper.h 
	PATH_SUFFIXES gpac/modules/openhevc_dec
	HINTS ${OpenHEVC_PATH_HINTS} 
	)
	
unset(OpenHEVC_LIBRARY_DIR CACHE)
find_path(OpenHEVC_LIBRARY_DIR
	 NAMES LibOpenHevcWrapper${CMAKE_STATIC_LIBRARY_SUFFIX} libLibOpenHevcWrapper.a
	 PATH_SUFFIXES ${CMAKE_BUILD_TYPE}
	 HINTS ${OpenHEVC_PATH_HINTS}
	 )
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenHEVC REQUIRED_VARS OpenHEVC_LIBRARY_DIR OpenHEVC_INCLUDE_DIR)

mark_as_advanced(OpenHEVC_INCLUDE_DIR OpenHEVC_LIBRARY_DIR)
