ADD_SUBDIRECTORY( igtlutil )

find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})

LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/Testing/googleTest/lib)
set(CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/Testing/googleTest)
IF( ${CMAKE_SYSTEM_NAME} MATCHES Darwin)
  find_package(googleTest REQUIRED)
  set(CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/Testing/googleTest/lib/MacOS)
  find_library(GOOGLETEST_LIBRARY NAMES gtest PATH ${PROJECT_SOURCE_DIR}/Testing/googleTest/lib/MacOS)
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES Linux)
  find_package(googleTest REQUIRED)
  set(CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/Testing/googleTest/lib/Linux)
  find_library(GOOGLETEST_LIBRARY NAMES gtest PATH ${PROJECT_SOURCE_DIR}/Testing/googleTest/lib/Linux)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES Darwin)
message(${CMAKE_SYSTEM_NAME})

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
TARGET_LINK_LIBRARIES(igtlMessageBaseTest OpenIGTLink ${GOOGLETEST_LIBRARY})

ADD_TEST(igtlSocketTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlSocketTest)
ADD_TEST(igtlMultiThreaderTest1 ${OpenIGTLink_EXECUTABLE_PATH}/igtlMultiThreaderTest1)
ADD_TEST(igtlMultiThreaderTest2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlMultiThreaderTest2)
ADD_TEST(igtlMultiThreaderTest3 ${OpenIGTLink_EXECUTABLE_PATH}/igtlMultiThreaderTest3)
ADD_TEST(igtlMessageFactoryTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlMessageFactoryTest)
ADD_TEST(igtlTimeStampTest1 ${OpenIGTLink_EXECUTABLE_PATH}/igtlTimeStampTest1)
ADD_TEST(igtlMessageBaseTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlMessageBaseTest)



