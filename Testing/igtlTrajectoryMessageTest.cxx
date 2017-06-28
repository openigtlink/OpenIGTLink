/*=========================================================================
 
  Program:   OpenIGTLink Library
  Language:  C++
 
  Copyright (c) Insight Software Consortium. All rights reserved.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.
 
  =========================================================================*/

//#define IGTL_LEGACY_TEST
#include "igtlTrajectoryMessage.h"
#include "igtlutil/igtl_test_data_trajectory.h"
#include "igtlMessageDebugFunction.h"
#include "igtl_trajectory.h"
#include "igtl_header.h"
#include "igtlTestConfig.h"
#include "string.h"


igtl::TrajectoryMessage::Pointer trajectorySendMsg = igtl::TrajectoryMessage::New();
igtl::TrajectoryMessage::Pointer trajectoryReceiveMsg = igtl::TrajectoryMessage::New();

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
  trajectorySendMsg = igtl::TrajectoryMessage::New();
  trajectorySendMsg->SetHeaderVersion(IGTL_HEADER_VERSION_1);
  trajectorySendMsg->SetDeviceName("DeviceName");
  trajectorySendMsg->SetTimeStamp(0, 1234567892);
  trajectorySendMsg->AddTrajectoryElement(trajectoryElement0);
  trajectorySendMsg->AddTrajectoryElement(trajectoryElement1);
  trajectorySendMsg->AddTrajectoryElement(trajectoryElement2);
}

TEST(TrajectoryMessageTest, DeprecatedMethodTest)
{
  BuildUpElements();
  int size = trajectorySendMsg->GetNumberOfTrajectoryElement();
  EXPECT_EQ(size, 3);
  igtl::TrajectoryElement::Pointer trajectoryElementNotUsed = igtl::TrajectoryElement::New();
  trajectorySendMsg->ClearTrajectoryElement(trajectoryElementNotUsed);
  size = trajectorySendMsg->GetNumberOfTrajectoryElement();
  EXPECT_EQ(size, 0);
  BuildUpElements();
  size = trajectorySendMsg->GetNumberOfTrajectoryElement();
  EXPECT_EQ(size, 3);
  trajectorySendMsg->ClearTrajectoryElement(trajectoryElementNotUsed);
  size = trajectorySendMsg->GetNumberOfTrajectoryElement();
  EXPECT_EQ(size, 0);
}

TEST(TrajectoryMessageTest, PackFormatVersion1)
{
  BuildUpElements();
  trajectorySendMsg->Pack();
  int r = memcmp((const void*)trajectorySendMsg->GetPackPointer(), (const void*)test_trajectory_message,
                 (size_t)(IGTL_HEADER_SIZE));
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)trajectorySendMsg->GetPackBodyPointer(), (const void*)(test_trajectory_message+(size_t)(IGTL_HEADER_SIZE)), IGTL_TRAJECTORY_ELEMENT_SIZE*3 );
  EXPECT_EQ(r, 0);
}


