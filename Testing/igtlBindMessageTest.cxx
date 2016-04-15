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
#include "igtlutil/igtl_test_data_bind.h"
#include "igtlutil/igtl_test_data_image.h"
#include "igtlTransformMessage.h"
#include "igtlImageMessage2.h"
#include "igtlSensorMessage.h"
#include "igtlMessageDebugFunction.h"
#include "igtlUnit.h"
#include "igtl_unit.h"
#include "igtl_header.h"
#include "igtl_image.h"
#include "igtl_transform.h"
#include "igtl_bind.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#define MESSAGE_BIND_HEADER_SIZE 98
#define MESSAGE_BIND_BODY_SIZE 2678
#define TEST_IMAGE_MESSAGE_SIZE 2500
#define MESSAGE_SENSOR_BODY_SIZE 58

igtl::ImageMessage2::Pointer imageSendMsg2 = igtl::ImageMessage2::New();
igtl::ImageMessage2::Pointer imageReceiveMsg2 = igtl::ImageMessage2::New();
igtl::SensorMessage::Pointer sensorDataSendMsg = igtl::SensorMessage::New();
igtl::SensorMessage::Pointer sensorDataReceiveMsg = igtl::SensorMessage::New();
igtl::TransformMessage::Pointer transformSendMsg = igtl::TransformMessage::New();
igtl::TransformMessage::Pointer transformReceiveMsg = igtl::TransformMessage::New();
igtl::BindMessage::Pointer bindSendMsg = igtl::BindMessage::New();
igtl::BindMessage::Pointer bindReceiveMsg = igtl::BindMessage::New();

igtl::GetBindMessage::Pointer getBindMessage = igtl::GetBindMessage::New();
igtl::StartBindMessage::Pointer startBindMessage = igtl::StartBindMessage::New();


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
igtlFloat64 sensorValues[6] = {123456.78,12345.678,1234.5678,123.45678,12.345678,1.2345678};

void BuildUpElements()
{
  imageSendMsg2 = igtl::ImageMessage2::New();
  imageSendMsg2->SetTimeStamp(0, 1234567890);
  imageSendMsg2->SetDeviceName("ChildImage");
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
  imageSendMsg2->AllocatePack(IGTL_IMAGE_HEADER_SIZE+TEST_IMAGE_MESSAGE_SIZE);
  imageSendMsg2->AllocateScalars();
  memcpy(imageSendMsg2->GetPackFragmentPointer(1), (void*)(test_image_message+IGTL_HEADER_SIZE), IGTL_IMAGE_HEADER_SIZE);//here m_Body is set.
  imageSendMsg2->SetScalarPointer((void*)test_image);
  imageSendMsg2->Pack();
  
  imageReceiveMsg2->AllocatePack(imageSendMsg2->GetPackSize());
  imageReceiveMsg2->AllocateScalars();
  
  sensorDataSendMsg->AllocatePack();
  sensorDataSendMsg->SetLength(6);
  sensorDataSendMsg->SetDeviceName("ChildSensor");
  sensorDataSendMsg->SetTimeStamp(0, 1234567890);
  igtl::Unit::Pointer unit = igtl::Unit::New();
  unit->Init();
  unit->SetPrefix(IGTL_UNIT_PREFIX_NONE);
  unit->Append(IGTL_UNIT_SI_BASE_METER, (igtl_int8) 1);
  unit->Append(IGTL_UNIT_SI_BASE_SECOND, (igtl_int8) -2);
  unit->Pack();
  sensorDataSendMsg->SetUnit(unit);
  for (int i =0; i < 6; i++)
  {
    sensorDataSendMsg->SetValue(i, sensorValues[i]);
  }
  sensorDataSendMsg->Pack();
  
  sensorDataReceiveMsg->AllocatePack();
  sensorDataReceiveMsg->SetLength(6);
  
  transformSendMsg->AllocatePack();
  transformSendMsg->SetTimeStamp(0, 1234567890);
  transformSendMsg->SetDeviceName("ChildTrans");
  transformSendMsg->SetMatrix(inMatrix);
  transformSendMsg->Pack();
  
  transformReceiveMsg->AllocatePack();
  
  bindSendMsg->Init();
  bindSendMsg->SetTimeStamp(0, 1234567890);
  bindSendMsg->SetDeviceName("DeviceName");
  bindSendMsg->AppendChildMessage(transformSendMsg);
  bindSendMsg->AppendChildMessage(imageSendMsg2);
  bindSendMsg->AppendChildMessage(sensorDataSendMsg);
  bindSendMsg->Pack();
}

