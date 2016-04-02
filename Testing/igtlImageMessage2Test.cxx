/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 Date:      $Date: 2016/02/12 19:53:38 $
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "../Source/igtlImageMessage2.h"
#include "igtl_types.h"
#include "igtl_header.h"
#include "igtl_image.h"
#include "igtl_util.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
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

TEST(imageMessage2Test, subVolume)
{
  int inSubDimension[3] = {25,25,1}; 
  int inSubOffset[3] = {25,25,0}; 
  imageMessage2Test->SetSubVolume(inSubDimension, inSubOffset); 
  int outSubDimension[3] = {0,0,0}; 
  int outSubOffset[3] = {0,0,0}; 
  imageMessage2Test->GetSubVolume(outSubDimension,outSubOffset); 
  EXPECT_THAT(outSubDimension, testing::ElementsAreArray(inSubDimension));
  EXPECT_THAT(outSubOffset, testing::ElementsAreArray(inSubOffset));
  inSubDimension[0] = 30, inSubDimension[1] = 30, inSubDimension[2] = 1;
  inSubOffset[0] = 20, inSubOffset[1] = 20, inSubOffset[2] = 0;
  imageMessage2Test->SetSubVolume(inSubDimension[0], inSubDimension[1], inSubDimension[2], inSubOffset[0], inSubOffset[1], inSubOffset[2]); 
  imageMessage2Test->GetSubVolume(outSubDimension[0], outSubDimension[1], outSubDimension[2], outSubOffset[0], outSubOffset[1], outSubOffset[2]); 
  EXPECT_THAT(outSubDimension, testing::ElementsAreArray(inSubDimension));
  EXPECT_THAT(outSubOffset, testing::ElementsAreArray(inSubOffset));
}


TEST(imageMessage2Test, Dimension)
{
  int inDimension[3] = {50,50,1}; 
  imageMessage2Test->SetDimensions(inDimension); 
  int outDimension[3] = {0,0,0}; 
  imageMessage2Test->GetDimensions(outDimension); 
  EXPECT_THAT(outDimension, testing::ElementsAreArray(inDimension));
  inDimension[0] = 30, inDimension[1] = 30, inDimension[2] = 1;
  imageMessage2Test->SetDimensions(inDimension[0], inDimension[1], inDimension[2]); 
  imageMessage2Test->GetDimensions(outDimension[0], outDimension[1], outDimension[2]); 
  EXPECT_THAT(outDimension, testing::ElementsAreArray(inDimension));
}


TEST(imageMessage2Test, Spacing)
{
  float inSpacing[3] = {1.0,1.0,1.0}; 
  imageMessage2Test->SetSpacing(inSpacing); 
  float outSpacing[3] = {0.0,0.0,0.0}; 
  imageMessage2Test->GetSpacing(outSpacing); 
  EXPECT_THAT(outSpacing, testing::ElementsAreArray(inSpacing));
  inSpacing[0] = 0.5, inSpacing[1] = 0.5, inSpacing[2] = 0.5;
  imageMessage2Test->SetSpacing(inSpacing[0], inSpacing[1], inSpacing[2]); 
  imageMessage2Test->GetSpacing(outSpacing[0], outSpacing[1], outSpacing[2]); 
  EXPECT_THAT(outSpacing, testing::ElementsAreArray(inSpacing));
}

TEST(imageMessage2Test, Origin)
{
  float inOrigin[3] = {0.0,0.0,0.0}; 
  imageMessage2Test->SetOrigin(inOrigin); 
  float outOrigin[3] = {10.0,10.0,10.0}; 
  imageMessage2Test->GetOrigin(outOrigin); 
  EXPECT_THAT(outOrigin, testing::ElementsAreArray(inOrigin));
  inOrigin[0] = 0.5, inOrigin[1] = 0.5, inOrigin[2] = 0.5;
  imageMessage2Test->SetOrigin(inOrigin[0], inOrigin[1], inOrigin[2]); 
  imageMessage2Test->GetOrigin(outOrigin[0], outOrigin[1], outOrigin[2]); 
  EXPECT_THAT(outOrigin, testing::ElementsAreArray(inOrigin));
}


