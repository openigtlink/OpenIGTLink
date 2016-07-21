/*=========================================================================

  Program:   OpenIGTLink Library
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlMessageBase.h"
#include "igtlMessageHeader.h"
#include "igtlTestConfig.h"
#include "string.h"

TEST(MessageBaseTest, InitializationTest)
{
  igtl::MessageBase::Pointer messageBaseTest = igtl::MessageBase::New();
  messageBaseTest->SetDeviceName("DeviceTest");
  EXPECT_STREQ(messageBaseTest->GetDeviceName(), "DeviceTest");
  messageBaseTest->InitPack();
  EXPECT_STREQ(messageBaseTest->GetDeviceName(),"");
}

TEST(MessageBaseTest, SetDeviceNameTest)
{
  igtl::MessageBase::Pointer messageBaseTest = igtl::MessageBase::New();
  messageBaseTest->InitPack();
  EXPECT_STREQ(messageBaseTest->GetDeviceName(), "");
  messageBaseTest->SetDeviceName("DeviceTest");
  EXPECT_STREQ(messageBaseTest->GetDeviceName(), "DeviceTest");
}

TEST(MessageBaseTest, GetDeviceNameTest)
{
  igtl::MessageBase::Pointer messageBaseTest = igtl::MessageBase::New();
  messageBaseTest->SetDeviceName("DeviceTest");
  EXPECT_STREQ(messageBaseTest->GetDeviceName(), "DeviceTest");
}


TEST(MessageBaseTest, TimeStampTest)
{
  igtl::MessageBase::Pointer messageBaseTest = igtl::MessageBase::New();
  messageBaseTest->SetTimeStamp(1,2);
  unsigned int sec=0, nanosec=0;
  messageBaseTest->GetTimeStamp(&sec,&nanosec);
  EXPECT_EQ(sec, 1);
  EXPECT_EQ(nanosec, 2);
  igtl::TimeStamp::Pointer ts_input = igtl::TimeStamp::New();
  ts_input->SetTime(123,500000000); //nanosecond can not be larger or equal to 1e9.
  //5e8 nanosecond equals 2147483647 frac second.
  messageBaseTest->SetTimeStamp(ts_input);
  messageBaseTest->GetTimeStamp(&sec,&nanosec);
  EXPECT_EQ(sec, 123);
  EXPECT_EQ(nanosec, 2147483648);
}

TEST(MessageBaseTest, UNPACKTEST)
{
  igtl::MessageBase::Pointer messageBaseTest = igtl::MessageBase::New();
  messageBaseTest->AllocateBuffer();
  int status = messageBaseTest->Unpack();
  EXPECT_EQ(status, static_cast<int>(messageBaseTest->UNPACK_HEADER));
}


int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}



