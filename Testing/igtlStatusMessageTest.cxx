/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 Date:      $Date: 2016/02/12 19:53:38 $
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "../Source/igtlStatusMessage.h"
#include "igtl_header.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "igtl_status.h"
#include "igtlOSUtil.h"
#include "../Testing/igtlutil/igtl_test_data_status.h"


igtl::StatusMessage::Pointer statusMsg = igtl::StatusMessage::New();
igtl::StatusMessage::Pointer statusMsg2 = igtl::StatusMessage::New();

#define STR_ERROR_NAME     "ACTUATOR_DISABLED" /* within 20 characters */
#define STR_ERROR_MESSAGE  "Actuator A is disabled."

TEST(StatusMessageTest, Pack)
{
  std::string statusString = "randomstringrandomstring";
  statusMsg->SetStatusString(statusString.c_str());// pass an empty string with size 54,just to initialize the memory
  statusMsg->AllocatePack();
  statusMsg->SetTimeStamp(0, 1234567890);
  statusMsg->SetDeviceName("DeviceName");
  statusMsg->SetCode(IGTL_STATUS_DISABLED);
  statusMsg->SetSubCode(0x0A);
  statusMsg->SetErrorName(STR_ERROR_NAME);
  statusMsg->SetStatusString(STR_ERROR_MESSAGE);
  statusMsg->Pack();
  int r = memcmp((const void*)statusMsg->GetPackPointer(), (const void*)test_status_message,
                 (size_t)(IGTL_HEADER_SIZE));
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)statusMsg->GetPackBodyPointer(), (const void*)(test_status_message+IGTL_HEADER_SIZE),statusMsg->GetPackBodySize());
  EXPECT_EQ(r, 0);
}


TEST(StatusMessageTest, Unpack)
{
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->AllocatePack();
  memcpy(headerMsg->GetPackPointer(), (const void*)test_status_message, IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  statusMsg2->SetMessageHeader(headerMsg);
  statusMsg2->AllocatePack();
  
  memcpy(statusMsg2->GetPackBodyPointer(), statusMsg->GetPackBodyPointer(), statusMsg->GetPackBodySize());
  statusMsg2->Unpack();
  
  EXPECT_EQ(statusMsg2->GetCode(),IGTL_STATUS_DISABLED);
  EXPECT_EQ(statusMsg2->GetSubCode(),(igtlInt64)0x0A);
  EXPECT_STREQ(statusMsg2->GetErrorName(),STR_ERROR_NAME);
  EXPECT_STREQ(statusMsg2->GetStatusString(),STR_ERROR_MESSAGE);
}


int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

