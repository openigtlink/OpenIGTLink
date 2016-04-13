/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 Date:      $Date: 2016/02/12 19:53:38 $
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "../Source/igtlTrajectoryMessage.h"
#include "igtl_header.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "igtl_trajectory.h"
#include "igtlOSUtil.h"
#include "../Testing/igtlutil/igtl_test_data_trajectory.h"

igtl::TrajectoryMessage::Pointer trajectoryMsg = igtl::TrajectoryMessage::New();
igtl::TrajectoryMessage::Pointer trajectoryMsg2 = igtl::TrajectoryMessage::New();

igtl::TrajectoryElement::Pointer trajectoryElement0 = igtl::TrajectoryElement::New();
igtl::TrajectoryElement::Pointer trajectoryElement1 = igtl::TrajectoryElement::New();
igtl::TrajectoryElement::Pointer trajectoryElement2 = igtl::TrajectoryElement::New();

void BuildUpElements()
{
  trajectoryElement0->SetName("TRAJECTORY_DESCRIPTION_0");
  trajectoryElement0->SetGroupName("TRAJECTORY");
  trajectoryElement0->SetType(IGTL_TRAJECTORY_TYPE_ENTRY_TARGET);
  trajectoryElement0->SetRGBA(255,0,0,255);
  trajectoryElement0->SetEntryPosition(10.0,15.0,20.0);
  trajectoryElement0->SetTargetPosition(25.0,30.0,35.0);
  trajectoryElement0->SetRadius(5.0);
  trajectoryElement0->SetOwner("IMAGE_0");
  trajectoryElement1->SetName("TRAJECTORY_DESCRIPTION_1");
  trajectoryElement1->SetGroupName("TRAJECTORY");
  trajectoryElement1->SetType(IGTL_TRAJECTORY_TYPE_ENTRY_TARGET);
  trajectoryElement1->SetRGBA(0,255,0,255);
  trajectoryElement1->SetEntryPosition(40.0,45.0,50.0);
  trajectoryElement1->SetTargetPosition(55.0,60.0,65.0);
  trajectoryElement1->SetRadius(2.5);
  trajectoryElement1->SetOwner("IMAGE_0");
  trajectoryElement2->SetName("TRAJECTORY_DESCRIPTION_2");
  trajectoryElement2->SetGroupName("TRAJECTORY");
  trajectoryElement2->SetType(IGTL_TRAJECTORY_TYPE_ENTRY_TARGET);
  trajectoryElement2->SetRGBA(0,0,255,255);
  trajectoryElement2->SetEntryPosition(70.0,75.0,80.0);
  trajectoryElement2->SetTargetPosition(85.0,90.0,95.0);
  trajectoryElement2->SetRadius(0.0);
  trajectoryElement2->SetOwner("IMAGE_0");
  trajectoryMsg = igtl::TrajectoryMessage::New();
  trajectoryMsg->SetDeviceName("DeviceName");
  trajectoryMsg->SetTimeStamp(0, 1234567890);
  trajectoryMsg->AddTrajectoryElement(trajectoryElement0);
  trajectoryMsg->AddTrajectoryElement(trajectoryElement1);
  trajectoryMsg->AddTrajectoryElement(trajectoryElement2);
  trajectoryMsg->Pack();
}

TEST(TrackingMessageTest, Pack)
{
  BuildUpElements();
  for (int i = 0 ; i< 508;++i)
  {
    std::cerr<<+((igtlUint8*)trajectoryMsg->GetPackPointer())[i]<<"  "<<+test_trajectory_message[i]<<std::endl;
  }
  int r = memcmp((const void*)trajectoryMsg->GetPackPointer(), (const void*)test_trajectory_message,
                 (size_t)(IGTL_HEADER_SIZE));
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)trajectoryMsg->GetPackBodyPointer(), (const void*)(test_trajectory_message+(size_t)(IGTL_HEADER_SIZE)), IGTL_TRAJECTORY_ELEMENT_SIZE*3 );
  EXPECT_EQ(r, 0);
}


