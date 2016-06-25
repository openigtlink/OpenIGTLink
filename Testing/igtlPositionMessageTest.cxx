/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 Date:      $Date: 2016/02/12 19:53:38 $
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlPositionMessage.h"
#include "igtlutil/igtl_test_data_position.h"
#include "igtl_position.h"
#include "igtl_header.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


igtl::PositionMessage::Pointer positionSendMsg = igtl::PositionMessage::New();
igtl::PositionMessage::Pointer positionReceiveMsg = igtl::PositionMessage::New();

TEST(PositionMessageTest, Pack)
{
  positionSendMsg->SetHeaderVersion(IGTL_HEADER_VERSION_1);
  positionSendMsg->AllocatePack();
  positionSendMsg->SetTimeStamp(0, 1234567892);
  positionSendMsg->SetDeviceName("DeviceName");
  positionSendMsg->SetPosition(46.0531f, 19.4709f, 46.0531f);
  positionSendMsg->SetQuaternion(0.0f, 0.5773502691f, 0.5773502692f, 0.3333333333f);
  positionSendMsg->Pack();
  int r = memcmp((const void*)positionSendMsg->GetPackPointer(), (const void*)test_position_message,
                 (size_t)(IGTL_HEADER_SIZE));
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)positionSendMsg->GetPackBodyPointer(), (const void*)(test_position_message+IGTL_HEADER_SIZE),positionSendMsg->GetPackBodySize());
  EXPECT_EQ(r, 0);
}


TEST(PositionMessageTest, Unpack)
{
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->AllocatePack();
  memcpy(headerMsg->GetPackPointer(), (const void*)test_position_message, IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  positionReceiveMsg->SetMessageHeader(headerMsg);
  positionReceiveMsg->AllocatePack();
  
  memcpy(positionReceiveMsg->GetPackBodyPointer(), positionSendMsg->GetPackBodyPointer(), positionSendMsg->GetPackBodySize());
  positionReceiveMsg->Unpack();
  
  igtl_float32 position_Truth[3] = {46.0531f, 19.4709f, 46.0531f};
  igtl_float32 position[3] = {0.0,0.0,0.0};
  positionReceiveMsg->GetPosition(position);
  EXPECT_THAT(position, ::testing::ElementsAreArray(position_Truth));
  
  igtl_float32 quaternion_Truth[4] = {0.0f, 0.5773502691f, 0.5773502692f, 0.3333333333f};
  igtl_float32 quaternion[4] = {0.0,0.0,0.0,0.0};
  positionReceiveMsg->GetQuaternion(quaternion);
  EXPECT_THAT(quaternion, ::testing::ElementsAreArray(quaternion_Truth));
}


int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

