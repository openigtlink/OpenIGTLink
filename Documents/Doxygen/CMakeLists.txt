#-- Add an Option to toggle the generation of the API documentation
if(BUILD_DOCUMENTATION)
  FIND_PACKAGE(Doxygen)
  if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is needed to build the documentation. Please install it correctly")
  endif()
  #-- Add a custom target to run Doxygen when ever the project is built
  add_custom_target (doc ALL 
			COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Documents/Doxygen
			SOURCES ${PROJECT_BINARY_DIR}/Documents/Doxygen)
  # IF you do NOT want the documentation to be generated EVERY time you build the project
  # then leave out the 'ALL' keyword from the above command.
endif()