/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 Date:      $Date: 2016/02/12 19:53:38 $
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlBindMessage.h"
#include "../Source/igtlTransformMessage.h"
#include "../Source/igtlStringMessage.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../Source/igtlImageMessage2.h"
#include "../Source/igtlMessageBase.h"
#include "igtl_header.h"
#include "igtl_image.h"
#include "igtlutil/igtl_test_data_image.h"

igtl::ImageMessage2::Pointer imageMessage2Test = igtl::ImageMessage2::New();

void setupTest()
{
  //------------------------------------------------------------
  // size parameters
  int   size[]     = {50, 50, 1};       // image dimension
  float spacing[]  = {1.0, 1.0, 5.0};     // spacing (mm/pixel)
  int   svsize[]   = {50, 50, 1};       // sub-volume size
  int   svoffset[] = {0, 0, 0};           // sub-volume offset
  int   scalarType = igtl::ImageMessage2::TYPE_UINT8;// scalar type
  
  //Initialization of a image message
  imageMessage2Test->SetDimensions(size);
  imageMessage2Test->SetSpacing(spacing);
  imageMessage2Test->SetScalarType(scalarType);
  imageMessage2Test->SetSubVolume(svsize, svoffset);
  imageMessage2Test->AllocatePack(IGTL_IMAGE_HEADER_SIZE+TEST_IMAGE_MESSAGE_SIZE);
  imageMessage2Test->AllocateScalars();
  memcpy((void*)imageMessage2Test->GetPackBodyPointer(), test_image_message+IGTL_HEADER_SIZE, IGTL_IMAGE_HEADER_SIZE+TEST_IMAGE_MESSAGE_SIZE);//here m_Body is set.
  imageMessage2Test->SetScalarPointer((void*)test_image);
  imageMessage2Test->Pack();
}

TEST(BindMessageBaseTest, basicFunctions)
{
  igtl::BindMessageBase::Pointer bindMsgBase;
  bindMsgBase = igtl::BindMessageBase::New();
  
  igtl::StringMessage::Pointer stringMsg;
  stringMsg = igtl::StringMessage::New();
  stringMsg->SetDeviceName("StringMessage");
  
  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();
  transMsg->SetDeviceName("Tracker");
  
  bindMsgBase->Init();
  EXPECT_GT(bindMsgBase->AppendChildMessage(stringMsg),0);
  EXPECT_GT(bindMsgBase->AppendChildMessage(transMsg),1);
  EXPECT_EQ(bindMsgBase->GetNumberOfChildMessages(),2);
  EXPECT_STREQ(bindMsgBase->GetChildMessageType(0), "STRING");
  EXPECT_STREQ(bindMsgBase->GetChildMessageType(1), "TRANSFORM");
  bindMsgBase->SetChildMessage(0, transMsg);
  EXPECT_STREQ(bindMsgBase->GetChildMessageType(0), "TRANSFORM");
  bindMsgBase->Init();
  EXPECT_EQ(bindMsgBase->GetNumberOfChildMessages(),0);
  bindMsgBase->SetNumberOfChildMessages(2);
  EXPECT_EQ(bindMsgBase->GetNumberOfChildMessages(),2);
}


TEST(BindMessageTest, GetChildMessage)
{
  igtl::BindMessage::Pointer bindMessage = igtl::BindMessage::New();
  bindMessage->Init();
  bindMessage->AppendChildMessage(imageMessage2Test);
  //Set up a new image message for receiving the data.
  igtl::ImageMessage2::Pointer childMessage = igtl::ImageMessage2::New();
  childMessage->AllocatePack(IGTL_IMAGE_HEADER_SIZE+TEST_IMAGE_MESSAGE_SIZE);
  childMessage->AllocateScalars();
  memcpy((void*)childMessage->GetPackBodyPointer(), test_image_message+IGTL_HEADER_SIZE, IGTL_IMAGE_HEADER_SIZE+TEST_IMAGE_MESSAGE_SIZE);//here m_Body is set.
  //------
  bindMessage->GetChildMessage(0, childMessage);
  EXPECT_STREQ(childMessage->GetDeviceType(), "IMAGE");
  EXPECT_EQ(strcmp((const char *)childMessage->GetPackBodyPointer(), (const char *)(test_image_message+IGTL_HEADER_SIZE)),0);
}

TEST(GetBindMessage, AppendChildMessage)
{
  igtl::GetBindMessage::Pointer getBindMessage = igtl::GetBindMessage::New();
  getBindMessage->Init();
  getBindMessage->Unpack();
  getBindMessage->AppendChildMessage("someType", "someName");
  igtl::MessageBase::Pointer childMessage = igtl::MessageBase::New();
  //from a GetBindMessage object, we could only get a child messageType,not the message itself, what is the use of this class?.
  EXPECT_EQ(strcmp((const char *)getBindMessage->GetChildMessageType(0), (const char *)("someType")),0);
  getBindMessage->Pack();
}




int main(int argc, char **argv)
{
  setupTest();
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

