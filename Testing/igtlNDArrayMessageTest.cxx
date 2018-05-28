/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlNDArrayMessage.h"
#include "igtlutil/igtl_test_data_ndarray.h"
#include "igtl_ndarray.h"
#include "igtl_header.h"
#include "igtlTestConfig.h"
#include "string.h"

#define NDARRAY_MESSAGE_BODY_SIZE 488

igtl::NDArrayMessage::Pointer NDArraySendMsg = igtl::NDArrayMessage::New();
igtl::NDArrayMessage::Pointer NDArrayReceiveMsg = igtl::NDArrayMessage::New();
igtl::Array<igtl_float64> array;
std::vector<igtlUint16> size(3);

void BuildUpArrayElements()
{
  size[0] = 5;
  size[1] = 4;
  size[2] = 3;
  array.SetSize(size);
  int i,j,k;
  igtl_float64 arrayFloat[120];
  for (i = 0; i < size[0]; i ++)
    {
    for (j = 0; j < size[1]; j ++)
      {
      for (k = 0; k < size[2]; k ++)
        {
        arrayFloat[i*(4*3) + j*3 + k] = (igtl_float64) (i*(4*3) + j*3 + k);
        }
      }
    }
  array.SetArray((void*) arrayFloat);
  NDArraySendMsg = igtl::NDArrayMessage::New();
  NDArraySendMsg->SetDeviceName("DeviceName");
  NDArraySendMsg->SetArray(igtl::NDArrayMessage::TYPE_FLOAT64, &array);
  NDArraySendMsg->SetTimeStamp(0, 1234567892);
  NDArraySendMsg->SetHeaderVersion(IGTL_HEADER_VERSION_1);
  NDArraySendMsg->Pack();
}

TEST(NDArrayMessageTest, SetAndGetValue)
{
  BuildUpArrayElements();
  igtl::ArrayBase::IndexType index(array.GetSize());
  index.at(0) = 1;
  index.at(1) = 2;
  index.at(2) = 3;
  igtl_float64 returnValue = 1.0;
  array.GetValue(index, returnValue);
  // The value at index(1,2,3) = (i*(4*3) + j*3 + k) = 21
  EXPECT_FLOAT_EQ(returnValue, 21);
  igtl_float64 inputValue = 3.0;
  array.SetValue(index, inputValue);
  array.GetValue(index, returnValue);
  EXPECT_FLOAT_EQ(inputValue, returnValue);
}

TEST(NDArrayMessageTest, Pack)
{
  BuildUpArrayElements();
  
  int r = memcmp((const void*)NDArraySendMsg->GetPackPointer(), (const void*)(test_ndarray_message_header), IGTL_HEADER_SIZE);
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)NDArraySendMsg->GetPackBodyPointer(), (const void*)(test_ndarray_message_body), NDArraySendMsg->GetPackSize()-IGTL_HEADER_SIZE);
  EXPECT_EQ(r, 0);
}

TEST(NDArrayMessageTest, Unpack)
{
  BuildUpArrayElements();
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->AllocatePack();
  memcpy(headerMsg->GetPackPointer(), (const void*)NDArraySendMsg->GetPackPointer(), IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  NDArrayReceiveMsg->InitPack();
  NDArrayReceiveMsg->SetMessageHeader(headerMsg);
  NDArrayReceiveMsg->AllocatePack();
  memcpy(NDArrayReceiveMsg->GetPackBodyPointer(), NDArraySendMsg->GetPackBodyPointer(), NDARRAY_MESSAGE_BODY_SIZE);
  NDArrayReceiveMsg->Unpack();
  
  memcpy(NDArrayReceiveMsg->GetPackBodyPointer(), test_ndarray_message_body, NDArraySendMsg->GetPackSize()-IGTL_HEADER_SIZE);
  NDArrayReceiveMsg->Unpack();
  igtl::ArrayBase *tempArrayBase = NDArrayReceiveMsg->GetArray();
  igtl_float64* arraytemp = (igtl_float64 *)tempArrayBase->GetRawArray();
  int i,j,k;
  for (i = 0; i < size[0]; i ++)
    {
    for (j = 0; j < size[1]; j ++)
      {
      for (k = 0; k < size[2]; k ++)
        {
        EXPECT_EQ(i*(4*3) + j*3 + k, (igtl_float64)(*(arraytemp+i*(4*3) + j*3 + k)));
        }
      }
    }
  
}

TEST(NDArrayMessageTest, 64BitConversion)
{
  igtl_ndarray_info info;
  igtl_ndarray_init_info(&info);
  info.dim  = 3;
  info.type = igtl::NDArrayMessage::TYPE_FLOAT64;
  igtlUint16 sizeTemp[3] = {5,4,3};
  info.size = sizeTemp;
  unsigned char *data = new unsigned char [480];
  memcpy(data, (const void*)(test_ndarray_message_body+8), 480);
  info.array = (void*)data;
  
  unsigned char *dataArray = new unsigned char [488];
  igtl_ndarray_pack(&info, dataArray, IGTL_TYPE_PREFIX_NONE);
  igtl_ndarray_info info_return;
  igtl_ndarray_init_info(&info_return);
  igtl_ndarray_unpack(IGTL_TYPE_PREFIX_NONE, dataArray, &info_return, 488);
  int r = memcmp(info.array, info_return.array,480);
  EXPECT_EQ(r, 0);
}


int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