TEST(TrajectoryMessageTest, UnpackFormatVersion1)
{
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->AllocatePack();
  memcpy(headerMsg->GetPackPointer(), trajectorySendMsg->GetPackPointer(), IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  trajectoryReceiveMsg->SetMessageHeader(headerMsg);
  trajectoryReceiveMsg->AllocatePack();
  memcpy(trajectoryReceiveMsg->GetPackBodyPointer(), trajectorySendMsg->GetPackBodyPointer(), IGTL_TRAJECTORY_ELEMENT_SIZE*3);
  trajectoryReceiveMsg->Unpack();
  igtl_header *messageHeader = (igtl_header *)trajectoryReceiveMsg->GetPackPointer();
  EXPECT_STREQ(messageHeader->device_name, "DeviceName");
  EXPECT_STREQ(messageHeader->name, "TRAJ");
  EXPECT_EQ(messageHeader->header_version, 1);
  EXPECT_EQ(messageHeader->timestamp, 1234567892);
  EXPECT_EQ(messageHeader->body_size, IGTL_TRAJECTORY_ELEMENT_SIZE*3);
  
  
  std::vector<std::vector<igtlUint8> > groundTruthRGBA(3,std::vector<igtlUint8>(4));
  igtlUint8 tempIni1[4] = {255,0,0,255};
  groundTruthRGBA[0].assign(tempIni1,tempIni1+4);
  igtlUint8 tempIni2[4] = {0,255,0,255};
  groundTruthRGBA[1].assign(tempIni2,tempIni2+4);
  igtlUint8 tempIni3[4] = {0,0,255,255};
  groundTruthRGBA[2].assign(tempIni3,tempIni3+4);

  igtlFloat32 groundTruthEntryPoints[3][3] ={{10.0,15.0,20.0},
                                             {40.0,45.0,50.0},
                                             {70.0,75.0,80.0}};
  
  igtlFloat32 groundTruthTargetPoints[3][3] = {{25.0,30.0,35.0},
                                               {55.0,60.0,65.0},
                                               {85.0,90.0,95.0}};
  
  igtlFloat32 groundTruthRadius[3] = {5.0,2.5,0.0};
  
  std::vector<char*> trajectoryDescription;
  trajectoryDescription.push_back((char*)"TRAJECTORY_DESCRIPTION_0");
  trajectoryDescription.push_back((char*)"TRAJECTORY_DESCRIPTION_1");
  trajectoryDescription.push_back((char*)"TRAJECTORY_DESCRIPTION_2");
  
  for (int i = 0; i<3;++i)
    {
    igtl::TrajectoryElement::Pointer elem = igtl::TrajectoryElement::New();
    trajectoryReceiveMsg->GetTrajectoryElement(i, elem);
    EXPECT_EQ(strncmp((char*)(elem->GetName()), trajectoryDescription[i], 24),0);
    igtlUint8 returnedRGBA[4] ={0,0,0,0};
    elem->GetRGBA(returnedRGBA);
    EXPECT_THAT(returnedRGBA, testing::ElementsAreArray(groundTruthRGBA[i]));
    igtlFloat32 returnedEntryPoint[3] ={0,0,0};
    elem->GetEntryPosition(returnedEntryPoint);
    EXPECT_TRUE(ArrayFloatComparison(returnedEntryPoint, groundTruthEntryPoints[i], 3, ABS_ERROR));
    igtlFloat32 returnedTargetPoint[3] ={0,0,0};
    elem->GetTargetPosition(returnedTargetPoint);
    EXPECT_TRUE(ArrayFloatComparison(returnedTargetPoint, groundTruthTargetPoints[i], 3, ABS_ERROR));
    EXPECT_EQ(elem->GetRadius(), groundTruthRadius[i]);
    EXPECT_EQ(strncmp((char*)elem->GetOwner(), "IMAGE_0", 7),0);
    }
}


#if OpenIGTLink_PROTOCOL_VERSION >= 3
#include "igtlutil/igtl_test_data_trajectoryFormat2.h"
#include "igtlMessageFormat2TestMacro.h"

TEST(TrajectoryMessageTest, PackFormatVersion2)
{
  trajectorySendMsg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
  BuildUpElements();
  igtlMetaDataAddElementMacro(trajectorySendMsg);
  trajectorySendMsg->Pack();
  int r = memcmp((const void*)trajectorySendMsg->GetPackPointer(), (const void*)test_trajectory_message_Format2,
                 (size_t)(IGTL_HEADER_SIZE));
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)trajectorySendMsg->GetPackBodyPointer(), (const void*)(test_trajectory_message_Format2+(size_t)(IGTL_HEADER_SIZE)), IGTL_TRAJECTORY_ELEMENT_SIZE*3 + EXTENDED_CONTENT_SIZE);
  EXPECT_EQ(r, 0);
}

