/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 Date:      $Date: 2016/02/12 19:53:38 $
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "../Source/igtlCapabilityMessage.h"
#include "igtl_header.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


TEST(CapabilityMessageTest, NumberOfTypes)
{
  igtl::CapabilityMessage::Pointer capabilityMsg = igtl::CapabilityMessage::New();
  capabilityMsg->SetNumberOfTypes(2);
  EXPECT_EQ(capabilityMsg->GetNumberOfTypes(), 2);
}

TEST(CapabilityMessageTest, SetAndGetTypes)
{
  // Set types and pack the message,then
  igtl::CapabilityMessage::Pointer capabilityMsg = igtl::CapabilityMessage::New();
  capabilityMsg->SetNumberOfTypes(2);
  capabilityMsg->SetType(0, "Image");
  capabilityMsg->SetType(1, "Transform");
  capabilityMsg->Pack();
  EXPECT_EQ(capabilityMsg->GetPackSize(), 82);
  const char* strGrounTruth = "Image\0\0\0\0\0\0\0Transform\0\0\0";
  EXPECT_EQ(strcmp((char *)capabilityMsg->GetPackBodyPointer(), strGrounTruth), 0);
  //--------------------
  igtl::CapabilityMessage::Pointer capabilityMsg2 = igtl::CapabilityMessage::New();
  capabilityMsg2->InitPack();
  capabilityMsg2->SetNumberOfTypes(2);
  capabilityMsg2->AllocatePack();
  memcpy(capabilityMsg2->GetPackBodyPointer(),capabilityMsg->GetPackBodyPointer(),capabilityMsg->GetPackSize()-IGTL_HEADER_SIZE);
  capabilityMsg2->Unpack();
  EXPECT_EQ(strcmp(capabilityMsg2->GetType(0), "Image\0\0\0\0\0\0\0"),0);
  EXPECT_EQ(strcmp(capabilityMsg2->GetType(1), "Transform\0\0\0"),0);
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

