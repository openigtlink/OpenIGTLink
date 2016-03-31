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
#include "gmock/gmock.h"
#include "igtlutil/igtl_test_data_image.h"

igtl::ImageMessage::Pointer imageMessageTest = igtl::ImageMessage::New();


void setupTest()
{
  igtl::MessageHeader::Pointer messageHeader = igtl::MessageHeader::New();
  messageHeader->AllocatePack();
  memcpy(messageHeader->GetPackPointer(), test_image_message, IGTL_HEADER_SIZE);
  messageHeader->Unpack(0);
  
  imageMessageTest->AllocateScalars();
  imageMessageTest->Copy(messageHeader);
  imageMessageTest->AllocatePack();
  memcpy((void*)imageMessageTest->GetPackBodyPointer(), test_image_message+IGTL_HEADER_SIZE, IGTL_IMAGE_HEADER_SIZE+TEST_IMAGE_MESSAGE_SIZE);//here m_Body is set.
  igtl_image_header imageHeader;
  memcpy((void*)&imageHeader, test_image_message+IGTL_HEADER_SIZE, IGTL_IMAGE_HEADER_SIZE);
  igtl_image_convert_byte_order(&imageHeader);
  imageHeader.version = IGTL_HEADER_VERSION;
  imageMessageTest->SetSubVolume((int*)imageHeader.subvol_size, (int*)imageHeader.subvol_offset);
  imageMessageTest->SetDimensions((int*)imageHeader.size);
  imageMessageTest->SetScalarType(igtl::ImageMessage::TYPE_UINT8);
  imageMessageTest->Unpack(0); // use the crc from the m_header
}


TEST(ImageMessageTest, subVolume)
{
  int inSubDimension[3] = {25,25,1}; 
  int inSubOffset[3] = {25,25,0}; 
  imageMessageTest->SetSubVolume(inSubDimension, inSubOffset); 
  int outSubDimension[3] = {0,0,0}; 
  int outSubOffset[3] = {0,0,0}; 
  imageMessageTest->GetSubVolume(outSubDimension,outSubOffset); 
  EXPECT_THAT(outSubDimension, testing::ElementsAreArray(inSubDimension));
  EXPECT_THAT(outSubOffset, testing::ElementsAreArray(inSubOffset));
  inSubDimension[0] = 30, inSubDimension[1] = 30, inSubDimension[2] = 1;
  inSubOffset[0] = 20, inSubOffset[1] = 20, inSubOffset[2] = 0;
  imageMessageTest->SetSubVolume(inSubDimension[0], inSubDimension[1], inSubDimension[2], inSubOffset[0], inSubOffset[1], inSubOffset[2]); 
  imageMessageTest->GetSubVolume(outSubDimension[0], outSubDimension[1], outSubDimension[2], outSubOffset[0], outSubOffset[1], outSubOffset[2]); 
  EXPECT_THAT(outSubDimension, testing::ElementsAreArray(inSubDimension));
  EXPECT_THAT(outSubOffset, testing::ElementsAreArray(inSubOffset));
}


TEST(ImageMessageTest, Dimension)
{
  int inDimension[3] = {50,50,1}; 
  imageMessageTest->SetDimensions(inDimension); 
  int outDimension[3] = {0,0,0}; 
  imageMessageTest->GetDimensions(outDimension); 
  EXPECT_THAT(outDimension, testing::ElementsAreArray(inDimension));
  inDimension[0] = 30, inDimension[1] = 30, inDimension[2] = 1;
  imageMessageTest->SetDimensions(inDimension[0], inDimension[1], inDimension[2]); 
  imageMessageTest->GetDimensions(outDimension[0], outDimension[1], outDimension[2]); 
  EXPECT_THAT(outDimension, testing::ElementsAreArray(inDimension));
}


TEST(ImageMessageTest, Spacing)
{
  float inSpacing[3] = {1.0,1.0,1.0}; 
  imageMessageTest->SetSpacing(inSpacing); 
  float outSpacing[3] = {0.0,0.0,0.0}; 
  imageMessageTest->GetSpacing(outSpacing); 
  EXPECT_THAT(outSpacing, testing::ElementsAreArray(inSpacing));
  inSpacing[0] = 0.5, inSpacing[1] = 0.5, inSpacing[2] = 0.5;
  imageMessageTest->SetSpacing(inSpacing[0], inSpacing[1], inSpacing[2]); 
  imageMessageTest->GetSpacing(outSpacing[0], outSpacing[1], outSpacing[2]); 
  EXPECT_THAT(outSpacing, testing::ElementsAreArray(inSpacing));
}

