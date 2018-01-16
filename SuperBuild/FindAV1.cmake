# - The AV1 library
# Once done this will define
#
#  Open - A list of search hints
#
#  AV1_FOUND - found AV1
#  AV1_INCLUDE_DIR - the AV1 include directory
#  AV1_LIBRARY_DIR - AV1 library
SET( AV1_PATH_HINTS 
      ${AV1_ROOT} 
      ${AV1_INCLUDE_DIR}
      )
unset(AV1_INCLUDE_DIR CACHE)
find_path(AV1_INCLUDE_DIR NAMES aom.h aom_encoder.h aom_decoder.h 
	PATH_SUFFIXES aom
	HINTS ${AV1_PATH_HINTS} 
	)

if(NOT AV1_INCLUDE_DIR)
	MESSAGE(FATAL_ERROR "AV1 include files not found, specify the file path")
endif()  	

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows") 
  SET(AV1_PATH_HINTS 
      ${AV1_ROOT}
      ${AV1_LIBRARY_DIR}/Release 
      ${AV1_LIBRARY_DIR}/Debug 
      )
	IF(OpenIGTLink_BUILD_SHARED_LIBS)
		find_path(AV1_LIBRARY_DIRECT_DIR libaom.so
			 HINTS  ${AV1_PATH_HINTS}
			 )	 		 
		if(NOT AV1_LIBRARY_DIRECT_DIR)
			unset(AV1_LIBRARY_DIRECT_DIR  CACHE) # don't expose the AV1_LIBRARY_DIRECT_DIR to user, force the user to set the variable AV1_LIBRARY_DIR
			MESSAGE(FATAL_ERROR "AV1 library file not found, specify the path where the AV1 project was build, if AV1 was built in source, then set the library path the same as include path")
		else()
			add_library(AV1_lib SHARED IMPORTED GLOBAL)
			set_property(TARGET AV1_lib PROPERTY IMPORTED_LOCATION_RELEASE ${AV1_LIBRARY_DIR}/Release/libaom.so)
      set_property(TARGET AV1_lib PROPERTY IMPORTED_LOCATION_DEBUG ${AV1_LIBRARY_DIR}/Debug/libaom.so)
			unset(AV1_LIBRARY_DIRECT_DIR  CACHE)
		endif()
  ELSE()
  	find_path(AV1_LIBRARY_DIRECT_DIR libaom.a
			 HINTS  ${AV1_PATH_HINTS}
			 )	  		 
		if(NOT AV1_LIBRARY_DIRECT_DIR)
			unset(AV1_LIBRARY_DIRECT_DIR  CACHE) # don't expose the AV1_LIBRARY_DIRECT_DIR to user, force the user to set the variable AV1_LIBRARY_DIR
			MESSAGE(FATAL_ERROR "AV1 library file not found, specify the path where the AV1 project was build, if AV1 was built in source, then set the library path the same as include path")
		else()
			add_library(AV1_lib STATIC IMPORTED GLOBAL)
			set_property(TARGET AV1_lib PROPERTY IMPORTED_LOCATION_RELEASE ${AV1_LIBRARY_DIR}/Release/libaom.a)
      set_property(TARGET AV1_lib PROPERTY IMPORTED_LOCATION_DEBUG ${AV1_LIBRARY_DIR}/Debug/libaom.a)
			unset(AV1_LIBRARY_DIRECT_DIR  CACHE)
		endif()
  ENDIF()		
else()
  IF(OpenIGTLink_BUILD_SHARED_LIBS)
    message(FATAL_ERROR "Windows DLL builds not supported yet")
  ENDIF()
  
  set(AV1_LIBRARY_DIR "" CACHE PATH "")
  SET(AV1_PATH_HINTS 
      ${AV1_ROOT}
      ${AV1_LIBRARY_DIR}/Release 
      ${AV1_LIBRARY_DIR}/Debug 
      )
  find_path(AV1_LIBRARY_DIRECT_DIR aom.lib
     HINTS  ${AV1_PATH_HINTS}
     )	
  if(NOT AV1_LIBRARY_DIRECT_DIR)
    unset(AV1_LIBRARY_DIRECT_DIR  CACHE) # don't expose the AV1_LIBRARY_DIRECT_DIR to user, force the user to set the variable AV1_LIBRARY_DIR    
    MESSAGE(FATAL_ERROR "AV1 library file not found, specify the path where the AV1 project was build, if AV1 was built in source, then set the library path the same as include path")
  else()
    add_library(AV1_lib STATIC IMPORTED GLOBAL)
    set_property(TARGET AV1_lib PROPERTY IMPORTED_LOCATION_RELEASE ${AV1_LIBRARY_DIR}/Release/aom.lib)
    set_property(TARGET AV1_lib PROPERTY IMPORTED_LOCATION_DEBUG ${AV1_LIBRARY_DIR}/Debug/aom.lib)
    unset(AV1_LIBRARY_DIRECT_DIR  CACHE)  
  endif()
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(AV1 REQUIRED_VARS AV1_LIBRARY_DIR AV1_INCLUDE_DIR)
mark_as_advanced(AV1_INCLUDE_DIR AV1_LIBRARY_DIR)
