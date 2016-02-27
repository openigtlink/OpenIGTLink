/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 Date:      $Date: 2016/02/12 19:53:38 $
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlImageMessage.h"
#include "igtl_types.h"
#include "igtl_header.h"
#include "igtl_image.h"
#include "igtl_util.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "igtlutil/igtl_test_data_image.h"

igtl::Matrix4x4 matrix_groundTruth;
float spacing_groundTruth[4];

TEST(ImageMessageTest, UnpackBodyTest)// how to unpack the igtl_head??
{
  igtl::ImageMessage::Pointer imageMessageTest = igtl::ImageMessage::New();
  igtl::MessageHeader::Pointer messageHeader = igtl::MessageHeader::New();
  messageHeader->AllocatePack();
  memcpy(messageHeader->GetPackPointer(), test_image_message, IGTL_HEADER_SIZE);
  messageHeader->Unpack(0);
  //messageHeader->AllocatePack();
  //memcpy(messageHeader->GetPackPointer(), test_image_message, IGTL_HEADER_SIZE);
  //messageHeader->Unpack(0);
  imageMessageTest->AllocateScalars();
    //igtl_header_convert_byte_order(imageMessageTest->GetScalarPointer());
  imageMessageTest->Copy(messageHeader);
  memcpy((void*)imageMessageTest->GetPackBodyPointer(), test_image_message+IGTL_HEADER_SIZE, IGTL_IMAGE_HEADER_SIZE+TEST_IMAGE_MESSAGE_SIZE);//here m_Body is set.
  igtl_image_header imageHeader;
  memcpy((void*)&imageHeader, test_image_message+IGTL_HEADER_SIZE, IGTL_IMAGE_HEADER_SIZE);
  imageHeader.version = IGTL_HEADER_VERSION;
  imageMessageTest->SetSubVolume((int*)imageHeader.subvol_size, (int*)imageHeader.subvol_offset);
  imageMessageTest->SetDimensions((int*)imageHeader.size);
  imageMessageTest->SetScalarType(igtl::ImageMessage::TYPE_UINT8);
  imageMessageTest->Unpack(0); // use the crc from the m_header
  //EXPECT_EQ(imageMessageTest->GetImageSize(), 2500);
  //igtl::Matrix4x4 matrix;
  //imageMessageTest->GetMatrix(matrix);
  //std::vector<int> v(4,1);
  //ASSERT_THAT(v, testing::ElementsAre(1,1,1,1));
  EXPECT_EQ(1, 1);
  //EXPECT_EQ(imageMessageTest->GetImageSize(), 2500);
}

void getGroundTruth()
{
  float rspacing[3];
  float origin[3];
  float norm_i[3];
  float norm_j[3];
  float norm_k[3];
  igtl_image_header image_header;
  memset((void*)&image_header, 0, sizeof(image_header));
  //memcpy((void*)&image_header, test_image_message+IGTL_HEADER_SIZE, IGTL_IMAGE_HEADER_SIZE);
  memcpy((char *)&(image_header), test_image_message+IGTL_HEADER_SIZE, IGTL_IMAGE_HEADER_SIZE);
  std::cout<<(igtl_uint16)test_image_message[IGTL_HEADER_SIZE]<<(igtl_uint16)test_image_message[IGTL_HEADER_SIZE+1]<<std::endl;
  std::cout<<image_header.version<<" "<<image_header.num_components<<image_header.scalar_type<<std::endl;
  igtl_image_get_matrix(rspacing, origin,
                        norm_i, norm_j, norm_k,
                        &image_header);
  
  for (int i = 0; i < 3; i ++) {
    spacing_groundTruth[i] = rspacing[i];
    matrix_groundTruth[i][0] = norm_i[i];
    matrix_groundTruth[i][1] = norm_j[i];
    matrix_groundTruth[i][2] = norm_k[i];
    matrix_groundTruth[i][3] = origin[i];
  }
}

int main(int argc, char **argv)
{
  getGroundTruth();
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