TEST(TrackingMessageTest, Unpack)
{
  BuildUpElements();
  igtl::TrajectoryElement::Pointer temp= igtl::TrajectoryElement::New();
  trajectoryMsg2 = igtl::TrajectoryMessage::New();
  trajectoryMsg2->AddTrajectoryElement(temp);
  trajectoryMsg2->AddTrajectoryElement(temp);
  trajectoryMsg2->AddTrajectoryElement(temp);
  trajectoryMsg2->AllocatePack();
  memcpy(trajectoryMsg2->GetPackPointer(), trajectoryMsg->GetPackPointer(), IGTL_TRAJECTORY_ELEMENT_SIZE*3 + IGTL_HEADER_SIZE);
  trajectoryMsg2->Unpack();
  igtl_header *messageHeader = (igtl_header *)trajectoryMsg2->GetPackPointer();
  EXPECT_STREQ(messageHeader->device_name, "DeviceName");
  EXPECT_STREQ(messageHeader->name, "TRAJ");
  EXPECT_EQ(messageHeader->version, 1);
  EXPECT_EQ(messageHeader->timestamp, 1234567890);
  EXPECT_EQ(messageHeader->body_size, IGTL_TRAJECTORY_ELEMENT_SIZE*3);
  
  
  std::vector<std::vector<igtlUint8> > groundTruthRGBA(3,std::vector<igtlUint8>(4));
  igtlUint8 tempIni1[4] = {255,0,0,255};
  groundTruthRGBA[0].assign(tempIni1,tempIni1+4);
  igtlUint8 tempIni2[4] = {0,255,0,255};
  groundTruthRGBA[1].assign(tempIni2,tempIni2+4);
  igtlUint8 tempIni3[4] = {0,0,255,255};
  groundTruthRGBA[2].assign(tempIni3,tempIni3+4);
  
  std::vector<std::vector<igtlFloat32> > groundTruthEntryPoints(3,std::vector<igtlFloat32>(3));
  igtlFloat32 tempFloat1[3] = {10.0,15.0,20.0};
  groundTruthEntryPoints[0].assign(tempFloat1,tempFloat1+3);
  igtlFloat32 tempFloat2[3] = {40.0,45.0,50.0};
  groundTruthEntryPoints[1].assign(tempFloat2,tempFloat2+3);
  igtlFloat32 tempFloat3[3] = {70.0,75.0,80.0};
  groundTruthEntryPoints[2].assign(tempFloat3,tempFloat3+3);
  
  std::vector<std::vector<igtlFloat32> > groundTruthTargetPoints(3,std::vector<igtlFloat32>(3));
  igtlFloat32 tempFloat4[3] = {25.0,30.0,35.0};
  groundTruthTargetPoints[0].assign(tempFloat4,tempFloat4+3);
  igtlFloat32 tempFloat5[3] = {55.0,60.0,65.0};
  groundTruthTargetPoints[1].assign(tempFloat5,tempFloat5+3);
  igtlFloat32 tempFloat6[3] = {85.0,90.0,95.0};
  groundTruthTargetPoints[2].assign(tempFloat6,tempFloat6+3);
  
  igtlFloat32 groundTruthRadius[3] = {5.0,2.5,0.0};
  
  std::vector<char*> trajectoryDescription;
  trajectoryDescription.push_back((char*)"TRAJECTORY_DESCRIPTION_0");
  trajectoryDescription.push_back((char*)"TRAJECTORY_DESCRIPTION_1");
  trajectoryDescription.push_back((char*)"TRAJECTORY_DESCRIPTION_2");
  
  for (int i = 0; i<3;++i)
  {
    igtl::TrajectoryElement::Pointer elem = igtl::TrajectoryElement::New();
    trajectoryMsg2->GetTrajectoryElement(i, elem);
    EXPECT_EQ(strncmp((char*)(elem->GetName()), trajectoryDescription[i], 24),0);
    igtlUint8 returnedRGBA[4] ={0,0,0,0};
    elem->GetRGBA(returnedRGBA);
    EXPECT_THAT(returnedRGBA, testing::ElementsAreArray(groundTruthRGBA[i]));
    igtlFloat32 returnedEntryPoint[3] ={0,0,0};
    elem->GetEntryPosition(returnedEntryPoint);
    EXPECT_THAT(returnedEntryPoint, testing::ElementsAreArray(groundTruthEntryPoints[i]));
    igtlFloat32 returnedTargetPoint[3] ={0,0,0};
    elem->GetTargetPosition(returnedTargetPoint);
    EXPECT_THAT(returnedTargetPoint, testing::ElementsAreArray(groundTruthTargetPoints[i]));
    EXPECT_EQ(elem->GetRadius(), groundTruthRadius[i]);
    EXPECT_EQ(strncmp((char*)elem->GetOwner(), "IMAGE_0", 7),0);
  }
}


int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

