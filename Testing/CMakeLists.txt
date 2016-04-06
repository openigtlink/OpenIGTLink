ADD_SUBDIRECTORY( igtlutil )

find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})
message(${CMAKE_SYSTEM_NAME})
if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND (NOT(CMAKE_CXX_FLAGS MATCHES "/MD")))
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}/MD" CACHE STRING "compile flags" FORCE)
 set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}/MDd" CACHE STRING "compile flags" FORCE)
endif(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND (NOT(CMAKE_CXX_FLAGS MATCHES "/MD")))
  # Download and unpack googletest at configure time
message(${CMAKE_CXX_FLAGS})
configure_file(CMakeLists.txt.in
               googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}".
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Testing/googletest-download )
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Testing/googletest-download )
# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
 
# Add googletest directly to our build. This adds
# the following targets: gtest, gtest_main, gmock
# and gmock_main
add_subdirectory(${CMAKE_BINARY_DIR}/Testing/gmock
                 ${CMAKE_BINARY_DIR}/Testing/gmock-build)				 

# The gtest/gmock targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
include_directories("${gtest_SOURCE_DIR}/include")
include_directories("${gmock_SOURCE_DIR}/include")

ADD_EXECUTABLE(igtlSocketTest   igtlSocketTest.cxx)
ADD_EXECUTABLE(igtlMultiThreaderTest1   igtlMultiThreaderTest1.cxx)
ADD_EXECUTABLE(igtlMultiThreaderTest2   igtlMultiThreaderTest2.cxx)
ADD_EXECUTABLE(igtlMultiThreaderTest3   igtlMultiThreaderTest3.cxx)
ADD_EXECUTABLE(igtlMessageFactoryTest   igtlMessageFactoryTest.cxx)
ADD_EXECUTABLE(igtlTimeStampTest1   igtlTimeStampTest1.cxx)
ADD_EXECUTABLE(igtlMessageBaseTest   igtlMessageBaseTest.cxx)
ADD_EXECUTABLE(igtlImageMessageTest   igtlImageMessageTest.cxx)
ADD_EXECUTABLE(igtlImageMessage2Test   igtlImageMessage2Test.cxx)
ADD_EXECUTABLE(igtlServerSocketTest   igtlServerSocketTest.cxx)
ADD_EXECUTABLE(igtlBindMessageTest   igtlBindMessageTest.cxx)
ADD_EXECUTABLE(igtlSocketTest   igtlSocketTest.cxx)
ADD_EXECUTABLE(igtlClientSocketTest   igtlClientSocketTest.cxx)

TARGET_LINK_LIBRARIES(igtlSocketTest OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlMultiThreaderTest1 OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlMultiThreaderTest2 OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlMultiThreaderTest3 OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlMessageFactoryTest OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlTimeStampTest1 OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlMessageBaseTest OpenIGTLink gtest_main gtest gmock_main gmock)
TARGET_LINK_LIBRARIES(igtlImageMessageTest OpenIGTLink gtest_main gtest gmock_main gmock)
TARGET_LINK_LIBRARIES(igtlImageMessage2Test OpenIGTLink gtest_main gtest gmock_main gmock)
TARGET_LINK_LIBRARIES(igtlServerSocketTest OpenIGTLink gtest_main gtest gmock_main gmock)
TARGET_LINK_LIBRARIES(igtlBindMessageTest OpenIGTLink gtest_main gtest gmock_main gmock)
TARGET_LINK_LIBRARIES(igtlSocketTest OpenIGTLink gtest_main gtest gmock_main gmock)
TARGET_LINK_LIBRARIES(igtlClientSocketTest OpenIGTLink gtest_main gtest gmock_main gmock)

ADD_TEST(igtlSocketTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlSocketTest)
ADD_TEST(igtlMultiThreaderTest1 ${OpenIGTLink_EXECUTABLE_PATH}/igtlMultiThreaderTest1)
ADD_TEST(igtlMultiThreaderTest2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlMultiThreaderTest2)
ADD_TEST(igtlMultiThreaderTest3 ${OpenIGTLink_EXECUTABLE_PATH}/igtlMultiThreaderTest3)
ADD_TEST(igtlMessageFactoryTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlMessageFactoryTest)
ADD_TEST(igtlTimeStampTest1 ${OpenIGTLink_EXECUTABLE_PATH}/igtlTimeStampTest1)
ADD_TEST(igtlMessageBaseTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlMessageBaseTest)
ADD_TEST(igtlImageMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlImageMessageTest)
ADD_TEST(igtlImageMessage2Test ${OpenIGTLink_EXECUTABLE_PATH}/igtlImageMessage2Test)
ADD_TEST(igtlServerSocketTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlServerSocketTest)
ADD_TEST(igtlBindMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlBindMessageTest)
ADD_TEST(igtlSocketTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlSocketTest)
ADD_TEST(igtlClientSocketTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlSocketTest)


