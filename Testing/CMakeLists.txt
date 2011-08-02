ADD_SUBDIRECTORY( igtlutil )

find_package(OpenIGTLink REQUIRED)
include(${OpenIGTLink_USE_FILE})

ADD_EXECUTABLE(igtlSocketTest   igtlSocketTest.cxx)
ADD_EXECUTABLE(igtlMultiThreaderTest1   igtlMultiThreaderTest1.cxx)
ADD_EXECUTABLE(igtlMultiThreaderTest2   igtlMultiThreaderTest2.cxx)
ADD_EXECUTABLE(igtlMultiThreaderTest3   igtlMultiThreaderTest3.cxx)

TARGET_LINK_LIBRARIES(igtlSocketTest OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlMultiThreaderTest1 OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlMultiThreaderTest2 OpenIGTLink)
TARGET_LINK_LIBRARIES(igtlMultiThreaderTest3 OpenIGTLink)

ADD_TEST(igtlSocketTest ${OpenIGTLink_EXECUTABLE_PATH}/igtlSocketTest)
ADD_TEST(igtlMultiThreaderTest1 ${OpenIGTLink_EXECUTABLE_PATH}/igtlMultiThreaderTest1)
ADD_TEST(igtlMultiThreaderTest2 ${OpenIGTLink_EXECUTABLE_PATH}/igtlMultiThreaderTest2)
ADD_TEST(igtlMultiThreaderTest3 ${OpenIGTLink_EXECUTABLE_PATH}/igtlMultiThreaderTest3)



