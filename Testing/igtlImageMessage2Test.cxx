/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlImageMessage2.h"
#include "igtlutil/igtl_test_data_image.h"
#include "igtlMessageDebugFunction.h"
#include "igtl_image.h"
#include "igtl_types.h"
#include "igtl_header.h"
#include "igtl_util.h"
#include "igtlTestConfig.h"
#include "string.h"

igtl::ImageMessage2::Pointer imageSendMsg2 = igtl::ImageMessage2::New();
igtl::ImageMessage2::Pointer imageReceiveMsg2 = igtl::ImageMessage2::New();

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
int   scalarType = igtl::ImageMessage2::TYPE_UINT8;// scalar type

void BuildUp()
{
  imageSendMsg2 = igtl::ImageMessage2::New();
  imageSendMsg2->SetTimeStamp(0, 1234567892);
  imageSendMsg2->SetDeviceName("DeviceName");
  //Initialization of a image message
  imageSendMsg2->SetDimensions(size);
  imageSendMsg2->SetSpacing(spacing);
  imageSendMsg2->SetScalarType(scalarType);
  imageSendMsg2->SetSubVolume(svsize, svoffset);
  imageSendMsg2->SetNumComponents(1);
  imageSendMsg2->SetScalarType(IGTL_IMAGE_STYPE_TYPE_UINT8);
  imageSendMsg2->SetEndian(IGTL_IMAGE_ENDIAN_LITTLE);
  imageSendMsg2->SetCoordinateSystem(IGTL_IMAGE_COORD_RAS);
  imageSendMsg2->SetMatrix(inMatrix);
  imageSendMsg2->AllocateBuffer(IGTL_IMAGE_HEADER_SIZE+TEST_IMAGE_MESSAGE_SIZE);
  imageSendMsg2->AllocateScalars();
  memcpy(imageSendMsg2->GetPackFragmentPointer(1), (void*)(test_image_message+IGTL_HEADER_SIZE), IGTL_IMAGE_HEADER_SIZE);//here m_ImageHeader is set.
  imageSendMsg2->SetScalarPointer((void*)test_image); //m_Image and m_Body are set
  imageSendMsg2->Pack();
}

TEST(ImageMessage2Test, Pack)
{
  BuildUp();
  int r = memcmp((const void*)imageSendMsg2->GetPackPointer(), (const void*)test_image_message,
                 (size_t)(IGTL_HEADER_SIZE));
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)imageSendMsg2->GetPackFragmentPointer(1), (const void*)(test_image_message+IGTL_HEADER_SIZE), (size_t)(IGTL_IMAGE_HEADER_SIZE));
  EXPECT_EQ(r, 0);
}

TEST(ImageMessage2Test, Unpack)
{
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->AllocatePack();
  memcpy(headerMsg->GetPackPointer(), (const void*)imageSendMsg2->GetPackPointer(), IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  imageReceiveMsg2->SetMessageHeader(headerMsg);
  imageReceiveMsg2->SetDimensions(50, 50, 1);
  imageReceiveMsg2->SetSubVolume(50, 50, 1, 0, 0, 0);
  imageReceiveMsg2->AllocateScalars();
  memcpy(imageReceiveMsg2->GetPackFragmentPointer(0), imageSendMsg2->GetPackFragmentPointer(0), IGTL_HEADER_SIZE);
  
  memcpy(imageReceiveMsg2->GetPackBodyPointer(), imageSendMsg2->GetPackBodyPointer(), IGTL_IMAGE_HEADER_SIZE+TEST_IMAGE_MESSAGE_SIZE);
  imageReceiveMsg2->Unpack();
  igtl_header *messageHeader = (igtl_header *)headerMsg->GetPackPointer();
  EXPECT_STREQ(messageHeader->device_name, "DeviceName");
  EXPECT_STREQ(messageHeader->name, "IMAGE");
  EXPECT_EQ(messageHeader->header_version, 1);
  EXPECT_EQ(messageHeader->timestamp, 1234567892);
  EXPECT_EQ(messageHeader->body_size, IGTL_IMAGE_HEADER_SIZE + TEST_IMAGE_MESSAGE_SIZE);
  
  int returnSize[3] = {0,0,0};
  imageReceiveMsg2->GetDimensions(returnSize);
  EXPECT_THAT(returnSize,testing::ElementsAreArray(size));
  float returnSpacing[3] = {0.0f,0.0f,0.0f};
  imageReceiveMsg2->GetSpacing(returnSpacing);
  for(int i=0;i < 3; i++)
    {
    EXPECT_NEAR(returnSpacing[i], spacing[i], ABS_ERROR);
    }
  int returnSvsize[3] = {0,0,0}, returnSvoffset[3] = {0,0,0};
  imageReceiveMsg2->GetSubVolume(returnSvsize, returnSvoffset);
  EXPECT_THAT(returnSvsize,testing::ElementsAreArray(svsize));
  EXPECT_THAT(returnSvoffset,testing::ElementsAreArray(svoffset));
  EXPECT_EQ(imageReceiveMsg2->GetScalarType(), IGTL_IMAGE_STYPE_TYPE_UINT8);
  EXPECT_EQ(imageReceiveMsg2->GetEndian(), IGTL_IMAGE_ENDIAN_LITTLE);
  EXPECT_EQ(imageReceiveMsg2->GetCoordinateSystem(), IGTL_IMAGE_COORD_RAS);
  igtl::Matrix4x4 outMatrix = {{0.0,0.0,0.0,0.0},
                               {0.0,0.0,0.0,0.0},
                               {0.0,0.0,0.0,0.0},
                               {0.0,0.0,0.0,0.0}};
  imageReceiveMsg2->GetMatrix(outMatrix);
  EXPECT_TRUE(MatrixComparison(outMatrix, inMatrix, ABS_ERROR));
  int r = memcmp(imageReceiveMsg2->GetPackFragmentPointer(2), (unsigned char*)test_image, TEST_IMAGE_MESSAGE_SIZE);
  EXPECT_EQ(r, 0);
}

TEST(ImageMessage2Test, FragmentImageTest)
{
  EXPECT_TRUE(true);
  /*
    int TEST_FRAGMENTIMAGE_MESSAGE_SIZE = TEST_IMAGE_MESSAGE_SIZE/2;
    unsigned char test_fragmentImage[TEST_FRAGMENTIMAGE_MESSAGE_SIZE];
    memcpy(test_fragmentImage, test_image_message+IGTL_HEADER_SIZE+IGTL_IMAGE_HEADER_SIZE+TEST_FRAGMENTIMAGE_MESSAGE_SIZE, TEST_FRAGMENTIMAGE_MESSAGE_SIZE); // copy the last half of the image
    unsigned char * charwholeImagePointer = (unsigned char *)imageSendMsg2->GetScalarPointer();
    EXPECT_EQ(strcmp((const char *)charwholeImagePointer, (const char *)test_image), 0);
    imageSendMsg2->SetScalarPointer((void*)test_fragmentImage);
    unsigned char * fragmentImagePointer = (unsigned char *)imageSendMsg2->GetScalarPointer();
    EXPECT_NE(strcmp((const char *)charwholeImagePointer, (const char *)fragmentImagePointer),0);
    EXPECT_EQ(strcmp((const char *)fragmentImagePointer, (const char *)(test_image_message+IGTL_HEADER_SIZE+IGTL_IMAGE_HEADER_SIZE+TEST_FRAGMENTIMAGE_MESSAGE_SIZE)),0);
  */
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

