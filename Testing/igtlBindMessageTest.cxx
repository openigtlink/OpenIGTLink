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
#include "igtlutil/igtl_test_data_image.h"
#include "igtlTransformMessage.h"
#include "igtlStringMessage.h"
#include "igtlImageMessage2.h"
#include "igtlMessageBase.h"
#include "igtl_header.h"
#include "igtl_image.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

igtl::ImageMessage2::Pointer imageMsg2 = igtl::ImageMessage2::New();
igtl::GetBindMessage::Pointer getBindMessage = igtl::GetBindMessage::New();
igtl::StartBindMessage::Pointer startBindMessage = igtl::StartBindMessage::New();

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
  imageMsg2->SetDimensions(size);
  imageMsg2->SetSpacing(spacing);
  imageMsg2->SetScalarType(scalarType);
  imageMsg2->SetSubVolume(svsize, svoffset);
  imageMsg2->AllocatePack(IGTL_IMAGE_HEADER_SIZE+TEST_IMAGE_MESSAGE_SIZE);
  imageMsg2->AllocateScalars();
  memcpy((void*)imageMsg2->GetPackBodyPointer(), test_image_message+IGTL_HEADER_SIZE, IGTL_IMAGE_HEADER_SIZE+TEST_IMAGE_MESSAGE_SIZE);//here m_Body is set.
  imageMsg2->SetScalarPointer((void*)test_image);
  imageMsg2->Pack();
  
  getBindMessage->Init();
  getBindMessage->AppendChildMessage("Image", "UltrasoundImage");
  getBindMessage->Pack();
  startBindMessage->Init();
  startBindMessage->AppendChildMessage("Image", "UltrasoundImage");
  startBindMessage->Pack();
}

TEST(BindMessageBaseTest, BaseFunctions)
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
  bindMessage->AppendChildMessage(imageMsg2);
  //Set up a new image message for receiving the data.
  igtl::ImageMessage2::Pointer childMessage = igtl::ImageMessage2::New();
  childMessage->AllocatePack(IGTL_IMAGE_HEADER_SIZE+TEST_IMAGE_MESSAGE_SIZE);
  childMessage->AllocateScalars();
  //------
  bindMessage->GetChildMessage(0, childMessage);
  EXPECT_STREQ(childMessage->GetDeviceType(), "IMAGE");
  EXPECT_EQ(strcmp((const char *)childMessage->GetPackBodyPointer(), (const char *)(test_image_message+IGTL_HEADER_SIZE)),0);
}

TEST(GetBindMessage, AppendChildMessage)
{
  getBindMessage->AppendChildMessage("Transform", "Tracker");
  getBindMessage->Pack();
  EXPECT_EQ(getBindMessage->GetPackSize(),110);
  getBindMessage->AppendChildMessage("Point", "Sensor");
  getBindMessage->Pack();
  EXPECT_EQ(getBindMessage->GetPackSize(),129);
  EXPECT_EQ(strcmp((const char *)getBindMessage->GetChildMessageType(0), (const char *)("Image")),0);
  EXPECT_EQ(strcmp((const char *)getBindMessage->GetChildMessageType(1), (const char *)("Transform")),0);
  EXPECT_EQ(strcmp((const char *)getBindMessage->GetChildMessageType(2), (const char *)("Point")),0);
}


TEST(StartBindMessage, Resolution)
{
  igtlUint64 timeRes = 4999;
  startBindMessage->AppendChildMessage("Transform", "Tracker");
  startBindMessage->Pack();
  EXPECT_EQ(startBindMessage->GetPackSize(),118);
  startBindMessage->AppendChildMessage("Point", "Sensor");
  startBindMessage->Pack();
  EXPECT_EQ(startBindMessage->GetPackSize(),137);
  startBindMessage->SetResolution(timeRes);
  EXPECT_EQ(startBindMessage->GetResolution(), timeRes);
}

TEST(RTSBindMessage, Status)
{
  igtl::RTSBindMessage::Pointer rtsBindMessage = igtl::RTSBindMessage::New();
  rtsBindMessage->SetStatus(1);
  EXPECT_EQ(rtsBindMessage->GetStatus(), 1);
}


int main(int argc, char **argv)
{
  setupTest();
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

