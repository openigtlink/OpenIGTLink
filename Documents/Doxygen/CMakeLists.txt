#-- Add an Option to toggle the generation of the API documentation
if(BUILD_DOCUMENTATION)

  FIND_PACKAGE(Doxygen QUIET)
  if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is needed to build the documentation. Please install it correctly")
  endif()

  #IF (BUILD_DOXYGEN)
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
  #ENDIF (BUILD_DOXYGEN)

endif()
