/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlCommandMessage.h"
#include "igtlutil/igtl_test_data_command.h"
#include "igtlMessageDebugFunction.h"
#include "igtl_command.h"
#include "igtl_header.h"
#include "igtlTestConfig.h"
#include "string.h"

igtl::CommandMessage::Pointer sendCommandMsg = igtl::CommandMessage::New();
igtl::CommandMessage::Pointer receiveCommandMsg = igtl::CommandMessage::New();

void BuildMessageFormat1()
{
  sendCommandMsg = igtl::CommandMessage::New();
  sendCommandMsg->SetHeaderVersion(1);
  sendCommandMsg->SetCommandId(5);
  sendCommandMsg->SetContentEncoding(3);
  sendCommandMsg->SetCommandName("This is a test command");
  sendCommandMsg->SetCommandContent("Start the tracking machine");
  sendCommandMsg->Pack();
}

TEST(CommandMessageTest, Pack)
{
  BuildMessageFormat1();
  int r = memcmp((const void*)sendCommandMsg->GetBufferPointer(), (const void*)test_command_message,(size_t)(IGTL_HEADER_SIZE));
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)sendCommandMsg->GetBufferBodyPointer(), (const void*)(test_command_message+(size_t)(IGTL_HEADER_SIZE)), sizeof(test_command_message)-IGTL_HEADER_SIZE);
  EXPECT_EQ(r, 0);
}


TEST(CommandMessageTest, UnPack)
{
  BuildMessageFormat1();
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->InitBuffer();
  memcpy(headerMsg->GetBufferPointer(), (const void*)sendCommandMsg->GetBufferPointer(), IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  receiveCommandMsg->SetMessageHeader(headerMsg);
  receiveCommandMsg->AllocatePack();
  memcpy(receiveCommandMsg->GetBufferBodyPointer(), sendCommandMsg->GetBufferBodyPointer(), headerMsg->GetBodySizeToRead());
  receiveCommandMsg->Unpack();
  igtl_header *messageHeader = (igtl_header *)receiveCommandMsg->GetBufferPointer();
  EXPECT_STREQ(messageHeader->device_name, "");
  EXPECT_STREQ(messageHeader->name, "COMMAND");
  EXPECT_EQ(messageHeader->header_version, 1);
  EXPECT_EQ(messageHeader->timestamp, 0);
  EXPECT_EQ(messageHeader->body_size, sizeof(test_command_message)-IGTL_HEADER_SIZE);
  igtl_uint64 crc;
  memcpy(&crc, &test_command_message[IGTL_HEADER_SIZE-sizeof(igtl_uint64)], sizeof(igtl_uint64));
  if(igtl_is_little_endian())
  {
    crc = BYTE_SWAP_INT64(crc);
  }
  EXPECT_EQ(messageHeader->crc, crc);
  EXPECT_STREQ(receiveCommandMsg->GetCommandContent().c_str(), "Start the tracking machine");
}


#if OpenIGTLink_PROTOCOL_VERSION >= 3
#include "igtlutil/igtl_test_data_commandFormat2.h"
#include "igtlMessageFormat2TestMacro.h"
void BuildMessageFormat2()
{
  sendCommandMsg = igtl::CommandMessage::New();
  sendCommandMsg->SetHeaderVersion(2);
  sendCommandMsg->SetCommandId(5);
  sendCommandMsg->SetContentEncoding(3);
  sendCommandMsg->SetCommandName("This is a test command");
  sendCommandMsg->SetCommandContent("Start the tracking machine");
  sendCommandMsg->SetDeviceName("OpticalTracker");
  igtlMetaDataAddElementMacro(sendCommandMsg);
  sendCommandMsg->Pack();
}

TEST(CommandMessageTest, PackFormatVersion2)
{
  BuildMessageFormat2();
  int r = memcmp((const void*)sendCommandMsg->GetBufferPointer(), (const void*)test_command_messageFormat2,
                 (size_t)(IGTL_HEADER_SIZE));
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)sendCommandMsg->GetBufferBodyPointer(), (const void*)(test_command_messageFormat2+(size_t)(IGTL_HEADER_SIZE)), sizeof(test_command_messageFormat2)-IGTL_HEADER_SIZE);
  EXPECT_EQ(r, 0);
}


TEST(CommandMessageTest, UnpackFormatVersion2)
{
  BuildMessageFormat2();
  igtlMetaDataAddElementMacro(sendCommandMsg);
  sendCommandMsg->Pack();
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
  headerMsg->InitBuffer();
  memcpy(headerMsg->GetBufferPointer(), (const void*)sendCommandMsg->GetBufferPointer(), IGTL_HEADER_SIZE);
  headerMsg->Unpack();

  receiveCommandMsg->SetMessageHeader(headerMsg);
  receiveCommandMsg->AllocatePack();
  memcpy(receiveCommandMsg->GetBufferBodyPointer(), sendCommandMsg->GetBufferBodyPointer(), headerMsg->GetBodySizeToRead());
  receiveCommandMsg->Unpack();

  igtl_header *messageHeader = (igtl_header *)receiveCommandMsg->GetBufferPointer();
  EXPECT_STREQ(messageHeader->device_name, "OpticalTracker");
  EXPECT_STREQ(messageHeader->name, "COMMAND");
  EXPECT_EQ(messageHeader->header_version, 2);
  EXPECT_EQ(messageHeader->timestamp, 0);

  int size = sizeof(test_command_messageFormat2) - IGTL_HEADER_SIZE;
  EXPECT_EQ(messageHeader->body_size, sizeof(test_command_messageFormat2)-IGTL_HEADER_SIZE);

  igtl_uint64 crc;
  memcpy(&crc, &test_command_messageFormat2[IGTL_HEADER_SIZE-sizeof(igtl_uint64)], sizeof(igtl_uint64));
  if(igtl_is_little_endian())
  {
    crc = BYTE_SWAP_INT64(crc);
  }
  EXPECT_EQ(messageHeader->crc, crc);
  EXPECT_STREQ(receiveCommandMsg->GetCommandContent().c_str(), "Start the tracking machine");
  igtlMetaDataComparisonMacro(receiveCommandMsg);
}

#endif

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}




