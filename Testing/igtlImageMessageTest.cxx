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
#include "igtlutil/igtl_test_data_image.h"
#include "igtlMessageDebugFunction.h"
#include "igtl_types.h"
#include "igtl_header.h"
#include "igtl_image.h"
#include "igtl_util.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

igtl::ImageMessage::Pointer imageSendMsg = igtl::ImageMessage::New();
igtl::ImageMessage::Pointer imageReceiveMsg = igtl::ImageMessage::New();
float inT[4] = {-0.954892f, 0.196632f, -0.222525f, 0.0};
float inS[4] = {-0.196632f, 0.142857f, 0.970014f, 0.0};
float inN[4] = {0.222525f, 0.970014f, -0.0977491f, 0.0};
float inOrigin[4] = {46.0531f,19.4709f,46.0531f, 1.0};
igtl::Matrix4x4 inMatrix = {{inT[0],inS[0],inN[0],inOrigin[0]},
  {inT[1],inS[1],inN[1],inOrigin[1]},
  {inT[2],inS[2],inN[2],inOrigin[2]},
  {inT[3],inS[3],inN[3],inOrigin[3]}};
int   size[3]     = {50, 50, 1};       // image dimension
float spacing[3]  = {1.0f, 1.0f, 1.0f};     // spacing (mm/pixel)
int   svsize[3]   = {50, 50, 1};       // sub-volume size
int   svoffset[3] = {0, 0, 0};           // sub-volume offset
int   scalarType = igtl::ImageMessage::TYPE_UINT8;// scalar type


void BuildUp()
{
  imageSendMsg = igtl::ImageMessage::New();
  imageSendMsg->SetVersion(IGTL_HEADER_VERSION_1);
  imageSendMsg->SetTimeStamp(0, 1234567892);
  imageSendMsg->SetDeviceName("DeviceName");
  //Initialization of a image message
  imageSendMsg->SetDimensions(size);
  imageSendMsg->SetSpacing(spacing);
  imageSendMsg->SetScalarType(scalarType);
  imageSendMsg->SetSubVolume(svsize, svoffset);
  imageSendMsg->SetNumComponents(1);
  imageSendMsg->SetScalarType(IGTL_IMAGE_STYPE_TYPE_UINT8);
  imageSendMsg->SetEndian(IGTL_IMAGE_ENDIAN_LITTLE);
  imageSendMsg->SetCoordinateSystem(IGTL_IMAGE_COORD_RAS);
  imageSendMsg->SetMatrix(inMatrix);
  imageSendMsg->AllocateScalars();
  memcpy((void*)imageSendMsg->GetScalarPointer(), test_image_message+IGTL_HEADER_SIZE+IGTL_IMAGE_HEADER_SIZE, TEST_IMAGE_MESSAGE_SIZE);//here m_Image is set.
  imageSendMsg->Pack();
}

TEST(ImageMessageTest, Pack)
{
  BuildUp();
  int r = memcmp((const void*)imageSendMsg->GetPackPointer(), (const void*)test_image_message,
                 (size_t)(IGTL_HEADER_SIZE+IGTL_IMAGE_HEADER_SIZE+TEST_IMAGE_MESSAGE_SIZE));
  EXPECT_EQ(r, 0);
}

TEST(ImageMessageTest, Unpack)
{
  BuildUp();
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->AllocatePack();
  memcpy(headerMsg->GetPackPointer(), (const void*)imageSendMsg->GetPackPointer(), IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  imageReceiveMsg->SetMessageHeader(headerMsg);
  imageReceiveMsg->AllocatePack();
  memcpy(imageReceiveMsg->GetPackBodyPointer(), imageSendMsg->GetPackBodyPointer(), IGTL_IMAGE_HEADER_SIZE+TEST_IMAGE_MESSAGE_SIZE);
  imageReceiveMsg->Unpack();
  
  igtl_header *messageHeader = (igtl_header *)imageReceiveMsg->GetPackPointer();
  EXPECT_STREQ(messageHeader->device_name, "DeviceName");
  EXPECT_STREQ(messageHeader->name, "IMAGE");
  EXPECT_EQ(messageHeader->version, 1);
  EXPECT_EQ(messageHeader->timestamp, 1234567892);
  EXPECT_EQ(messageHeader->body_size, IGTL_IMAGE_HEADER_SIZE+TEST_IMAGE_MESSAGE_SIZE);

  int returnSize[3] = {0,0,0};
  imageReceiveMsg->GetDimensions(returnSize);
  EXPECT_THAT(returnSize,testing::ElementsAreArray(size));
  float returnSpacing[3] = {0.0f,0.0f,0.0f};
  imageReceiveMsg->GetSpacing(returnSpacing);
  EXPECT_TRUE(ArrayFloatComparison(returnSpacing, spacing, 3, ABS_ERROR));
  int returnSvsize[3] = {0,0,0}, returnSvoffset[3] = {0,0,0};
  imageReceiveMsg->GetSubVolume(returnSvsize, returnSvoffset);
  EXPECT_THAT(returnSvsize,testing::ElementsAreArray(svsize));
  EXPECT_THAT(returnSvoffset,testing::ElementsAreArray(svoffset));
  EXPECT_EQ(imageReceiveMsg->GetScalarType(), IGTL_IMAGE_STYPE_TYPE_UINT8);
  EXPECT_EQ(imageReceiveMsg->GetEndian(), IGTL_IMAGE_ENDIAN_LITTLE);
  EXPECT_EQ(imageReceiveMsg->GetCoordinateSystem(), IGTL_IMAGE_COORD_RAS);
  
  igtl::Matrix4x4 outMatrix = {{0.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,0.0}};
  imageReceiveMsg->GetMatrix(outMatrix);
  EXPECT_TRUE(MatrixComparison(outMatrix, inMatrix, ABS_ERROR));
  //The imageHeader is byte-wized converted, so we skip the comparison of the image header.
  int r = memcmp((const char*)imageReceiveMsg->GetPackBodyPointer()+IGTL_IMAGE_HEADER_SIZE, (const void*)(test_image_message+IGTL_HEADER_SIZE+IGTL_IMAGE_HEADER_SIZE), (size_t)(TEST_IMAGE_MESSAGE_SIZE));
  EXPECT_EQ(r, 0);
}


int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

