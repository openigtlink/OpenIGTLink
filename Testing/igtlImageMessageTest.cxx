/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 Date:      $Date: 2016/02/12 19:53:38 $
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlImageMessage.h"
#include "igtl_types.h"
#include "igtl_header.h"
#include "igtl_image.h"
#include "igtl_util.h"
#include "gtest/gtest.h"
#include "igtlutil/igtl_test_data_image.h"


TEST(ImageMessageTest, InitializationTest)
{
  igtl::ImageMessage::Pointer imageMessageTest = igtl::ImageMessage::New();
  imageMessageTest->InitPack();
  //unsigned char messageBodyPart[IGTL_IMAGE_HEADER_SIZE+TEST_IMAGE_MESSAGE_SIZE] = {};
  //std::copy(test_image_message+IGTL_HEADER_SIZE,test_image_message+IGTL_HEADER_SIZE+IGTL_IMAGE_HEADER_SIZE+TEST_IMAGE_MESSAGE_SIZE, messageBodyPart);
  imageMessageTest->AllocateScalars();
  memcpy((void*)imageMessageTest->GetPackPointer(), (const void*)test_image_message, IGTL_HEADER_SIZE);//here m_header is set.
  //imageMessageTest->GetPackBodyPointer() = (const void*)test_image_message;
  memcpy((void*)imageMessageTest->GetPackBodyPointer(), test_image_message+IGTL_HEADER_SIZE, IGTL_IMAGE_HEADER_SIZE+TEST_IMAGE_MESSAGE_SIZE);//here m_Body is set.
  //memcpy((void*)imageMessageTest->GetPackPointer(), (const void*)test_image_message, IGTL_HEADER_SIZE);
  imageMessageTest->Unpack(0); // use the crc from the m_header
  //EXPECT_STREQ(imageMessageTest->GetDeviceName(), "DeviceName");
}


int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

