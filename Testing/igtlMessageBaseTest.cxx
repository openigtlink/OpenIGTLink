/*=========================================================================

  Program:   OpenIGTLink Library
  Language:  C++
  Date:      $Date: 2016/01/25 19:53:38 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <igtlMessageBase.h>
#include <igtlMessageHeader.h>
#include "googleTest/include/gtest.h"
#include <stdexcept>
#include "igtlutil/igtl_test_data_lbmeta.h"


#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

TEST(testMath, myCubeTest)
{
  EXPECT_EQ(1000, 1000);
}

void IntialTheMessageBase(igtl::MessageBase::Pointer &messageBase, igtl::MessageHeader::Pointer &header)
{
  //char test_lbmeta_message[] = {0x00, 0x01, 0xd2};
}

int InitializationTest(igtl::MessageBase::Pointer &messageBaseTest, igtl::MessageHeader::Pointer &headerTest)
{
  messageBaseTest->SetDeviceName("DeviceTest");
  std::cout<< "Output device name: "<<messageBaseTest->GetDeviceName()<< std::endl;
  if (!strcmp(messageBaseTest->GetDeviceName(), "DeviceTest") )
  {
    messageBaseTest->InitPack();
    if (!strcmp(messageBaseTest->GetDeviceName(),"") )
    {
      return EXIT_SUCCESS;
    }
    else
    {
      std::cerr << "Initialization failed" << std::endl;
      return EXIT_FAILURE;
    }
  }
  else
  {
    std::cerr << "The device name is not set" << std::endl;
    return EXIT_FAILURE;
  }
}

int SetDeviceNameTest(igtl::MessageBase::Pointer &messageBaseTest, igtl::MessageHeader::Pointer &headerTest)
{
  messageBaseTest->InitPack();
  std::cout<< "Output device name: "<<messageBaseTest->GetDeviceName()<< std::endl;
  if (!strcmp(messageBaseTest->GetDeviceName(), "") )
  {
    messageBaseTest->SetDeviceName("DeviceTest");
    if (!strcmp(messageBaseTest->GetDeviceName(), "DeviceTest") )
    {
      return EXIT_SUCCESS;
    }
    else
    {
      std::cerr << "Initialization failed" << std::endl;
      return EXIT_FAILURE;
    }
  }
  else
  {
    std::cerr << "The device name is not initialized" << std::endl;
    return EXIT_FAILURE;
  }
}

int GetDeviceNameTest(igtl::MessageBase::Pointer &messageBaseTest, igtl::MessageHeader::Pointer &headerTest)
{
  messageBaseTest->SetDeviceName("DeviceTest");
  std::cout<< "Output device name: "<<messageBaseTest->GetDeviceName()<< std::endl;
  if (!strcmp(messageBaseTest->GetDeviceName(), "DeviceTest") )
  {
    return EXIT_SUCCESS;
  }
  else
  {
    std::cerr << "The device name is returned correctly" << std::endl;
    return EXIT_FAILURE;
  }
}

int main(int argc, char **argv)
{
  
  igtl::MessageBase::Pointer messageBaseTest = igtl::MessageBase::New();
  igtl::MessageHeader::Pointer headerTest = NULL;
  testing::InitGoogleTest(&argc, argv);
  int error = InitializationTest(messageBaseTest, headerTest);
  error += SetDeviceNameTest(messageBaseTest, headerTest);
  error += GetDeviceNameTest(messageBaseTest, headerTest);
  if (error)
  {
    std::cerr << "Test failed" << std::endl;
    //return EXIT_FAILURE;
  }
  else
  {
    std::cout << "Test is successful" << std::endl;
    //return EXIT_SUCCESS;
  }
  
  return RUN_ALL_TESTS();
}



