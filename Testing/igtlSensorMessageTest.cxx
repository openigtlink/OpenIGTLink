/*=========================================================================
 
  Program:   OpenIGTLink Library
  Language:  C++
 
  Copyright (c) Insight Software Consortium. All rights reserved.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.
 
  =========================================================================*/

#include "igtlSensorMessage.h"
#include "igtlutil/igtl_test_data_sensor.h"
#include "igtlUnit.h"
#include "igtl_sensor.h"
#include "igtl_unit.h"
#include "igtl_header.h"
#include "igtlTestConfig.h"
#include "string.h"

igtl::SensorMessage::Pointer sensorDataSendMsg = igtl::SensorMessage::New();
igtl::SensorMessage::Pointer sensorDataReceiveMsg = igtl::SensorMessage::New();
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
  sensorDataSendMsg->SetHeaderVersion(IGTL_HEADER_VERSION_1);
  sensorDataSendMsg->AllocatePack();
  sensorDataSendMsg->SetLength(6);
  sensorDataSendMsg->SetDeviceName("DeviceName");
  sensorDataSendMsg->SetTimeStamp(0, 1234567892);
  
  sensorDataSendMsg->SetUnit(unit);
  for (int i =0; i < 6; i++)
    {
    sensorDataSendMsg->SetValue(i, sensorValues[i]);
    }
  sensorDataSendMsg->Pack();
  int r = memcmp((const void*)sensorDataSendMsg->GetPackPointer(), (const void*)test_sensor_message,
                 (size_t)(IGTL_HEADER_SIZE));
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)sensorDataSendMsg->GetPackBodyPointer(), (const void*)(test_sensor_message+IGTL_HEADER_SIZE),sensorDataSendMsg->GetPackBodySize());
  EXPECT_EQ(r, 0);
}


TEST(SensorMessageTest, Unpack)
{
  UnitInitization();
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->AllocatePack();
  memcpy(headerMsg->GetPackPointer(), (const void*)test_sensor_message, IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  sensorDataReceiveMsg = igtl::SensorMessage::New();
  sensorDataReceiveMsg->SetMessageHeader(headerMsg);
  sensorDataReceiveMsg->AllocatePack();
  memcpy(sensorDataReceiveMsg->GetPackBodyPointer(), sensorDataSendMsg->GetPackBodyPointer(), 58);
  sensorDataReceiveMsg->Unpack();
  
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