TEST(imageMessage2Test, Normals)
{
  float inT[3] = {1.0, 0.0, 0.0};
  float inS[3] = {0.0, 0.707, -0.707};
  float inN[3] = {0.0, 0.707, 0.707};
  float inNormals[3][3] = {{inT[0],inS[0],inN[0]},
                           {inT[1],inS[1],inN[1]},
                           {inT[2],inS[2],inN[2]}};
  imageMessage2Test->SetNormals(inNormals);
  float outNormals[3][3] = {{0.0,0.0,0.0},
                            {0.0,0.0,0.0},
                            {0.0,0.0,0.0}};
  imageMessage2Test->GetNormals(outNormals);
  EXPECT_THAT(outNormals, testing::ElementsAre(testing::ElementsAreArray(inNormals[0]),
                                               testing::ElementsAreArray(inNormals[1]),
                                               testing::ElementsAreArray(inNormals[2])));
  inT[0] = 0.707, inT[1] = 0.0, inT[2] = -0.707;
  inS[0] = 0.0, inS[1] = 1.0, inS[2] = 0.0;
  inN[0] = 0.707, inN[1] = 0.0, inN[2] = 0.707;
  float outT[3] = {0.0, 0.0, 0.0};
  float outS[3] = {0.0, 0.0, 0.0};
  float outN[3] = {0.0, 0.0, 0.0};
  imageMessage2Test->SetNormals(inT, inS, inN); 
  imageMessage2Test->GetNormals(outT, outS, outN); 
  EXPECT_THAT(outT, testing::ElementsAreArray(inT));
  EXPECT_THAT(outS, testing::ElementsAreArray(inS));
  EXPECT_THAT(outN, testing::ElementsAreArray(inN));
}

TEST(imageMessage2Test, Matrix)
{
  float inT[4] = {1.0, 0.0, 0.0, 0.0};
  float inS[4] = {0.0, 0.707, -0.707, 0.0};
  float inN[4] = {0.0, 0.707, 0.707, 0.0};
  float inOrigin[4] = {10.0,10.0,10.0, 1.0}; 
  igtl::Matrix4x4 inMatrix = {{inT[0],inS[0],inN[0],inOrigin[0]},
                              {inT[1],inS[1],inN[1],inOrigin[1]},
                              {inT[2],inS[2],inN[2],inOrigin[2]},
                              {inT[3],inS[3],inN[3],inOrigin[3]}}; 
  imageMessage2Test->SetMatrix(inMatrix); 
  igtl::Matrix4x4 outMatrix = {{0.0,0.0,0.0,0.0},
                               {0.0,0.0,0.0,0.0},
                               {0.0,0.0,0.0,0.0},
                               {0.0,0.0,0.0,0.0}}; 
  imageMessage2Test->GetMatrix(outMatrix); 
  EXPECT_THAT(outMatrix, testing::ElementsAre(testing::ElementsAreArray(inMatrix[0]),
                                              testing::ElementsAreArray(inMatrix[1]),
                                              testing::ElementsAreArray(inMatrix[2]),
                                              testing::ElementsAreArray(inMatrix[3])));
}

TEST(imageMessage2Test, SetScalarPointer)
{
  int TEST_FRAGMENTIMAGE_MESSAGE_SIZE = TEST_IMAGE_MESSAGE_SIZE/2;
  unsigned char test_fragmentImage[TEST_FRAGMENTIMAGE_MESSAGE_SIZE];
  memcpy(test_fragmentImage, test_image_message+IGTL_HEADER_SIZE+IGTL_IMAGE_HEADER_SIZE+TEST_FRAGMENTIMAGE_MESSAGE_SIZE, TEST_FRAGMENTIMAGE_MESSAGE_SIZE); // copy the last half of the image
  unsigned char * charwholeImagePointer = (unsigned char *)imageMessage2Test->GetScalarPointer();
  EXPECT_EQ(strcmp((const char *)charwholeImagePointer, (const char *)(test_image_message+IGTL_HEADER_SIZE+IGTL_IMAGE_HEADER_SIZE)),0);
  imageMessage2Test->SetScalarPointer((void*)test_fragmentImage);
  unsigned char * fragmentImagePointer = (unsigned char *)imageMessage2Test->GetScalarPointer();
  EXPECT_NE(strcmp((const char *)charwholeImagePointer, (const char *)fragmentImagePointer),0);
  EXPECT_EQ(strcmp((const char *)fragmentImagePointer, (const char *)(test_image_message+IGTL_HEADER_SIZE+IGTL_IMAGE_HEADER_SIZE+TEST_FRAGMENTIMAGE_MESSAGE_SIZE)),0);
}

TEST(imageMessage2Test, Pack)
{
  imageMessage2Test->Pack();
  igtl_header *messageHead = (igtl_header *)imageMessage2Test->GetPackFragmentPointer(0);
  EXPECT_STREQ(messageHead->device_name, "");
  EXPECT_STREQ(messageHead->name, "IMAGE");
  EXPECT_EQ(messageHead->version, 256);
  //EXPECT_EQ(messageHead->body_size, 972);
  //EXPECT_EQ(messageHead->timestamp, 1407374883553280); the commended test cannot be checked
}

int main(int argc, char **argv)
{
  setupTest();
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

