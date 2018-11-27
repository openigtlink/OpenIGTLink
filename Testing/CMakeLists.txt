PROJECT( OpenIGTLinkTesting )

cmake_minimum_required(VERSION 2.8.11)
find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})
include_directories(${OpenIGTLink_INCLUDE_DIRS})
link_directories(${OpenIGTLink_LIBRARY_DIRS})
include_directories(${PROJECT_BINARY_DIR})
configure_file(${PROJECT_SOURCE_DIR}/igtlTestConfig.h.in ${PROJECT_BINARY_DIR}/igtlTestConfig.h)
ENABLE_TESTING()
ADD_SUBDIRECTORY( igtlutil )
IF(OpenIGTLink_USE_GTEST AND (NOT OpenIGTLink_BUILD_SHARED_LIBS))
	#-----------
	#download of GoogleTest
	if( MSVC ) # VS2012 doesn't support correctly the tuples yet
		add_definitions("-D_VARIADIC_MAX=10")
	endif()
	# Download and unpack googletest at configure time
	configure_file(GoogletestDownload.txt.in
								 googletest-download/CMakeLists.txt)
	execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" . 
		WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/googletest-download" )

	execute_process(COMMAND "${CMAKE_COMMAND}" --build . 
		WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/googletest-download" )

	# Prevent GoogleTest from overriding our compiler/linker options
	# when building with Visual Studio
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

	# Add googletest directly to our build. This adds
	# the following targets: gtest, gtest_main, gmock
	# and gmock_main
	add_subdirectory("${PROJECT_BINARY_DIR}/gmock"
									 "${PROJECT_BINARY_DIR}/gmock-build" )

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
ADD_EXECUTABLE(igtlMessageBaseTest   igtlMessageBaseTest.cxx)
ADD_EXECUTABLE(igtlConditionVariableTest   igtlConditionVariableTest.cxx)

ADD_EXECUTABLE(igtlImageMessageTest   igtlImageMessageTest.cxx)
ADD_EXECUTABLE(igtlImageMessage2Test   igtlImageMessage2Test.cxx)
ADD_EXECUTABLE(igtlTransformMessageTest   igtlTransformMessageTest.cxx)
ADD_EXECUTABLE(igtlPositionMessageTest   igtlPositionMessageTest.cxx)
ADD_EXECUTABLE(igtlStatusMessageTest   igtlStatusMessageTest.cxx)
ADD_EXECUTABLE(igtlCapabilityMessageTest   igtlCapabilityMessageTest.cxx)

#ADD_EXECUTABLE(igtlSocketTest         igtlSocketTest.cxx)
#ADD_EXECUTABLE(igtlServerSocketTest   igtlServerSocketTest.cxx)
#ADD_EXECUTABLE(igtlClientSocketTest   igtlClientSocketTest.cxx)

# Message Tests Added in Version 2
IF(${OpenIGTLink_PROTOCOL_VERSION} GREATER "1")
  ADD_EXECUTABLE(igtlBindMessageTest   igtlBindMessageTest.cxx)
  ADD_EXECUTABLE(igtlColorTableMessageTest   igtlColorTableMessageTest.cxx)
  ADD_EXECUTABLE(igtlLabelMetaMessageTest   igtlLabelMetaMessageTest.cxx)
  ADD_EXECUTABLE(igtlNDArrayMessageTest   igtlNDArrayMessageTest.cxx)
  ADD_EXECUTABLE(igtlImageMetaMessageTest   igtlImageMetaMessageTest.cxx)
  ADD_EXECUTABLE(igtlPointMessageTest   igtlPointMessageTest.cxx)
  ADD_EXECUTABLE(igtlPolyDataMessageTest   igtlPolyDataMessageTest.cxx)
  ADD_EXECUTABLE(igtlSensorMessageTest   igtlSensorMessageTest.cxx)
  ADD_EXECUTABLE(igtlStringMessageTest   igtlStringMessageTest.cxx)
  ADD_EXECUTABLE(igtlTrackingDataMessageTest   igtlTrackingDataMessageTest.cxx)
  ADD_EXECUTABLE(igtlTrajectoryMessageTest   igtlTrajectoryMessageTest.cxx)
ENDIF()

# Message Tests Added in Version 3
IF(${OpenIGTLink_PROTOCOL_VERSION} GREATER "2")
  ADD_EXECUTABLE(igtlCommandMessageTest   igtlCommandMessageTest.cxx)
  ADD_EXECUTABLE(igtlMessageRTPWrapperTest   igtlMessageRTPWrapperTest.cxx)
ENDIF()

IF(${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" AND (OpenIGTLink_USE_H264 OR OpenIGTLink_USE_VP9 OR (OpenIGTLink_USE_X265 AND OpenIGTLink_USE_OpenHEVC) OR OpenIGTLink_USE_AV1))
  ADD_EXECUTABLE(igtlVideoMessageTest   igtlVideoMessageTest.cxx)
  ADD_EXECUTABLE(igtlVideoMetaMessageTest   igtlVideoMetaMessageTest.cxx)
ENDIF()


IF(OpenIGTLink_USE_GTEST AND (NOT OpenIGTLink_BUILD_SHARED_LIBS))
  SET(GTEST_LINK OpenIGTLink gtest_main gtest gmock_main gmock)
ELSE()
  SET(GTEST_LINK OpenIGTLink)
ENDIF()

TARGET_LINK_LIBRARIES(igtlMultiThreaderTest1 OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlMultiThreaderTest2 OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlMultiThreaderTest3 OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlMessageFactoryTest OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlTimeStampTest1 OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlMessageBaseTest ${GTEST_LINK})
TARGET_LINK_LIBRARIES(igtlConditionVariableTest ${GTEST_LINK})

TARGET_LINK_LIBRARIES(igtlImageMessageTest ${GTEST_LINK})
TARGET_LINK_LIBRARIES(igtlImageMessage2Test ${GTEST_LINK})
TARGET_LINK_LIBRARIES(igtlCapabilityMessageTest ${GTEST_LINK})
TARGET_LINK_LIBRARIES(igtlStatusMessageTest ${GTEST_LINK})
TARGET_LINK_LIBRARIES(igtlTransformMessageTest ${GTEST_LINK})
TARGET_LINK_LIBRARIES(igtlPositionMessageTest ${GTEST_LINK})

# Message Tests Added in Version 2
IF(${OpenIGTLink_PROTOCOL_VERSION} GREATER "1")
  TARGET_LINK_LIBRARIES(igtlBindMessageTest ${GTEST_LINK})
  TARGET_LINK_LIBRARIES(igtlColorTableMessageTest ${GTEST_LINK})
  TARGET_LINK_LIBRARIES(igtlLabelMetaMessageTest ${GTEST_LINK})
  TARGET_LINK_LIBRARIES(igtlNDArrayMessageTest ${GTEST_LINK})
  TARGET_LINK_LIBRARIES(igtlImageMetaMessageTest ${GTEST_LINK})
  TARGET_LINK_LIBRARIES(igtlPointMessageTest ${GTEST_LINK})
  TARGET_LINK_LIBRARIES(igtlPolyDataMessageTest ${GTEST_LINK})
  TARGET_LINK_LIBRARIES(igtlSensorMessageTest ${GTEST_LINK})
  TARGET_LINK_LIBRARIES(igtlStringMessageTest ${GTEST_LINK})
  TARGET_LINK_LIBRARIES(igtlTrackingDataMessageTest ${GTEST_LINK})
  TARGET_LINK_LIBRARIES(igtlTrajectoryMessageTest ${GTEST_LINK})
ENDIF()

# Message Tests Added in Version 3
IF(${OpenIGTLink_PROTOCOL_VERSION} GREATER "2")
  TARGET_LINK_LIBRARIES(igtlCommandMessageTest ${GTEST_LINK})
  TARGET_LINK_LIBRARIES(igtlMessageRTPWrapperTest ${GTEST_LINK})
ENDIF()

IF(${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" AND (OpenIGTLink_USE_H264 OR OpenIGTLink_USE_VP9 OR (OpenIGTLink_USE_X265 AND OpenIGTLink_USE_OpenHEVC) OR OpenIGTLink_USE_AV1))
  TARGET_LINK_LIBRARIES(igtlVideoMessageTest ${GTEST_LINK})
  TARGET_LINK_LIBRARIES(igtlVideoMetaMessageTest ${GTEST_LINK})
ENDIF()


#TARGET_LINK_LIBRARIES(igtlSocketTest ${GTEST_LINK})
#TARGET_LINK_LIBRARIES(igtlClientSocketTest ${GTEST_LINK})
#TARGET_LINK_LIBRARIES(igtlServerSocketTest ${GTEST_LINK})

set(TestStringFormat1 "--gtest_filter=*.*FormatVersion1")

ADD_TEST(igtlMultiThreaderTest1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlMultiThreaderTest1)
ADD_TEST(igtlMultiThreaderTest2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlMultiThreaderTest2)
ADD_TEST(igtlMultiThreaderTest3 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlMultiThreaderTest3)
ADD_TEST(igtlMessageFactoryTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlMessageFactoryTest)
ADD_TEST(igtlTimeStampTest1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlTimeStampTest1)
ADD_TEST(igtlMessageBaseTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlMessageBaseTest)
ADD_TEST(igtlConditionVariableTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlConditionVariableTest ${TestStringFormat1})

ADD_TEST(igtlImageMessageTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlImageMessageTest)
ADD_TEST(igtlImageMessage2Test ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlImageMessage2Test)
ADD_TEST(igtlCapabilityMessageTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlCapabilityMessageTest)
ADD_TEST(igtlStatusMessageTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlStatusMessageTest)
ADD_TEST(igtlTransformMessageTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlTransformMessageTest ${TestStringFormat1})
ADD_TEST(igtlPositionMessageTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlPositionMessageTest ${TestStringFormat1})


# Message Tests Added in Version 2
IF(${OpenIGTLink_PROTOCOL_VERSION} GREATER "1")
  ADD_TEST(igtlBindMessageTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlBindMessageTest ${TestStringFormat1})
  ADD_TEST(igtlColorTableMessageTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlColorTableMessageTest ${TestStringFormat1})
  ADD_TEST(igtlLabelMetaMessageTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlLabelMetaMessageTest ${TestStringFormat1})
  ADD_TEST(igtlNDArrayMessageTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlNDArrayMessageTest ${TestStringFormat1})
  ADD_TEST(igtlImageMetaMessageTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlLabelMetaMessageTest ${TestStringFormat1})
  ADD_TEST(igtlPointMessageTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlPointMessageTest ${TestStringFormat1})
  ADD_TEST(igtlPolyDataMessageTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlPolyDataMessageTest ${TestStringFormat1})
  ADD_TEST(igtlSensorMessageTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlSensorMessageTest ${TestStringFormat1})
  ADD_TEST(igtlStringMessageTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlStringMessageTest ${TestStringFormat1})
  ADD_TEST(igtlTrackingDataMessageTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlTrackingDataMessageTest ${TestStringFormat1})
  ADD_TEST(igtlTrajectoryMessageTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlTrajectoryMessageTest ${TestStringFormat1})
ENDIF()

set(TestStringFormat2 "--gtest_filter=*.*FormatVersion2")
# Message Tests Added in Version 3
IF(${OpenIGTLink_PROTOCOL_VERSION} GREATER "2")
  ADD_TEST(igtlMessageBaseTestFormatVersion2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlMessageBaseTest ${TestStringFormat2})
  ADD_TEST(igtlImageMessageTestFormatVersion2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlImageMessageTest ${TestStringFormat2})
  ADD_TEST(igtlImageMessage2TestFormatVersion2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlImageMessage2Test ${TestStringFormat2})
  ADD_TEST(igtlBindMessageTestFormatVersion2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlBindMessageTest ${TestStringFormat2})
  ADD_TEST(igtlCapabilityMessageTestFormatVersion2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlCapabilityMessageTest ${TestStringFormat2})
  ADD_TEST(igtlColorTableMessageTestFormatVersion2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlColorTableMessageTest ${TestStringFormat2})
  ADD_TEST(igtlConditionVariableTestFormatVersion2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlConditionVariableTest ${TestStringFormat2})
  ADD_TEST(igtlLabelMetaMessageTestFormatVersion2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlLabelMetaMessageTest ${TestStringFormat2})
  ADD_TEST(igtlNDArrayMessageTestFormatVersion2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlNDArrayMessageTest ${TestStringFormat2})
  ADD_TEST(igtlImageMetaMessageTestFormatVersion2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlLabelMetaMessageTest ${TestStringFormat2})
  ADD_TEST(igtlPointMessageTestFormatVersion2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlPointMessageTest ${TestStringFormat2})
  ADD_TEST(igtlPolyDataMessageTestFormatVersion2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlPolyDataMessageTest ${TestStringFormat2})
  ADD_TEST(igtlSensorMessageTestFormatVersion2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlSensorMessageTest ${TestStringFormat2})
  ADD_TEST(igtlStatusMessageTestFormatVersion2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlStatusMessageTest ${TestStringFormat2})
  ADD_TEST(igtlStringMessageTestFormatVersion2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlStringMessageTest ${TestStringFormat2})
  ADD_TEST(igtlTrackingDataMessageTestFormatVersion2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlTrackingDataMessageTest ${TestStringFormat2})
  ADD_TEST(igtlTrajectoryMessageTestFormatVersion2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlTrajectoryMessageTest ${TestStringFormat2})
  ADD_TEST(igtlTransformMessageTestFormatVersion2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlTransformMessageTest ${TestStringFormat2})
  ADD_TEST(igtlPositionMessageTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlPositionMessageTest)
  ## Command message is available in Protocol version 3, however, we will test both header version 1 and 2
  ADD_TEST(igtlCommandMessageTestFormatVersion1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlCommandMessageTest ${TestStringFormat1})
  ADD_TEST(igtlCommandMessageTestFormatVersion2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlCommandMessageTest ${TestStringFormat2})
  ADD_TEST(igtlMessageRTPWrapperTestFormatVersion2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlMessageRTPWrapperTest ${TestStringFormat2})
ENDIF()

IF(${OpenIGTLink_PROTOCOL_VERSION} GREATER "2" AND (OpenIGTLink_USE_H264 OR OpenIGTLink_USE_VP9 OR (OpenIGTLink_USE_X265 AND OpenIGTLink_USE_OpenHEVC) OR OpenIGTLink_USE_AV1))
  ADD_TEST(igtlVideoMessageTestFormatVersion1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlVideoMessageTest ${TestStringFormat1})
  ADD_TEST(igtlVideoMessageTestFormatVersion2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlVideoMessageTest ${TestStringFormat2})
  ADD_TEST(igtlVideoMetaMessageTestFormatVersion2 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/igtlVideoMetaMessageTest ${TestStringFormat2})
ENDIF()
