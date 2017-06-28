/*=========================================================================
 
  Program:   OpenIGTLink Library
  Language:  C++
 
  Copyright (c) Insight Software Consortium. All rights reserved.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.
 
  =========================================================================*/

#include "igtlColorTableMessage.h"
#include "igtlutil/igtl_test_data_colortable2.h"
#include "igtlMessageDebugFunction.h"
#include "igtl_colortable.h"
#include "igtl_header.h"
#include "igtlTestConfig.h"
#include "string.h"

#define COLOR_TABLE_SIZE 256

igtl::ColorTableMessage::Pointer colorTableSendMsg = igtl::ColorTableMessage::New();
igtl::ColorTableMessage::Pointer colorTableReceiveMsg = igtl::ColorTableMessage::New();

TEST(ColorTableMessageTest, Pack)
{
  colorTableSendMsg->SetTimeStamp(0, 1234567892);
  colorTableSendMsg->SetDeviceName("DeviceName");
  colorTableSendMsg->SetIndexType(IGTL_COLORTABLE_INDEX_UINT8);
  colorTableSendMsg->SetMapType(IGTL_COLORTABLE_MAP_UINT8);
  colorTableSendMsg->AllocateTable();
  memcpy(colorTableSendMsg->GetTablePointer(), test_colortable_message+IGTL_HEADER_SIZE+IGTL_COLORTABLE_HEADER_SIZE, (size_t)(COLOR_TABLE_SIZE));
  colorTableSendMsg->Pack();
  
  TestDebugCharArrayCmp(colorTableSendMsg->GetPackPointer(), test_colortable_message, IGTL_HEADER_SIZE);
  int r = memcmp((const void*)colorTableSendMsg->GetPackPointer(), (const void*)test_colortable_message, IGTL_HEADER_SIZE);
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)colorTableSendMsg->GetPackBodyPointer(), (const void*)(test_colortable_message+IGTL_HEADER_SIZE), IGTL_COLORTABLE_HEADER_SIZE+COLOR_TABLE_SIZE);
  EXPECT_EQ(r, 0);
}


TEST(ColorTableMessageTest, Unpack)
{
  igtl::MessageHeader::Pointer headerMsg;
  headerMsg = igtl::MessageHeader::New();
  headerMsg->InitPack();
  memcpy(headerMsg->GetPackPointer(), colorTableSendMsg->GetPackPointer(), IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  colorTableReceiveMsg->SetMessageHeader(headerMsg);
  colorTableReceiveMsg->AllocatePack();
  memcpy(colorTableReceiveMsg->GetPackBodyPointer(), colorTableSendMsg->GetPackBodyPointer(), colorTableSendMsg->GetPackBodySize());
  colorTableReceiveMsg->Unpack();
  
  EXPECT_EQ(colorTableReceiveMsg->GetMapType(),IGTL_COLORTABLE_MAP_UINT8);
  EXPECT_EQ(colorTableReceiveMsg->GetIndexType(),IGTL_COLORTABLE_INDEX_UINT8);
  for (int i = 0; i < 256; i ++)
    {
    EXPECT_EQ(((unsigned char*)colorTableReceiveMsg->GetTablePointer())[i], i);
    }
  
}


int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

