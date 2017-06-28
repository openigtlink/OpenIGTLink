/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlPointMessage.h"
#include "igtlutil/igtl_test_data_point.h"
#include "igtl_point.h"
#include "igtl_header.h"
#include "igtlTestConfig.h"
#include "string.h"

igtl::PointMessage::Pointer pointSendMsg = igtl::PointMessage::New();
igtl::PointMessage::Pointer pointReceiveMsg = igtl::PointMessage::New();

igtl::PointElement::Pointer pointElement0 = igtl::PointElement::New();
igtl::PointElement::Pointer pointElement1 = igtl::PointElement::New();
igtl::PointElement::Pointer pointElement2 = igtl::PointElement::New();

void BuildUpElements()
{
  pointElement0->SetName("POINT_DESCRIPTION_0");
  pointElement0->SetGroupName("Landmark");
  pointElement0->SetRGBA(255,0,0,255);
  pointElement0->SetPosition(10.0, 15.0, 20.0);
  pointElement0->SetRadius(5.0);
  pointElement0->SetOwner("IMAGE_0");
  pointElement1->SetName("POINT_DESCRIPTION_1");
  pointElement1->SetGroupName("Landmark");
  pointElement1->SetRGBA(0,255,0,255);
  pointElement1->SetPosition(25.0, 30.0, 35.0);
  pointElement1->SetRadius(3.0);
  pointElement1->SetOwner("IMAGE_0");
  pointElement2->SetName("POINT_DESCRIPTION_2");
  pointElement2->SetGroupName("Landmark");
  pointElement2->SetRGBA(0,0,255,255);
  pointElement2->SetPosition(40.0, 45.0, 50.0);
  pointElement2->SetRadius(1.0);
  pointElement2->SetOwner("IMAGE_0");
  pointSendMsg = igtl::PointMessage::New();
  pointSendMsg->SetDeviceName("DeviceName");
  pointSendMsg->SetHeaderVersion(IGTL_HEADER_VERSION_1);
  pointSendMsg->SetTimeStamp(0, 1234567892);
  pointSendMsg->AddPointElement(pointElement0);
  pointSendMsg->AddPointElement(pointElement1);
  pointSendMsg->AddPointElement(pointElement2);
  pointSendMsg->Pack();
}

TEST(PointMessageTest, Pack)
{
  BuildUpElements();
  int r = memcmp((const void*)pointSendMsg->GetPackPointer(), (const void*)test_point_message,
                 (size_t)(IGTL_HEADER_SIZE));
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)pointSendMsg->GetPackBodyPointer(), (const void*)(test_point_message+(size_t)(IGTL_HEADER_SIZE)), IGTL_POINT_ELEMENT_SIZE*3 );
  EXPECT_EQ(r, 0);
}


TEST(PointMessageTest, Unpack)
{
  BuildUpElements();
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->AllocatePack();
  memcpy(headerMsg->GetPackPointer(), pointSendMsg->GetPackPointer(), IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  pointReceiveMsg = igtl::PointMessage::New();
  pointReceiveMsg->SetMessageHeader(headerMsg);
  pointReceiveMsg->AllocatePack();
  memcpy(pointReceiveMsg->GetPackBodyPointer(), pointSendMsg->GetPackBodyPointer(), IGTL_POINT_ELEMENT_SIZE*3);
  pointReceiveMsg->Unpack();
  
  igtl_header *messageHeader = (igtl_header *)pointReceiveMsg->GetPackPointer();
  EXPECT_STREQ(messageHeader->device_name, "DeviceName");
  EXPECT_STREQ(messageHeader->name, "POINT");
  EXPECT_EQ(messageHeader->header_version, 1);
  EXPECT_EQ(messageHeader->timestamp, 1234567892);
  EXPECT_EQ(messageHeader->body_size, IGTL_POINT_ELEMENT_SIZE*3);
  
  std::vector<std::vector<igtlUint8> > groundTruthRGBA(3,std::vector<igtlUint8>(4));
  igtlUint8 tempIni1[4] = {255,0,0,255};
  groundTruthRGBA[0].assign(tempIni1,tempIni1+4);
  igtlUint8 tempIni2[4] = {0,255,0,255};
  groundTruthRGBA[1].assign(tempIni2,tempIni2+4);
  igtlUint8 tempIni3[4] = {0,0,255,255};
  groundTruthRGBA[2].assign(tempIni3,tempIni3+4);
  
  std::vector<std::vector<igtlFloat32> > groundTruthPos(3,std::vector<igtlFloat32>(3));
  igtlFloat32 tempFloat1[3] = {10.0,15.0,20.0};
  groundTruthPos[0].assign(tempFloat1,tempFloat1+3);
  igtlFloat32 tempFloat2[3] = {25.0,30.0,35.0};
  groundTruthPos[1].assign(tempFloat2,tempFloat2+3);
  igtlFloat32 tempFloat3[3] = {40.0,45.0,50.0};
  groundTruthPos[2].assign(tempFloat3,tempFloat3+3);
  
  igtlFloat32 groundTruthRadius[3] = {5.0,3.0,1.0};
  
  std::vector<char*> pointDescription;
  pointDescription.push_back((char*)"POINT_DESCRIPTION_0");
  pointDescription.push_back((char*)"POINT_DESCRIPTION_1");
  pointDescription.push_back((char*)"POINT_DESCRIPTION_2");
  
  for (int i = 0; i<3;++i)
    {
    igtl::PointElement::Pointer elem = igtl::PointElement::New();
    pointReceiveMsg->GetPointElement(i, elem);
    EXPECT_EQ(strncmp((char*)(elem->GetName()), pointDescription[i], 19),0);
    igtlUint8 returnedRGBA[4] ={0,0,0,0};
    elem->GetRGBA(returnedRGBA);
    EXPECT_THAT(returnedRGBA, testing::ElementsAreArray(groundTruthRGBA[i]));
    igtlFloat32 returnedPosition[3] ={0,0,0};
    elem->GetPosition(returnedPosition);
    EXPECT_THAT(returnedPosition, testing::ElementsAreArray(groundTruthPos[i]));
    EXPECT_EQ(elem->GetRadius(), groundTruthRadius[i]);
    EXPECT_EQ(strncmp((char*)elem->GetOwner(), "IMAGE_0", 7),0);
    }
}


int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

