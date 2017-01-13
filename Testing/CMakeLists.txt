PROJECT( OpenIGTLinkTesting )

cmake_minimum_required(VERSION 2.9)
find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})
include_directories(${OpenIGTLink_INCLUDE_DIRS})
link_directories(${OpenIGTLink_LIBRARY_DIRS})
include_directories(${PROJECT_BINARY_DIR})
OPTION(USE_GTEST "Build the testing tree." ON)
IF(USE_GTEST)
	SET(OpenIGTLink_USE_GTEST "1")
ELSE()
  SET(OpenIGTLink_USE_GTEST "0")
ENDIF()	
configure_file(${PROJECT_SOURCE_DIR}/igtlTestConfig.h.in ${PROJECT_BINARY_DIR}/igtlTestConfig.h)
ENABLE_TESTING()
ADD_SUBDIRECTORY( igtlutil )
IF(OpenIGTLink_USE_GTEST)
	#-----------
	#download of GoogleTest
	if( MSVC ) # VS2012 doesn't support correctly the tuples yet
		add_definitions("-D_VARIADIC_MAX=10")
	endif()
	# Download and unpack googletest at configure time
	configure_file(CMakeLists.txt.in
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
#---------------
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
ENDIF()


IF(USE_GTEST)
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
ENDIF()


#TARGET_LINK_LIBRARIES(igtlSocketTest ${GTEST_LINK})
#TARGET_LINK_LIBRARIES(igtlClientSocketTest ${GTEST_LINK})
#TARGET_LINK_LIBRARIES(igtlServerSocketTest ${GTEST_LINK})

set(TestStringFormat1 "--gtest_filter=*.*FormatVersion1")

ADD_TEST(igtlMultiThreaderTest1 ${OpenIGTLink_EXECUTABLE_PATH}/igtlMultiThreaderTest1)
ADD_TEST(igtlMultiThreaderTest2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlMultiThreaderTest2)
ADD_TEST(igtlMultiThreaderTest3 ${OpenIGTLink_EXECUTABLE_PATH}/igtlMultiThreaderTest3)
ADD_TEST(igtlMessageFactoryTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlMessageFactoryTest)
ADD_TEST(igtlTimeStampTest1 ${OpenIGTLink_EXECUTABLE_PATH}/igtlTimeStampTest1)
ADD_TEST(igtlMessageBaseTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlMessageBaseTest ${TestStringFormat1})
ADD_TEST(igtlConditionVariableTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlConditionVariableTest ${TestStringFormat1})

ADD_TEST(igtlImageMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlImageMessageTest ${TestStringFormat1})
ADD_TEST(igtlImageMessage2Test ${OpenIGTLink_EXECUTABLE_PATH}/igtlImageMessage2Test ${TestStringFormat1})
ADD_TEST(igtlCapabilityMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlCapabilityMessageTest ${TestStringFormat1})
ADD_TEST(igtlStatusMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlStatusMessageTest ${TestStringFormat1})
ADD_TEST(igtlTransformMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlTransformMessageTest ${TestStringFormat1})
ADD_TEST(igtlPositionMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlPositionMessageTest ${TestStringFormat1})


# Message Tests Added in Version 2
IF(${OpenIGTLink_PROTOCOL_VERSION} GREATER "1")
  ADD_TEST(igtlBindMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlBindMessageTest ${TestStringFormat1})
  ADD_TEST(igtlColorTableMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlColorTableMessageTest ${TestStringFormat1})
  ADD_TEST(igtlLabelMetaMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlLabelMetaMessageTest ${TestStringFormat1})
  ADD_TEST(igtlNDArrayMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlNDArrayMessageTest ${TestStringFormat1})
  ADD_TEST(igtlImageMetaMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlLabelMetaMessageTest ${TestStringFormat1})
  ADD_TEST(igtlPointMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlPointMessageTest ${TestStringFormat1})
  ADD_TEST(igtlPolyDataMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlPolyDataMessageTest ${TestStringFormat1})
  ADD_TEST(igtlSensorMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlSensorMessageTest ${TestStringFormat1})
  ADD_TEST(igtlStringMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlStringMessageTest ${TestStringFormat1})
  ADD_TEST(igtlTrackingDataMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlTrackingDataMessageTest ${TestStringFormat1})
  ADD_TEST(igtlTrajectoryMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlTrajectoryMessageTest ${TestStringFormat1})
ENDIF()

# Message Tests Added in Version 3
IF(${OpenIGTLink_PROTOCOL_VERSION} GREATER "2")
  set(TestStringFormat2 "--gtest_filter=*.*FormatVersion2")
  ADD_TEST(igtlMessageBaseTestFormatVersion2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlMessageBaseTest ${TestStringFormat2})
  ADD_TEST(igtlImageMessageTestFormatVersion2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlImageMessageTest ${TestStringFormat2})
  ADD_TEST(igtlImageMessage2TestFormatVersion2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlImageMessage2Test ${TestStringFormat2})
  ADD_TEST(igtlBindMessageTestFormatVersion2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlBindMessageTest ${TestStringFormat2})
  ADD_TEST(igtlCapabilityMessageTestFormatVersion2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlCapabilityMessageTest ${TestStringFormat2})
  ADD_TEST(igtlColorTableMessageTestFormatVersion2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlColorTableMessageTest ${TestStringFormat2})
  ADD_TEST(igtlConditionVariableTestFormatVersion2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlConditionVariableTest ${TestStringFormat2})
  ADD_TEST(igtlLabelMetaMessageTestFormatVersion2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlLabelMetaMessageTest ${TestStringFormat2})
  ADD_TEST(igtlNDArrayMessageTestFormatVersion2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlNDArrayMessageTest ${TestStringFormat2})
  ADD_TEST(igtlImageMetaMessageTestFormatVersion2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlLabelMetaMessageTest ${TestStringFormat2})
  ADD_TEST(igtlPointMessageTestFormatVersion2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlPointMessageTest ${TestStringFormat2})
  ADD_TEST(igtlPolyDataMessageTestFormatVersion2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlPolyDataMessageTest ${TestStringFormat2})
  ADD_TEST(igtlSensorMessageTestFormatVersion2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlSensorMessageTest ${TestStringFormat2})
  ADD_TEST(igtlStatusMessageTestFormatVersion2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlStatusMessageTest ${TestStringFormat2})
  ADD_TEST(igtlStringMessageTestFormatVersion2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlStringMessageTest ${TestStringFormat2})
  ADD_TEST(igtlTrackingDataMessageTestFormatVersion2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlTrackingDataMessageTest ${TestStringFormat2})
  ADD_TEST(igtlTrajectoryMessageTestFormatVersion2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlTrajectoryMessageTest ${TestStringFormat2})
  ADD_TEST(igtlTransformMessageTestFormatVersion2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlTransformMessageTest ${TestStringFormat2})
  ADD_TEST(igtlPositionMessageTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlPositionMessageTest)
  ADD_TEST(igtlCommandMessageTestFormatVersion1 ${OpenIGTLink_EXECUTABLE_PATH}/igtlCommandMessageTest ${TestStringFormat1})
  ADD_TEST(igtlCommandMessageTestFormatVersion2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlCommandMessageTest ${TestStringFormat2})
ENDIF()
