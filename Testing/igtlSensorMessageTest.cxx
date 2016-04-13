/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 Date:      $Date: 2016/02/12 19:53:38 $
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "../Source/igtlSensorMessage.h"
#include "igtl_header.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "igtl_sensor.h"
#include "igtlOSUtil.h"
#include "../Testing/igtlutil/igtl_test_data_sensor.h"


igtl::SensorMessage::Pointer sensorDataMsg = igtl::SensorMessage::New();
igtl::SensorMessage::Pointer sensorDataMsg2 = igtl::SensorMessage::New();
igtlFloat64 sensorValues[6] = {123456.78,12345.678,1234.5678,123.45678,12.345678,1.2345678};
igtl::Unit::Pointer unit = igtl::Unit::New();

void UnitInitization()
{
  unit = igtl::Unit::New();
  unit->Init();
  unit->SetPrefix(IGTL_UNIT_PREFIX_NONE);
  unit->Append(IGTL_UNIT_SI_BASE_METER, (igtl_int8) 1);
  unit->Append(IGTL_UNIT_SI_BASE_SECOND, (igtl_int8) -2);
  unit->Pack();
}

TEST(SensorMessageTest, Pack)
{
  UnitInitization();
  sensorDataMsg->AllocatePack();
  sensorDataMsg->SetLength(6);
  sensorDataMsg->SetDeviceName("DeviceName");
  sensorDataMsg->SetTimeStamp(0, 1234567890);
  
  sensorDataMsg->SetUnit(unit);
  for (int i =0; i < 6; i++)
  {
    sensorDataMsg->SetValue(i, sensorValues[i]);
  }
  sensorDataMsg->Pack();
  int r = memcmp((const void*)sensorDataMsg->GetPackPointer(), (const void*)test_sensor_message,
                 (size_t)(IGTL_HEADER_SIZE));
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)sensorDataMsg->GetPackBodyPointer(), (const void*)(test_sensor_message+IGTL_HEADER_SIZE),sensorDataMsg->GetPackBodySize());
  EXPECT_EQ(r, 0);
}


TEST(SensorMessageTest, Unpack)
{
  UnitInitization();
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->AllocatePack();
  memcpy(headerMsg->GetPackPointer(), (const void*)test_sensor_message, IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  sensorDataMsg2 = igtl::SensorMessage::New();
  sensorDataMsg2->SetMessageHeader(headerMsg);
  sensorDataMsg2->AllocatePack();
  
  memcpy(sensorDataMsg2->GetPackBodyPointer(), sensorDataMsg->GetPackBodyPointer(), 58);
  sensorDataMsg2->Unpack();
  igtl::igtlUnit unitTruth = 0x443E0000000000;
  EXPECT_EQ(sensorDataMsg2->GetUnit(), unitTruth);
  EXPECT_EQ(sensorDataMsg2->GetLength(),6);
  EXPECT_EQ(sensorDataMsg2->GetValue(0),123456.78);
  EXPECT_EQ(sensorDataMsg2->GetValue(1),12345.678);
  EXPECT_EQ(sensorDataMsg2->GetValue(2),1234.5678);
  EXPECT_EQ(sensorDataMsg2->GetValue(3),123.45678);
  EXPECT_EQ(sensorDataMsg2->GetValue(4),12.345678);
  EXPECT_EQ(sensorDataMsg2->GetValue(5),1.2345678);
}


int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