TEST(ImageMessageTest, Origin)
{
  float inOrigin[3] = {0.0,0.0,0.0}; 
  imageMessageTest->SetOrigin(inOrigin); 
  float outOrigin[3] = {10.0,10.0,10.0}; 
  imageMessageTest->GetOrigin(outOrigin); 
  EXPECT_THAT(outOrigin, testing::ElementsAreArray(inOrigin));
  inOrigin[0] = 0.5, inOrigin[1] = 0.5, inOrigin[2] = 0.5;
  imageMessageTest->SetOrigin(inOrigin[0], inOrigin[1], inOrigin[2]); 
  imageMessageTest->GetOrigin(outOrigin[0], outOrigin[1], outOrigin[2]); 
  EXPECT_THAT(outOrigin, testing::ElementsAreArray(inOrigin));
}


TEST(ImageMessageTest, Normals)
{
  float inT[3] = {1.0, 0.0, 0.0};
  float inS[3] = {0.0, 0.707, -0.707};
  float inN[3] = {0.0, 0.707, 0.707};
  float inNormals[3][3] = {inT[0],inS[0],inN[0],
                           inT[1],inS[1],inN[1],
                           inT[2],inS[2],inN[2]}; 
  imageMessageTest->SetNormals(inNormals); 
  float outNormals[3][3] = {0.0,0.0,0.0,
                            0.0,0.0,0.0,
                            0.0,0.0,0.0}; 
  imageMessageTest->GetNormals(outNormals); 
  EXPECT_THAT(outNormals, testing::ElementsAre(testing::ElementsAreArray(inNormals[0]),
                                               testing::ElementsAreArray(inNormals[1]),
                                               testing::ElementsAreArray(inNormals[2])));
  inT[0] = 0.707, inT[1] = 0.0, inT[2] = -0.707;
  inS[0] = 0.0, inS[1] = 1.0, inS[2] = 0.0;
  inN[0] = 0.707, inN[1] = 0.0, inN[2] = 0.707;
  float outT[3] = {0.0, 0.0, 0.0};
  float outS[3] = {0.0, 0.0, 0.0};
  float outN[3] = {0.0, 0.0, 0.0};
  imageMessageTest->SetNormals(inT, inS, inN); 
  imageMessageTest->GetNormals(outT, outS, outN); 
  EXPECT_THAT(outT, testing::ElementsAreArray(inT));
  EXPECT_THAT(outS, testing::ElementsAreArray(inS));
  EXPECT_THAT(outN, testing::ElementsAreArray(inN));
}

TEST(ImageMessageTest, Matrix)
{
  float inT[4] = {1.0, 0.0, 0.0, 0.0};
  float inS[4] = {0.0, 0.707, -0.707, 0.0};
  float inN[4] = {0.0, 0.707, 0.707, 0.0};
  float inOrigin[4] = {10.0,10.0,10.0, 1.0}; 
  igtl::Matrix4x4 inMatrix = {inT[0],inS[0],inN[0],inOrigin[0],
                              inT[1],inS[1],inN[1],inOrigin[1],
                              inT[2],inS[2],inN[2],inOrigin[2],
                              inT[3],inS[3],inN[3],inOrigin[3]}; 
  imageMessageTest->SetMatrix(inMatrix); 
  igtl::Matrix4x4 outMatrix = {0.0,0.0,0.0,0.0,
                               0.0,0.0,0.0,0.0,
                               0.0,0.0,0.0,0.0,
                               0.0,0.0,0.0,0.0}; 
  imageMessageTest->GetMatrix(outMatrix); 
  EXPECT_THAT(outMatrix, testing::ElementsAre(testing::ElementsAreArray(inMatrix[0]),
                                              testing::ElementsAreArray(inMatrix[1]),
                                              testing::ElementsAreArray(inMatrix[2]),
                                              testing::ElementsAreArray(inMatrix[3])));
}


int main(int argc, char **argv)
{
  setupTest();
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

