/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 Date:      $Date: 2016/02/12 19:53:38 $
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlTrackingDataMessage.h"
#include "igtlutil/igtl_test_data_tdata.h"
#include "igtlMessageDebugFunction.h"
#include "igtl_tdata.h"
#include "igtl_header.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


igtl::TrackingDataMessage::Pointer trackingSendMsg = igtl::TrackingDataMessage::New();
igtl::TrackingDataMessage::Pointer trackingReceiveMsg = igtl::TrackingDataMessage::New();

igtl::TrackingDataElement::Pointer trackingElement0 = igtl::TrackingDataElement::New();
igtl::TrackingDataElement::Pointer trackingElement1 = igtl::TrackingDataElement::New();
igtl::TrackingDataElement::Pointer trackingElement2 = igtl::TrackingDataElement::New();

float inT[4] = {-0.954892f, 0.196632f, -0.222525f, 0.0};
float inS[4] = {-0.196632f, 0.142857f, 0.970014f, 0.0};
float inN[4] = {0.222525f, 0.970014f, -0.0977491f, 0.0};
float inOrigin[4] = {46.0531f,19.4709f,46.0531f, 1.0};
igtl::Matrix4x4 inMatrix = {{inT[0],inS[0],inN[0],inOrigin[0]},
  {inT[1],inS[1],inN[1],inOrigin[1]},
  {inT[2],inS[2],inN[2],inOrigin[2]},
  {inT[3],inS[3],inN[3],inOrigin[3]}};

void BuildUpElements()
{
  trackingElement0->SetName("Tracker0");
  trackingElement0->SetType(IGTL_TDATA_TYPE_6D);
  trackingElement0->SetMatrix(inMatrix);
  
  trackingElement1->SetName("Tracker1");
  trackingElement1->SetType(IGTL_TDATA_TYPE_6D);
  trackingElement1->SetMatrix(inMatrix);
  
  trackingElement2->SetName("Tracker2");
  trackingElement2->SetType(IGTL_TDATA_TYPE_6D);
  trackingElement2->SetMatrix(inMatrix);

  trackingSendMsg = igtl::TrackingDataMessage::New();
  trackingSendMsg->SetHeaderVersion(IGTL_HEADER_VERSION_1);
  trackingSendMsg->SetDeviceName("DeviceName");
  trackingSendMsg->SetTimeStamp(0, 1234567892);
  trackingSendMsg->AddTrackingDataElement(trackingElement0);
  trackingSendMsg->AddTrackingDataElement(trackingElement1);
  trackingSendMsg->AddTrackingDataElement(trackingElement2);
  trackingSendMsg->Pack();
}

TEST(TrackingMessageTest, Pack)
{
  BuildUpElements();
  int r = memcmp((const void*)trackingSendMsg->GetPackPointer(), (const void*)test_tdata_message,
                 (size_t)(IGTL_HEADER_SIZE));
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)trackingSendMsg->GetPackBodyPointer(), (const void*)(test_tdata_message+(size_t)(IGTL_HEADER_SIZE)), IGTL_TDATA_ELEMENT_SIZE*3 );
  EXPECT_EQ(r, 0);
}


TEST(TrackingMessageTest, Unpack)
{
  BuildUpElements();
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->AllocatePack();
  memcpy(headerMsg->GetPackPointer(), (const void*)trackingSendMsg->GetPackPointer(), IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  trackingReceiveMsg->SetMessageHeader(headerMsg);
  trackingReceiveMsg->AllocatePack();
  memcpy(trackingReceiveMsg->GetPackBodyPointer(), trackingSendMsg->GetPackBodyPointer(), IGTL_TDATA_ELEMENT_SIZE*3);
  trackingReceiveMsg->Unpack();
  igtl_header *messageHeader = (igtl_header *)trackingReceiveMsg->GetPackPointer();
  EXPECT_STREQ(messageHeader->device_name, "DeviceName");
  EXPECT_STREQ(messageHeader->name, "TDATA");
  EXPECT_EQ(messageHeader->header_version, 1);
  EXPECT_EQ(messageHeader->timestamp, 1234567892);
  EXPECT_EQ(messageHeader->body_size, IGTL_TDATA_ELEMENT_SIZE*3);
  
  std::vector<char*> trackerDescription;
  trackerDescription.push_back((char*)"Tracker0");
  trackerDescription.push_back((char*)"Tracker1");
  trackerDescription.push_back((char*)"Tracker2");
  
  for (int i = 0; i<3;++i)
  {
    igtl::TrackingDataElement::Pointer elem = igtl::TrackingDataElement::New();
    trackingReceiveMsg->GetTrackingDataElement(i, elem);
    EXPECT_EQ(strncmp((char*)(elem->GetName()), trackerDescription[i], 8),0);
    igtl::Matrix4x4 outMatrix = {{0.0,0.0,0.0,0.0},
      {0.0,0.0,0.0,0.0},
      {0.0,0.0,0.0,0.0},
      {0.0,0.0,0.0,0.0}};
    elem->GetMatrix(outMatrix);
    EXPECT_TRUE(MatrixComparison(outMatrix, inMatrix, ABS_ERROR));
  }
}


int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

