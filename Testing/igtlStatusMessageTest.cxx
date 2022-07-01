/*=========================================================================
 
  Program:   OpenIGTLink Library
  Language:  C++
 
  Copyright (c) Insight Software Consortium. All rights reserved.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.
 
  =========================================================================*/

#include "igtlStatusMessage.h"
#include "igtlutil/igtl_test_data_status.h"
#include "string.h"
#include "igtl_status.h"
#include "igtl_header.h"
#include "igtlTestConfig.h"
#include "string.h"


igtl::StatusMessage::Pointer statusSendMsg = igtl::StatusMessage::New();
igtl::StatusMessage::Pointer statusReceiveMsg = igtl::StatusMessage::New();

#define STR_ERROR_NAME     "ACTUATOR_DISABLED" /* within 20 characters */
#define STR_ERROR_MESSAGE  "Actuator A is disabled."

TEST(StatusMessageTest, Pack)
{
  std::string statusString = "randomstringrandomstring";
  statusSendMsg->SetHeaderVersion(IGTL_HEADER_VERSION_1);
  statusSendMsg->SetStatusString(statusString.c_str());// pass an empty string with size 54,just to initialize the memory
  statusSendMsg->AllocatePack();
  statusSendMsg->SetTimeStamp(0, 1234567892);
  statusSendMsg->SetDeviceName("DeviceName");
  statusSendMsg->SetCode(IGTL_STATUS_DISABLED);
  statusSendMsg->SetSubCode(0x0A);
  statusSendMsg->SetErrorName(STR_ERROR_NAME);
  statusSendMsg->SetStatusString(STR_ERROR_MESSAGE);
  statusSendMsg->Pack();
  int r = memcmp((const void*)statusSendMsg->GetPackPointer(), (const void*)test_status_message,
                 (size_t)(IGTL_HEADER_SIZE));
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)statusSendMsg->GetPackBodyPointer(), (const void*)(test_status_message+IGTL_HEADER_SIZE),statusSendMsg->GetPackBodySize());
  EXPECT_EQ(r, 0);
}


TEST(StatusMessageTest, Unpack)
{
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->AllocatePack();
  memcpy(headerMsg->GetPackPointer(), statusSendMsg->GetPackPointer(), IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  statusReceiveMsg->SetMessageHeader(headerMsg);
  statusReceiveMsg->AllocatePack();
  memcpy(statusReceiveMsg->GetPackBodyPointer(), statusSendMsg->GetPackBodyPointer(), statusSendMsg->GetPackBodySize());
  statusReceiveMsg->Unpack();
  
  EXPECT_EQ(statusReceiveMsg->GetCode(),IGTL_STATUS_DISABLED);
  EXPECT_EQ(statusReceiveMsg->GetSubCode(),(igtlInt64)0x0A);
  EXPECT_STREQ(statusReceiveMsg->GetErrorName(),STR_ERROR_NAME);
  EXPECT_STREQ(statusReceiveMsg->GetStatusString(),STR_ERROR_MESSAGE);
}


#if OpenIGTLink_HEADER_VERSION >= 2
TEST(StatusMessageTest, UnpackInvalidMetaData)
{
  statusSendMsg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
  statusSendMsg->SetMetaDataElement("metaData1", 1);
  statusSendMsg->AllocatePack();
  statusSendMsg->Pack();

  // set an invalid meta data element number to force wrong memory access 
  ((char*)statusSendMsg->GetBufferPointer())[statusSendMsg->GetBufferSize() - 13] = 20;

  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->AllocatePack();
  memcpy(headerMsg->GetPackPointer(), statusSendMsg->GetPackPointer(), IGTL_HEADER_SIZE); 
  headerMsg->Unpack();

  statusReceiveMsg->SetMessageHeader(headerMsg);
  statusReceiveMsg->AllocatePack();
  memcpy(statusReceiveMsg->GetPackBodyPointer(), statusSendMsg->GetPackBodyPointer(), statusSendMsg->GetPackBodySize());
  
  EXPECT_NE(statusReceiveMsg->Unpack(), igtl::MessageBase::UNPACK_BODY);
}
#endif


int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

