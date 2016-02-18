ADD_SUBDIRECTORY( igtlutil )

find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})

# Enable ExternalProject CMake module
include(ExternalProject)
# Download and install GoogleTest
ExternalProject_Add(
    gtest
    URL https://googletest.googlecode.com/files/gtest-1.7.0.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
    # Disable install step
    INSTALL_COMMAND ""
)

# Create a libgtest target to be used as a dependency by test programs
add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest)

# Set gtest properties
ExternalProject_Get_Property(gtest source_dir binary_dir)
set_target_properties(libgtest PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/libgtest.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
#    "INTERFACE_INCLUDE_DIRECTORIES" "${source_dir}/include"
)
include_directories("${source_dir}/include")
message(${binary_dir})


ADD_EXECUTABLE(igtlSocketTest   igtlSocketTest.cxx)
ADD_EXECUTABLE(igtlMultiThreaderTest1   igtlMultiThreaderTest1.cxx)
ADD_EXECUTABLE(igtlMultiThreaderTest2   igtlMultiThreaderTest2.cxx)
ADD_EXECUTABLE(igtlMultiThreaderTest3   igtlMultiThreaderTest3.cxx)
ADD_EXECUTABLE(igtlMessageFactoryTest   igtlMessageFactoryTest.cxx)
ADD_EXECUTABLE(igtlTimeStampTest1   igtlTimeStampTest1.cxx)
ADD_EXECUTABLE(igtlMessageBaseTest   igtlMessageBaseTest.cxx)

TARGET_LINK_LIBRARIES(igtlSocketTest OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlMultiThreaderTest1 OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlMultiThreaderTest2 OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlMultiThreaderTest3 OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlMessageFactoryTest OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlTimeStampTest1 OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlMessageBaseTest OpenIGTLink libgtest)

ADD_TEST(igtlSocketTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlSocketTest)
ADD_TEST(igtlMultiThreaderTest1 ${OpenIGTLink_EXECUTABLE_PATH}/igtlMultiThreaderTest1)
ADD_TEST(igtlMultiThreaderTest2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlMultiThreaderTest2)
ADD_TEST(igtlMultiThreaderTest3 ${OpenIGTLink_EXECUTABLE_PATH}/igtlMultiThreaderTest3)
ADD_TEST(igtlMessageFactoryTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlMessageFactoryTest)
ADD_TEST(igtlTimeStampTest1 ${OpenIGTLink_EXECUTABLE_PATH}/igtlTimeStampTest1)
ADD_TEST(igtlMessageBaseTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlMessageBaseTest)



