/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 Date:      $Date: 2016/02/12 19:53:38 $
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "../Source/igtlTransformMessage.h"
#include "igtl_header.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "igtl_transform.h"
#include "igtlOSUtil.h"
#include "../Testing/igtlutil/igtl_test_data_transform.h"

igtl::TransformMessage::Pointer transformMsg = igtl::TransformMessage::New();
igtl::TransformMessage::Pointer transformMsg2 = igtl::TransformMessage::New();
float inT[4] = {-0.954892f, 0.196632f, -0.222525f, 0.0};
float inS[4] = {-0.196632f, 0.142857f, 0.970014f, 0.0};
float inN[4] = {0.222525f, 0.970014f, -0.0977491f, 0.0};
float inOrigin[4] = {46.0531f,19.4709f,46.0531f, 1.0};
igtl::Matrix4x4 inMatrix = {{inT[0],inS[0],inN[0],inOrigin[0]},
  {inT[1],inS[1],inN[1],inOrigin[1]},
  {inT[2],inS[2],inN[2],inOrigin[2]},
  {inT[3],inS[3],inN[3],inOrigin[3]}};

TEST(TransformMessageTest, Pack)
{
  transformMsg->AllocatePack();
  transformMsg->SetTimeStamp(0, 1234567890);
  transformMsg->SetDeviceName("DeviceName");
  transformMsg->SetMatrix(inMatrix);
  transformMsg->Pack();
  int r = memcmp((const void*)transformMsg->GetPackPointer(), (const void*)test_transform_message,
                 (size_t)(IGTL_HEADER_SIZE));
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)transformMsg->GetPackBodyPointer(), (const void*)(test_transform_message+IGTL_HEADER_SIZE), IGTL_TRANSFORM_SIZE);
  EXPECT_EQ(r, 0);
}


TEST(TransformMessageTest, Unpack)
{
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->AllocatePack();
  memcpy(headerMsg->GetPackPointer(), (const void*)test_transform_message, IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  transformMsg2->SetMessageHeader(headerMsg);
  transformMsg2->AllocatePack();
  
  memcpy(transformMsg2->GetPackBodyPointer(), transformMsg->GetPackBodyPointer(), transformMsg2->GetPackBodySize());
  transformMsg2->Unpack();
  
  igtl_header *messageHeader = (igtl_header *)transformMsg2->GetPackPointer();
  EXPECT_STREQ(messageHeader->device_name, "DeviceName");
  EXPECT_STREQ(messageHeader->name, "TRANSFORM");
  EXPECT_EQ(messageHeader->version, 1);
  EXPECT_EQ(messageHeader->timestamp, 1234567890);
  EXPECT_EQ(messageHeader->body_size, IGTL_TRANSFORM_SIZE);
  
  igtl::Matrix4x4 outMatrix = {{0.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,0.0}};
  transformMsg2->GetMatrix(outMatrix);
  EXPECT_THAT(outMatrix, testing::ElementsAre(testing::ElementsAreArray(inMatrix[0]),
                                              testing::ElementsAreArray(inMatrix[1]),
                                              testing::ElementsAreArray(inMatrix[2]),
                                              testing::ElementsAreArray(inMatrix[3])));
  
}



int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

