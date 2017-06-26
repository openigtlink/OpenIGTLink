/*=========================================================================
 
  Program:   OpenIGTLink Library
  Language:  C++
 
  Copyright (c) Insight Software Consortium. All rights reserved.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.
 
  =========================================================================*/

#include "igtlStringMessage.h"
#include "igtlutil/igtl_test_data_string.h"
#include "igtl_string.h"
#include "igtl_header.h"
#include "igtlTestConfig.h"
#include "string.h"

#define IGTL_STRING_TEST_STRING "Welcome to OpenIGTLink"
#define IGTL_STRING_TEST_STRING_LEN 22

igtl::StringMessage::Pointer stringSendMsg = igtl::StringMessage::New();
igtl::StringMessage::Pointer stringReceiveMsg = igtl::StringMessage::New();

TEST(StringMessageTest, Pack)
{
  stringSendMsg->SetHeaderVersion(IGTL_HEADER_VERSION_1);
  stringSendMsg->SetString(IGTL_STRING_TEST_STRING);
  stringSendMsg->AllocatePack();
  stringSendMsg->SetTimeStamp(0, 1234567892);
  stringSendMsg->SetDeviceName("DeviceName");
  stringSendMsg->SetEncoding(3);
  stringSendMsg->Pack();
  int r = memcmp((const void*)stringSendMsg->GetPackPointer(), (const void*)test_string_message,
                 (size_t)(IGTL_HEADER_SIZE));
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)stringSendMsg->GetPackBodyPointer(), (const void*)(test_string_message+IGTL_HEADER_SIZE),stringSendMsg->GetPackBodySize());
  EXPECT_EQ(r, 0);
}


TEST(StringMessageTest, Unpack)
{
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->AllocatePack();
  memcpy(headerMsg->GetPackPointer(), (const void*)test_string_message, IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  stringReceiveMsg->SetMessageHeader(headerMsg);
  stringReceiveMsg->AllocatePack();
  
  memcpy(stringReceiveMsg->GetPackBodyPointer(), stringSendMsg->GetPackBodyPointer(), stringSendMsg->GetPackBodySize());
  stringReceiveMsg->Unpack();
  
  EXPECT_EQ(stringReceiveMsg->GetEncoding(),3);
  EXPECT_STREQ(stringReceiveMsg->GetString(),IGTL_STRING_TEST_STRING);
  
}


int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

