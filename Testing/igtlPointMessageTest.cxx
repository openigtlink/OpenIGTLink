/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 Date:      $Date: 2016/02/12 19:53:38 $
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "../Source/igtlPointMessage.h"
#include "igtl_header.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "igtl_point.h"
#include "igtlOSUtil.h"
#include "../Testing/igtlutil/igtl_test_data_point.h"

igtl::PointMessage::Pointer pointMsg = igtl::PointMessage::New();
igtl::PointMessage::Pointer pointMsg2 = igtl::PointMessage::New();

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
  pointMsg = igtl::PointMessage::New();
  pointMsg->SetDeviceName("DeviceName");
  pointMsg->SetTimeStamp(0, 1234567890);
  pointMsg->AddPointElement(pointElement0);
  pointMsg->AddPointElement(pointElement1);
  pointMsg->AddPointElement(pointElement2);
  pointMsg->Pack();
}

TEST(PointMessageTest, Pack)
{
  BuildUpElements();
  for (int i = 0 ; i< 58;++i)
  {
    std::cerr<<+((igtlUint8*)pointMsg->GetPackPointer())[i]<<"  "<<+test_point_message[i]<<std::endl;
  }
  int r = memcmp((const void*)pointMsg->GetPackPointer(), (const void*)test_point_message,
                 (size_t)(IGTL_HEADER_SIZE));
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)pointMsg->GetPackBodyPointer(), (const void*)(test_point_message+(size_t)(IGTL_HEADER_SIZE)), IGTL_POINT_ELEMENT_SIZE*3 );
  EXPECT_EQ(r, 0);
}


TEST(PointMessageTest, Unpack)
{
  BuildUpElements();
  igtl::PointElement::Pointer temp= igtl::PointElement::New();
  pointMsg2 = igtl::PointMessage::New();
  pointMsg2->AddPointElement(temp);
  pointMsg2->AddPointElement(temp);
  pointMsg2->AddPointElement(temp);
  pointMsg2->AllocatePack();
  memcpy(pointMsg2->GetPackPointer(), pointMsg->GetPackPointer(), IGTL_POINT_ELEMENT_SIZE*3 + IGTL_HEADER_SIZE);
  pointMsg2->Unpack();
  igtl_header *messageHeader = (igtl_header *)pointMsg2->GetPackPointer();
  EXPECT_STREQ(messageHeader->device_name, "DeviceName");
  EXPECT_STREQ(messageHeader->name, "POINT");
  EXPECT_EQ(messageHeader->version, 1);
  EXPECT_EQ(messageHeader->timestamp, 1234567890);
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
    pointMsg2->GetPointElement(i, elem);
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

