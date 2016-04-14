/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 Date:      $Date: 2016/02/12 19:53:38 $
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlColorTableMessage.h"
#include "igtl_colortable.h"
#include "igtl_header.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"


igtl_uint8 RGBColorTabel[256*256*3];

TEST(ColorTableMessageTest, IndexAndMapType)
{
  igtl::ColorTableMessage::Pointer colorTabelMsg = igtl::ColorTableMessage::New();
  colorTabelMsg->SetMapType(igtl::ColorTableMessage::MAP_RGB);
  EXPECT_EQ(colorTabelMsg->GetMapType(), 19);
  colorTabelMsg->SetMapTypeToUint8();
  EXPECT_EQ(colorTabelMsg->GetMapType(), (int)igtl::ColorTableMessage::MAP_UINT8);
  colorTabelMsg->SetMapTypeToUint16();
  EXPECT_EQ(colorTabelMsg->GetMapType(), (int)igtl::ColorTableMessage::MAP_UINT16);
  colorTabelMsg->SetIndexType(igtl::ColorTableMessage::INDEX_UINT8);
  EXPECT_EQ(colorTabelMsg->GetIndexType(), 3);
  colorTabelMsg->SetIndexTypeToUint8();
  EXPECT_EQ(colorTabelMsg->GetIndexType(), (int)igtl::ColorTableMessage::INDEX_UINT8);
  colorTabelMsg->SetIndexTypeToUint16();
  EXPECT_EQ(colorTabelMsg->GetIndexType(), (int)igtl::ColorTableMessage::INDEX_UINT16);
}

TEST(ColorTableMessageTest, PackAndUnpackTable)
{
  for (int i=0; i<256*256; i++)
  {
    RGBColorTabel[i] = i;
    RGBColorTabel[i+256*256] = i;
    RGBColorTabel[i+256*256*2] = i;
  }
  igtl::ColorTableMessage::Pointer colorTabelMsg = igtl::ColorTableMessage::New();
  colorTabelMsg->SetIndexTypeToUint16();
  colorTabelMsg->SetMapType(igtl::ColorTableMessage::MAP_RGB);
  colorTabelMsg->AllocateTable();
  colorTabelMsg->Pack();
  memcpy(colorTabelMsg->GetTablePointer(),RGBColorTabel,256*256*3);
  EXPECT_EQ(colorTabelMsg->GetPackSize(), 256*256*3+IGTL_COLORTABLE_HEADER_SIZE+IGTL_HEADER_SIZE);
  //--------------------
  igtl::ColorTableMessage::Pointer colorTabelMsg2 = igtl::ColorTableMessage::New();
  colorTabelMsg2->InitPack();
  colorTabelMsg2->SetIndexTypeToUint16();
  colorTabelMsg2->SetMapType(igtl::ColorTableMessage::MAP_RGB);
  colorTabelMsg2->AllocatePack();
  memcpy(colorTabelMsg2->GetPackBodyPointer(),colorTabelMsg2->GetPackBodyPointer(),colorTabelMsg2->GetPackSize()-IGTL_HEADER_SIZE);
  colorTabelMsg2->Unpack();
  EXPECT_EQ(strcmp((char*)colorTabelMsg->GetTablePointer(), (char*)RGBColorTabel), 0);
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

