/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlCapabilityMessage.h"
#include "igtlutil/igtl_test_data_capability.h"
#include "igtlMessageDebugFunction.h"
#include "igtl_header.h"
#include "igtlTestConfig.h"
#include "string.h"

#define CAPABILITY_MESSAGE_BODY_SIZE 48

igtl::CapabilityMessage::Pointer capabilitySendMsg = igtl::CapabilityMessage::New();
igtl::CapabilityMessage::Pointer capabilityReceiveMsg = igtl::CapabilityMessage::New();

TEST(ColorTableMessageTest, Pack)
{
  capabilitySendMsg->SetTimeStamp(0, 1234567892);
  capabilitySendMsg->SetDeviceName("DeviceName");
  capabilitySendMsg->SetNumberOfTypes(4);
  capabilitySendMsg->SetType(0, "IMAGE");
  capabilitySendMsg->SetType(1, "GET_IMAGE");
  capabilitySendMsg->SetType(2, "TRANSFORM");
  capabilitySendMsg->SetType(3, "GET_TRANS");
  capabilitySendMsg->Pack();
  
  TestDebugCharArrayCmp(capabilitySendMsg->GetPackPointer(), test_capability_message, IGTL_HEADER_SIZE);
  int r = memcmp((const void*)capabilitySendMsg->GetPackPointer(), (const void*)test_capability_message, IGTL_HEADER_SIZE);
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)capabilitySendMsg->GetPackBodyPointer(), (const void*)(test_capability_message+IGTL_HEADER_SIZE), CAPABILITY_MESSAGE_BODY_SIZE);
  EXPECT_EQ(r, 0);
}


TEST(ColorTableMessageTest, Unpack)
{
  igtl::MessageHeader::Pointer headerMsg;
  headerMsg = igtl::MessageHeader::New();
  headerMsg->InitPack();
  memcpy(headerMsg->GetPackPointer(), capabilitySendMsg->GetPackPointer(), IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  capabilityReceiveMsg->SetMessageHeader(headerMsg);
  capabilityReceiveMsg->AllocatePack();
  memcpy(capabilityReceiveMsg->GetPackBodyPointer(), capabilitySendMsg->GetPackBodyPointer(), capabilitySendMsg->GetPackBodySize());
  capabilityReceiveMsg->Unpack();
  
  EXPECT_EQ(capabilityReceiveMsg->GetNumberOfTypes(),4);
  EXPECT_STREQ(capabilityReceiveMsg->GetType(0),"IMAGE");
  EXPECT_STREQ(capabilityReceiveMsg->GetType(1),"GET_IMAGE");
  EXPECT_STREQ(capabilityReceiveMsg->GetType(2),"TRANSFORM");
  EXPECT_STREQ(capabilityReceiveMsg->GetType(3),"GET_TRANS");
  std::vector<std::string> typeStrings = capabilityReceiveMsg->GetTypes();
  EXPECT_EQ(typeStrings.size(),4);
  EXPECT_STREQ(typeStrings[0].c_str(),"IMAGE");
  EXPECT_STREQ(typeStrings[1].c_str(),"GET_IMAGE");
  EXPECT_STREQ(typeStrings[2].c_str(),"TRANSFORM");
  EXPECT_STREQ(typeStrings[3].c_str(),"GET_TRANS");
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

