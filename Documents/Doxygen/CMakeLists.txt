#-- Add an Option to toggle the generation of the API documentation
if(BUILD_DOCUMENTATION)

  INCLUDE (${CMAKE_ROOT}/Modules/Documentation.cmake)

  FIND_PACKAGE(Doxygen QUIET)
  if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is needed to build the documentation. Please install it correctly")
  endif()

  ##-- Add a custom target to run Doxygen when ever the project is built
  #add_custom_target (doc ALL 
  #			COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Documents/Doxygen
  #			SOURCES ${PROJECT_BINARY_DIR}/Documents/Doxygen)

  IF (BUILD_DOXYGEN)

    #
    # Configure the script and the doxyfile, then add target
    #
    CONFIGURE_FILE(${OpenIGTLink_SOURCE_DIR}/Utilities/Doxygen/doxygen.config.in
                   ${OpenIGTLink_BINARY_DIR}/Utilities/Doxygen/doxygen.config)
  
    CONFIGURE_FILE(${OpenIGTLink_SOURCE_DIR}/Utilities/Doxygen/igtldoxygen.pl.in
                   ${OpenIGTLink_BINARY_DIR}/Utilities/Doxygen/igtldoxygen.pl)
  
    ADD_CUSTOM_TARGET(Documentation 
      COMMAND ${DOXYGEN_EXECUTABLE} ${OpenIGTLink_BINARY_DIR}/Utilities/Doxygen/doxygen.config
      MAIN_DEPENDENCY ${OpenIGTLink_BINARY_DIR}/Utilities/Doxygen/doxygen.config
      DEPENDS         ${OpenIGTLink_BINARY_DIR}/Utilities/Doxygen/igtldoxygen.pl
      WORKING_DIRECTORY ${OpenIGTLink_BINARY_DIR}/Utilities/Doxygen
      )
  ENDIF (BUILD_DOXYGEN)

endif()