TEST(BindMessageTest, Pack)
{
  BuildUpElements();

  int r = memcmp((const void*)bindSendMsg->GetPackPointer(), (const void*)test_bind_message_header,
                 (size_t)(IGTL_HEADER_SIZE));
  EXPECT_EQ(r, 0);
  
  r = memcmp((const void*)bindSendMsg->GetPackBodyPointer(), (const void*)test_bind_message_bind_header,  MESSAGE_BIND_HEADER_SIZE);  
  EXPECT_EQ(r, 0);
  char * messageBody = (char*)bindSendMsg->GetPackBodyPointer() + MESSAGE_BIND_HEADER_SIZE;
  TestDebugCharArrayCmp((void*)messageBody,test_bind_message_bind_body,MESSAGE_BIND_BODY_SIZE);
  r = memcmp((const void*)(messageBody), (const void*)test_bind_message_bind_body,  MESSAGE_BIND_BODY_SIZE);
  EXPECT_EQ(r, 0);
}

TEST(BindMessageTest, Unpack)
{
  BuildUpElements();
  igtl::ImageMessage2::Pointer temp= igtl::ImageMessage2::New();
  bindReceiveMsg = igtl::BindMessage::New();
  bindReceiveMsg->AppendChildMessage(transformReceiveMsg);
  bindReceiveMsg->AppendChildMessage(imageReceiveMsg2);
  bindReceiveMsg->AppendChildMessage(sensorDataReceiveMsg);
  bindReceiveMsg->AllocatePack();
  memcpy(bindReceiveMsg->GetPackPointer(), bindSendMsg->GetPackPointer(), IGTL_HEADER_SIZE + MESSAGE_BIND_HEADER_SIZE + MESSAGE_BIND_BODY_SIZE);
  bindReceiveMsg->Unpack();
  bindReceiveMsg->GetChildMessage(0, transformReceiveMsg);
  bindReceiveMsg->GetChildMessage(1, imageReceiveMsg2);
  bindReceiveMsg->GetChildMessage(2, sensorDataReceiveMsg);
  
  igtl_header *messageHeader = (igtl_header *)bindReceiveMsg->GetPackPointer();
  EXPECT_STREQ(messageHeader->device_name, "DeviceName");
  EXPECT_STREQ(messageHeader->name, "BIND");
  EXPECT_EQ(messageHeader->version, 1);
  EXPECT_EQ(messageHeader->timestamp, 1234567890);
  EXPECT_EQ(messageHeader->body_size, MESSAGE_BIND_HEADER_SIZE + MESSAGE_BIND_BODY_SIZE);
  int r = memcmp(bindReceiveMsg->GetPackBodyPointer(), test_bind_message_bind_header, MESSAGE_BIND_HEADER_SIZE);
  EXPECT_EQ(r, 0);
  
  igtl_header *imageHeader = (igtl_header *)imageReceiveMsg2->GetPackFragmentPointer(0);
  EXPECT_STREQ(imageHeader->device_name, "ChildImage");
  EXPECT_STREQ(imageHeader->name, "IMAGE");
  EXPECT_EQ(imageHeader->version, 1);
  EXPECT_EQ(imageHeader->timestamp, 1234567890);
  EXPECT_EQ(imageHeader->body_size, IGTL_IMAGE_HEADER_SIZE+TEST_IMAGE_MESSAGE_SIZE);
  
  
  int returnSize[3] = {0,0,0};
  imageReceiveMsg2->GetDimensions(returnSize);
  EXPECT_THAT(returnSize,testing::ElementsAreArray(size));
  float returnSpacing[3] = {0.0f,0.0f,0.0f};
  imageReceiveMsg2->GetSpacing(returnSpacing);
  for(int i=0;i < 3; i++)
  {
    EXPECT_NEAR(returnSpacing[i], spacing[i], ABS_ERROR);
  }
  //testing::internal::FloatingEqMatcher<float> a(1e-8,false);
  //EXPECT_THAT(returnSpacing,testing::Pointwise(a, spacing));
  int returnSvsize[3] = {0,0,0}, returnSvoffset[3] = {0,0,0};
  imageReceiveMsg2->GetSubVolume(returnSvsize, returnSvoffset);
  EXPECT_THAT(returnSvsize,testing::ElementsAreArray(svsize));
  EXPECT_THAT(returnSvoffset,testing::ElementsAreArray(svoffset));
  EXPECT_EQ(imageReceiveMsg2->GetScalarType(), IGTL_IMAGE_STYPE_TYPE_UINT8);
  EXPECT_EQ(imageReceiveMsg2->GetEndian(), IGTL_IMAGE_ENDIAN_LITTLE);
  EXPECT_EQ(imageReceiveMsg2->GetCoordinateSystem(), IGTL_IMAGE_COORD_RAS);
  
  r = memcmp(imageReceiveMsg2->GetPackFragmentPointer(2), test_image, TEST_IMAGE_MESSAGE_SIZE);
  EXPECT_EQ(r, 0);

  igtl_header *transformHeader = (igtl_header *)transformReceiveMsg->GetPackPointer();
  EXPECT_STREQ(transformHeader->device_name, "ChildTrans");
  EXPECT_STREQ(transformHeader->name, "TRANSFORM");
  EXPECT_EQ(transformHeader->version, 1);
  EXPECT_EQ(transformHeader->timestamp, 1234567890);
  EXPECT_EQ(transformHeader->body_size, IGTL_TRANSFORM_SIZE);
 
  igtl::Matrix4x4 outMatrix = {{0.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,0.0}};
  transformReceiveMsg->GetMatrix(outMatrix);
  EXPECT_TRUE(MatrixComparison(outMatrix, inMatrix, ABS_ERROR));
  
  igtl_header *sensorHeader = (igtl_header *)sensorDataReceiveMsg->GetPackPointer();
  EXPECT_STREQ(sensorHeader->device_name, "ChildSensor");
  EXPECT_STREQ(sensorHeader->name, "SENSOR");
  EXPECT_EQ(sensorHeader->version, 1);
  EXPECT_EQ(sensorHeader->timestamp, 1234567890);
  EXPECT_EQ(sensorHeader->body_size, MESSAGE_SENSOR_BODY_SIZE);
  
  igtl::igtlUnit unitTruth = 0x443E0000000000;
  EXPECT_EQ(sensorDataReceiveMsg->GetUnit(), unitTruth);
  EXPECT_EQ(sensorDataReceiveMsg->GetLength(),6);
  EXPECT_EQ(sensorDataReceiveMsg->GetValue(0),123456.78);
  EXPECT_EQ(sensorDataReceiveMsg->GetValue(1),12345.678);
  EXPECT_EQ(sensorDataReceiveMsg->GetValue(2),1234.5678);
  EXPECT_EQ(sensorDataReceiveMsg->GetValue(3),123.45678);
  EXPECT_EQ(sensorDataReceiveMsg->GetValue(4),12.345678);
  EXPECT_EQ(sensorDataReceiveMsg->GetValue(5),1.2345678);
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

