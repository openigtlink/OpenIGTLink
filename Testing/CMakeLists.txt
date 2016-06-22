ADD_SUBDIRECTORY( igtlutil )

find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})

IF( OpenIGTLink_USE_GTEST )
  # TODO : gtest does not compile on Windows (tried VS2012, Windows 10)
  if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND (NOT(CMAKE_CXX_FLAGS MATCHES "/MD")))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MD" CACHE STRING "compile flags" FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd" CACHE STRING "compile flags" FORCE)
  endif()
 
  # Download and unpack googletest at configure time
  configure_file(CMakeLists.txt.in
                 googletest-download/CMakeLists.txt)
  execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" . 
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/Testing/googletest-download" )

  execute_process(COMMAND "${CMAKE_COMMAND}" --build . 
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/Testing/googletest-download" )

  # Prevent GoogleTest from overriding our compiler/linker options
  # when building with Visual Studio
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  # Add googletest directly to our build. This adds
  # the following targets: gtest, gtest_main, gmock
  # and gmock_main
  add_subdirectory("${CMAKE_BINARY_DIR}/Testing/gmock"
                   "${CMAKE_BINARY_DIR}/Testing/gmock-build" )

  # The gtest/gmock targets carry header search path
  # dependencies automatically when using CMake 2.8.11 or
  # later. Otherwise we have to add them here ourselves.
  include_directories("${gtest_SOURCE_DIR}/include")
  include_directories("${gmock_SOURCE_DIR}/include")
ENDIF()


ADD_EXECUTABLE(igtlMultiThreaderTest1   igtlMultiThreaderTest1.cxx)
ADD_EXECUTABLE(igtlMultiThreaderTest2   igtlMultiThreaderTest2.cxx)
ADD_EXECUTABLE(igtlMultiThreaderTest3   igtlMultiThreaderTest3.cxx)
ADD_EXECUTABLE(igtlMessageFactoryTest   igtlMessageFactoryTest.cxx)
ADD_EXECUTABLE(igtlTimeStampTest1   igtlTimeStampTest1.cxx)
IF( OpenIGTLink_USE_GTEST )
  ADD_EXECUTABLE(igtlSocketTest   igtlSocketTest.cxx)
  ADD_EXECUTABLE(igtlMessageBaseTest   igtlMessageBaseTest.cxx)
  ADD_EXECUTABLE(igtlImageMessageTest   igtlImageMessageTest.cxx)
  ADD_EXECUTABLE(igtlImageMessage2Test   igtlImageMessage2Test.cxx)
  ADD_EXECUTABLE(igtlServerSocketTest   igtlServerSocketTest.cxx)
  ADD_EXECUTABLE(igtlBindMessageTest   igtlBindMessageTest.cxx)
  ADD_EXECUTABLE(igtlSocketTest   igtlSocketTest.cxx)
  ADD_EXECUTABLE(igtlClientSocketTest   igtlClientSocketTest.cxx)
  ADD_EXECUTABLE(igtlCapabilityMessageTest   igtlCapabilityMessageTest.cxx)
  ADD_EXECUTABLE(igtlColorTableMessageTest   igtlColorTableMessageTest.cxx)
  ADD_EXECUTABLE(igtlConditionVariableTest   igtlConditionVariableTest.cxx)
  ADD_EXECUTABLE(igtlLabelMetaMessageTest   igtlLabelMetaMessageTest.cxx)
  ADD_EXECUTABLE(igtlNDArrayMessageTest   igtlNDArrayMessageTest.cxx)
  ADD_EXECUTABLE(igtlImageMetaMessageTest   igtlImageMetaMessageTest.cxx)
  ADD_EXECUTABLE(igtlPointMessageTest   igtlPointMessageTest.cxx)
  ADD_EXECUTABLE(igtlPolyDataMessageTest   igtlPolyDataMessageTest.cxx)
  ADD_EXECUTABLE(igtlSensorMessageTest   igtlSensorMessageTest.cxx)
  ADD_EXECUTABLE(igtlStatusMessageTest   igtlStatusMessageTest.cxx)
  ADD_EXECUTABLE(igtlStringMessageTest   igtlStringMessageTest.cxx)
  ADD_EXECUTABLE(igtlTrackingDataMessageTest   igtlTrackingDataMessageTest.cxx)
  ADD_EXECUTABLE(igtlTrajectoryMessageTest   igtlTrajectoryMessageTest.cxx)
  ADD_EXECUTABLE(igtlTransformMessageTest   igtlTransformMessageTest.cxx)
  ADD_EXECUTABLE(igtlPositionMessageTest   igtlPositionMessageTest.cxx)
ENDIF()


TARGET_LINK_LIBRARIES(igtlMultiThreaderTest1 OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlMultiThreaderTest2 OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlMultiThreaderTest3 OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlMessageFactoryTest OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlTimeStampTest1 OpenIGTLink)
IF( OpenIGTLink_USE_GTEST )
  TARGET_LINK_LIBRARIES(igtlSocketTest OpenIGTLink gtest_main gtest gmock_main gmock)
  TARGET_LINK_LIBRARIES(igtlMessageBaseTest OpenIGTLink gtest_main gtest gmock_main gmock)
  TARGET_LINK_LIBRARIES(igtlImageMessageTest OpenIGTLink gtest_main gtest gmock_main gmock)
  TARGET_LINK_LIBRARIES(igtlImageMessage2Test OpenIGTLink gtest_main gtest gmock_main gmock)
  TARGET_LINK_LIBRARIES(igtlServerSocketTest OpenIGTLink gtest_main gtest gmock_main gmock)
  TARGET_LINK_LIBRARIES(igtlBindMessageTest OpenIGTLink gtest_main gtest gmock_main gmock)
  TARGET_LINK_LIBRARIES(igtlSocketTest OpenIGTLink gtest_main gtest gmock_main gmock)
  TARGET_LINK_LIBRARIES(igtlClientSocketTest OpenIGTLink gtest_main gtest gmock_main gmock)
  TARGET_LINK_LIBRARIES(igtlCapabilityMessageTest OpenIGTLink gtest_main gtest gmock_main gmock)
  TARGET_LINK_LIBRARIES(igtlColorTableMessageTest OpenIGTLink gtest_main gtest gmock_main gmock)
  TARGET_LINK_LIBRARIES(igtlConditionVariableTest OpenIGTLink gtest_main gtest gmock_main gmock)
  TARGET_LINK_LIBRARIES(igtlLabelMetaMessageTest OpenIGTLink gtest_main gtest gmock_main gmock)
  TARGET_LINK_LIBRARIES(igtlNDArrayMessageTest OpenIGTLink gtest_main gtest gmock_main gmock)
  TARGET_LINK_LIBRARIES(igtlImageMetaMessageTest OpenIGTLink gtest_main gtest gmock_main gmock)
  TARGET_LINK_LIBRARIES(igtlPointMessageTest OpenIGTLink gtest_main gtest gmock_main gmock)
  TARGET_LINK_LIBRARIES(igtlPolyDataMessageTest OpenIGTLink gtest_main gtest gmock_main gmock)
  TARGET_LINK_LIBRARIES(igtlSensorMessageTest OpenIGTLink gtest_main gtest gmock_main gmock)
  TARGET_LINK_LIBRARIES(igtlStatusMessageTest OpenIGTLink gtest_main gtest gmock_main gmock)
  TARGET_LINK_LIBRARIES(igtlStringMessageTest OpenIGTLink gtest_main gtest gmock_main gmock)
  TARGET_LINK_LIBRARIES(igtlTrackingDataMessageTest OpenIGTLink gtest_main gtest gmock_main gmock)
  TARGET_LINK_LIBRARIES(igtlTrajectoryMessageTest OpenIGTLink gtest_main gtest gmock_main gmock)
  TARGET_LINK_LIBRARIES(igtlTransformMessageTest OpenIGTLink gtest_main gtest gmock_main gmock)
  TARGET_LINK_LIBRARIES(igtlPositionMessageTest OpenIGTLink gtest_main gtest gmock_main gmock)
ENDIF()

ADD_TEST(igtlMultiThreaderTest1 ${OpenIGTLink_EXECUTABLE_PATH}/igtlMultiThreaderTest1)
ADD_TEST(igtlMultiThreaderTest2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlMultiThreaderTest2)
ADD_TEST(igtlMultiThreaderTest3 ${OpenIGTLink_EXECUTABLE_PATH}/igtlMultiThreaderTest3)
ADD_TEST(igtlMessageFactoryTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlMessageFactoryTest)
ADD_TEST(igtlTimeStampTest1 ${OpenIGTLink_EXECUTABLE_PATH}/igtlTimeStampTest1)
IF( OpenIGTLink_USE_GTEST )
  ADD_TEST(igtlMessageBaseTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlMessageBaseTest)
  ADD_TEST(igtlImageMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlImageMessageTest)
  ADD_TEST(igtlImageMessage2Test ${OpenIGTLink_EXECUTABLE_PATH}/igtlImageMessage2Test)
  ADD_TEST(igtlBindMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlBindMessageTest)
  ADD_TEST(igtlCapabilityMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlCapabilityMessageTest)
  ADD_TEST(igtlColorTableMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlColorTableMessageTest)
  ADD_TEST(igtlConditionVariableTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlConditionVariableTest)
  ADD_TEST(igtlLabelMetaMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlLabelMetaMessageTest)
  ADD_TEST(igtlNDArrayMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlNDArrayMessageTest)
  ADD_TEST(igtlImageMetaMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlLabelMetaMessageTest)
  ADD_TEST(igtlPointMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlPointMessageTest)
  ADD_TEST(igtlPolyDataMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlPolyDataMessageTest)
  ADD_TEST(igtlSensorMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlSensorMessageTest)
  ADD_TEST(igtlStatusMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlStatusMessageTest)
  ADD_TEST(igtlStringMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlStringMessageTest)
  ADD_TEST(igtlTrackingDataMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlTrackingDataMessageTest)
  ADD_TEST(igtlTrajectoryMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlTrajectoryMessageTest)
  ADD_TEST(igtlTransformMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlTransformMessageTest)
  ADD_TEST(igtlPositionMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlPositionMessageTest)
ENDIF()