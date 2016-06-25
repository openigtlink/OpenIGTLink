/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 Date:      $Date: 2016/02/12 19:53:38 $
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlTransformMessage.h"
#include "igtlutil/igtl_test_data_transform.h"
#include "igtlMessageDebugFunction.h"
#include "igtl_transform.h"
#include "igtl_header.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"


igtl::TransformMessage::Pointer transformSendMsg = igtl::TransformMessage::New();
igtl::TransformMessage::Pointer transformReceiveMsg = igtl::TransformMessage::New();
float inT[4] = {-0.954892f, 0.196632f, -0.222525f, 0.0};
float inS[4] = {-0.196632f, 0.142857f, 0.970014f, 0.0};
float inN[4] = {0.222525f, 0.970014f, -0.0977491f, 0.0};
float inOrigin[4] = {46.0531f,19.4709f,46.0531f, 1.0};
igtl::Matrix4x4 inMatrix = {{inT[0],inS[0],inN[0],inOrigin[0]},
  {inT[1],inS[1],inN[1],inOrigin[1]},
  {inT[2],inS[2],inN[2],inOrigin[2]},
  {inT[3],inS[3],inN[3],inOrigin[3]}};

TEST(TransformMessageTest, PackFormatVersion1)
{
  transformSendMsg->SetHeaderVersion(IGTL_HEADER_VERSION_1);
  transformSendMsg->AllocatePack();
  transformSendMsg->SetTimeStamp(0, 1234567892);
  transformSendMsg->SetDeviceName("DeviceName");
  transformSendMsg->SetMatrix(inMatrix);
  transformSendMsg->Pack();

  int r = memcmp((const void*)transformSendMsg->GetPackPointer(), (const void*)test_transform_message,
                 (size_t)(IGTL_HEADER_SIZE));
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)transformSendMsg->GetPackBodyPointer(), (const void*)(test_transform_message+IGTL_HEADER_SIZE), IGTL_TRANSFORM_SIZE);
  EXPECT_EQ(r, 0);
}


TEST(TransformMessageTest, UnpackFormatVersion1)
{
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->AllocatePack();
  memcpy(headerMsg->GetPackPointer(), (const void*)test_transform_message, IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  transformReceiveMsg->SetMessageHeader(headerMsg);
  transformReceiveMsg->AllocatePack();
  
  memcpy(transformReceiveMsg->GetPackBodyPointer(), transformSendMsg->GetPackBodyPointer(), transformReceiveMsg->GetPackBodySize());
  transformReceiveMsg->Unpack();
  
  igtl_header *messageHeader = (igtl_header *)transformReceiveMsg->GetPackPointer();
  EXPECT_STREQ(messageHeader->device_name, "DeviceName");
  EXPECT_STREQ(messageHeader->name, "TRANSFORM");
  EXPECT_EQ(messageHeader->header_version, 1);
  EXPECT_EQ(messageHeader->timestamp, 1234567892);
  EXPECT_EQ(messageHeader->body_size, IGTL_TRANSFORM_SIZE);
  
  igtl::Matrix4x4 outMatrix = {{0.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,0.0}};
  transformReceiveMsg->GetMatrix(outMatrix);
  EXPECT_TRUE(MatrixComparison(outMatrix, inMatrix, ABS_ERROR));
  
}

#if OpenIGTLink_PROTOCOL_VERSION >= 3
#include "igtlutil/igtl_test_data_transformFormat2.h"
TEST(TransformMessageTest, PackFormatVersion2)
{
  transformSendMsg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
  transformSendMsg->AllocatePack();
  transformSendMsg->SetTimeStamp(0, 1234567892);
  transformSendMsg->SetDeviceName("DeviceName");
  transformSendMsg->SetMatrix(inMatrix);
  unsigned short codingScheme = 3; // 3 corresponding to US-ASCII
  transformSendMsg->AddMetaDataElement("First patient age", codingScheme, "22");
  transformSendMsg->AddMetaDataElement("Second patient age",codingScheme, "25");
  transformSendMsg->SetMessageID(1);
  transformSendMsg->Pack();
  int r = memcmp((const void*)transformSendMsg->GetPackPointer(), (const void*)test_transform_message_Format2,
                 (size_t)(IGTL_HEADER_SIZE));
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)transformSendMsg->GetPackBodyPointer(), (const void*)(test_transform_message_Format2+IGTL_HEADER_SIZE), IGTL_TRANSFORM_SIZE);
  EXPECT_EQ(r, 0);
}

TEST(TransformMessageTest, UnpackFormatVersion2)
{
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->AllocatePack();
  memcpy(headerMsg->GetPackPointer(), (const void*)test_transform_message_Format2, IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  transformReceiveMsg->SetMessageHeader(headerMsg);
  transformReceiveMsg->AllocatePack();
  
  memcpy(transformReceiveMsg->GetPackBodyPointer(), transformSendMsg->GetPackBodyPointer(), transformReceiveMsg->GetPackBodySize());
  transformReceiveMsg->Unpack();
  
  igtl_header *messageHeader = (igtl_header *)transformReceiveMsg->GetPackPointer();
  EXPECT_STREQ(messageHeader->device_name, "DeviceName");
  EXPECT_STREQ(messageHeader->name, "TRANSFORM");
  EXPECT_EQ(messageHeader->header_version, 2);
  EXPECT_EQ(messageHeader->timestamp, 1234567892);
  EXPECT_EQ(messageHeader->body_size, transformSendMsg->GetPackBodySize());
  
  igtl::Matrix4x4 outMatrix = {{0.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,0.0}};
  transformReceiveMsg->GetMatrix(outMatrix);
  EXPECT_TRUE(MatrixComparison(outMatrix, inMatrix, ABS_ERROR));
  int i = 0;
  std::vector<std::string> groundTruth(0);
  groundTruth.push_back("First patient age");
  groundTruth.push_back("Second patient age");
  std::vector<std::string> groundTruthAge(0);
  groundTruthAge.push_back("22");
  groundTruthAge.push_back("25");
  EXPECT_EQ(transformReceiveMsg->GetMessageID(),1);
  for (std::map<std::string, std::string>::const_iterator it = transformReceiveMsg->GetMetaData().begin(); it != transformReceiveMsg->GetMetaData().end(); ++it, ++i)
  {
    EXPECT_STREQ(it->first.c_str(), groundTruth[i].c_str());
    EXPECT_EQ(transformReceiveMsg->GetMetaDataHeaderEntries()[i].value_encoding,3);
    EXPECT_STREQ(it->second.c_str(), groundTruthAge[i].c_str());
  }
  
}

#endif

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