TEST(TrajectoryMessageTest, UnpackFormatVersion2)
{
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->AllocatePack();
  memcpy(headerMsg->GetPackPointer(), trajectorySendMsg->GetPackPointer(), IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  trajectoryReceiveMsg->SetMessageHeader(headerMsg);
  trajectoryReceiveMsg->AllocatePack();
  memcpy(trajectoryReceiveMsg->GetPackBodyPointer(), trajectorySendMsg->GetPackBodyPointer(), IGTL_TRAJECTORY_ELEMENT_SIZE*3 + EXTENDED_CONTENT_SIZE);
  trajectoryReceiveMsg->Unpack();
  igtl_header *messageHeader = (igtl_header *)trajectoryReceiveMsg->GetPackPointer();
  EXPECT_STREQ(messageHeader->device_name, "DeviceName");
  EXPECT_STREQ(messageHeader->name, "TRAJ");
  EXPECT_EQ(messageHeader->header_version, 2);
  EXPECT_EQ(messageHeader->timestamp, 1234567892);
  EXPECT_EQ(messageHeader->body_size, IGTL_TRAJECTORY_ELEMENT_SIZE*3 + EXTENDED_CONTENT_SIZE);
  
  
  std::vector<std::vector<igtlUint8> > groundTruthRGBA(3,std::vector<igtlUint8>(4));
  igtlUint8 tempIni1[4] = {255,0,0,255};
  groundTruthRGBA[0].assign(tempIni1,tempIni1+4);
  igtlUint8 tempIni2[4] = {0,255,0,255};
  groundTruthRGBA[1].assign(tempIni2,tempIni2+4);
  igtlUint8 tempIni3[4] = {0,0,255,255};
  groundTruthRGBA[2].assign(tempIni3,tempIni3+4);
  
  igtlFloat32 groundTruthEntryPoints[3][3] ={{10.0,15.0,20.0},
                                             {40.0,45.0,50.0},
                                             {70.0,75.0,80.0}};
  
  igtlFloat32 groundTruthTargetPoints[3][3] = {{25.0,30.0,35.0},
                                               {55.0,60.0,65.0},
                                               {85.0,90.0,95.0}};
  
  igtlFloat32 groundTruthRadius[3] = {5.0,2.5,0.0};
  
  std::vector<char*> trajectoryDescription;
  trajectoryDescription.push_back((char*)"TRAJECTORY_DESCRIPTION_0");
  trajectoryDescription.push_back((char*)"TRAJECTORY_DESCRIPTION_1");
  trajectoryDescription.push_back((char*)"TRAJECTORY_DESCRIPTION_2");
  
  for (int i = 0; i<3;++i)
    {
    igtl::TrajectoryElement::Pointer elem = igtl::TrajectoryElement::New();
    trajectoryReceiveMsg->GetTrajectoryElement(i, elem);
    EXPECT_EQ(strncmp((char*)(elem->GetName()), trajectoryDescription[i], 24),0);
    igtlUint8 returnedRGBA[4] ={0,0,0,0};
    elem->GetRGBA(returnedRGBA);
    EXPECT_THAT(returnedRGBA, testing::ElementsAreArray(groundTruthRGBA[i]));
    igtlFloat32 returnedEntryPoint[3] ={0,0,0};
    elem->GetEntryPosition(returnedEntryPoint);
    EXPECT_TRUE(ArrayFloatComparison(returnedEntryPoint, groundTruthEntryPoints[i], 3, ABS_ERROR));
    igtlFloat32 returnedTargetPoint[3] ={0,0,0};
    elem->GetTargetPosition(returnedTargetPoint);
    EXPECT_TRUE(ArrayFloatComparison(returnedTargetPoint, groundTruthTargetPoints[i], 3, ABS_ERROR));
    EXPECT_EQ(elem->GetRadius(), groundTruthRadius[i]);
    EXPECT_EQ(strncmp((char*)elem->GetOwner(), "IMAGE_0", 7),0);
    }
  igtlMetaDataComparisonMacro(trajectoryReceiveMsg);
}

#endif

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

