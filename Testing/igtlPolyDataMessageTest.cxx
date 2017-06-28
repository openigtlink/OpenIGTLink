/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlPolyDataMessage.h"
#include "igtlutil/igtl_test_data_polydata.h"
#include "igtlMessageDebugFunction.h"
#include "igtl_polydata.h"
#include "igtl_header.h"
#include "igtlTestConfig.h"
#include "string.h"

#include <list>

#define POLY_BODY_SIZE 300

igtl::PolyDataMessage::Pointer polyDataSendMsg = igtl::PolyDataMessage::New();
igtl::PolyDataMessage::Pointer polyDataReceiveMsg = igtl::PolyDataMessage::New();

igtl::PolyDataPointArray::Pointer polyPoint = igtl::PolyDataPointArray::New();
igtl::PolyDataCellArray::Pointer polyGon = igtl::PolyDataCellArray::New();
igtl::PolyDataAttribute::Pointer polyAttr = igtl::PolyDataAttribute::New();

static igtl_float32 points[8][3]={{0,0,0}, {1,0,0}, {1,1,0}, {0,1,0},
                                  {0,0,1}, {1,0,1}, {1,1,1}, {0,1,1}};
static igtl_uint32 polyArray[6][4]={{0,1,2,3}, {4,5,6,7}, {0,1,5,4},
                                    {1,2,6,5}, {2,3,7,6}, {3,0,4,7}};
static igtl_float32 attribute[8]={0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};
std::vector<std::list<igtl_uint32> >poly(6, std::list<igtl_uint32>(4));

void BuildUpElements()
{
  polyPoint->Clear();
  polyGon->Clear();
  for (int i =0; i<6; i++)
    {
    poly[i].assign(polyArray[i],polyArray[i]+4);
    }
  for (int i = 0; i < 8 ;i++)
    {
    polyPoint->AddPoint(points[i]);
    }
  for (int i = 0; i < 6 ;i++)
    {
    polyGon->AddCell(poly[i]);
    }
  polyAttr->SetType(IGTL_POLY_ATTR_TYPE_SCALAR);
  polyAttr->SetSize(8);
  polyAttr->SetName("attr");
  polyAttr->SetData(attribute);
  polyDataSendMsg = igtl::PolyDataMessage::New();
  polyDataSendMsg->SetHeaderVersion(IGTL_HEADER_VERSION_1);
  polyDataSendMsg->SetPoints(polyPoint.GetPointer());
  polyDataSendMsg->SetPolygons(polyGon.GetPointer());
  polyDataSendMsg->AddAttribute(polyAttr.GetPointer());
  polyDataSendMsg->SetDeviceName("DeviceName");
  polyDataSendMsg->SetTimeStamp(0, 1234567892);
  polyDataSendMsg->Pack();
}

TEST(PolyDataMessageTest, Pack)
{
  BuildUpElements();
  int r = memcmp((const void*)polyDataSendMsg->GetPackPointer(), (const void*)test_polydata_message_header,
                 (size_t)(IGTL_HEADER_SIZE));
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)polyDataSendMsg->GetPackBodyPointer(), (const void*)test_polydata_message_body, POLY_BODY_SIZE);
  EXPECT_EQ(r, 0);
}


TEST(PolyDataMessageTest, Unpack)
{
  BuildUpElements();
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->AllocatePack();
  memcpy(headerMsg->GetPackPointer(), (const void*)test_polydata_message_header, IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  polyDataReceiveMsg = igtl::PolyDataMessage::New();
  polyDataReceiveMsg->SetMessageHeader(headerMsg);
  polyDataReceiveMsg->AllocatePack();
  igtl_header *messageHeader = (igtl_header *)polyDataReceiveMsg->GetPackPointer();
  EXPECT_STREQ(messageHeader->device_name, "DeviceName");
  EXPECT_STREQ(messageHeader->name, "POLYDATA");
  EXPECT_EQ(messageHeader->header_version, 1);
  EXPECT_EQ(messageHeader->timestamp, 1234567892);
  EXPECT_EQ(messageHeader->body_size, POLY_BODY_SIZE);
  
  memcpy(polyDataReceiveMsg->GetPackBodyPointer(), polyDataSendMsg->GetPackBodyPointer(), POLY_BODY_SIZE);
  polyDataReceiveMsg->Unpack();
  
  igtl::PolyDataPointArray::Pointer pointUnpacked = polyDataReceiveMsg->GetPoints();;
  igtl::PolyDataCellArray::Pointer polygonUnpacked = polyDataReceiveMsg->GetPolygons();
  igtl::PolyDataAttribute::Pointer attrUnpacked = polyDataReceiveMsg->GetAttribute(0);
  
  for (int i = 0; i<8; i++)
    {
    igtlFloat32 point[3] = {0,0,0};
    pointUnpacked->GetPoint(i, point);
    EXPECT_TRUE(ArrayFloatComparison(point, points[i], 3, ABS_ERROR));
    }
  for (int i = 0; i<6; i++)
    {
    igtl_uint32 polygon[4] = {0,0,0,0};
    polygonUnpacked->GetCell(i, polygon);
    EXPECT_THAT(polygon, ::testing::ElementsAreArray(polyArray[i]));
    }
  igtl_float32 attributeValue[8];
  for (int i = 0; i<8; i++)
    {
    igtl_float32 value[1];
    attrUnpacked->GetNthData(i, value);
    attributeValue[i] = *value;
    }
  EXPECT_TRUE(ArrayFloatComparison(attributeValue, attribute,8,ABS_ERROR));
}


int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

