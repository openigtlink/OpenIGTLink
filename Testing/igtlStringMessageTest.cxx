/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 Date:      $Date: 2016/02/12 19:53:38 $
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "../Source/igtlStringMessage.h"
#include "igtl_header.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "igtl_string.h"
#include "igtlOSUtil.h"
#include "../Testing/igtlutil/igtl_test_data_string.h"

#define IGTL_STRING_TEST_STRING "Welcome to OpenIGTLink"
#define IGTL_STRING_TEST_STRING_LEN 22

igtl::StringMessage::Pointer stringMsg = igtl::StringMessage::New();
igtl::StringMessage::Pointer stringMsg2 = igtl::StringMessage::New();

TEST(StringMessageTest, Pack)
{
  stringMsg->SetString(IGTL_STRING_TEST_STRING);
  stringMsg->AllocatePack();
  stringMsg->SetTimeStamp(0, 1234567890);
  stringMsg->SetDeviceName("DeviceName");
  stringMsg->SetEncoding(3);
  stringMsg->Pack();
  int r = memcmp((const void*)stringMsg->GetPackPointer(), (const void*)test_string_message,
                 (size_t)(IGTL_HEADER_SIZE));
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)stringMsg->GetPackBodyPointer(), (const void*)(test_string_message+IGTL_HEADER_SIZE),stringMsg->GetPackBodySize());
  EXPECT_EQ(r, 0);
}


TEST(StringMessageTest, Unpack)
{
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->AllocatePack();
  memcpy(headerMsg->GetPackPointer(), (const void*)test_string_message, IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  stringMsg2->SetMessageHeader(headerMsg);
  stringMsg2->AllocatePack();
  
  memcpy(stringMsg2->GetPackBodyPointer(), stringMsg->GetPackBodyPointer(), stringMsg->GetPackBodySize());
  stringMsg2->Unpack();
  
  EXPECT_EQ(stringMsg2->GetEncoding(),3);
  EXPECT_STREQ(stringMsg2->GetString(),IGTL_STRING_TEST_STRING);
  
}


int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

