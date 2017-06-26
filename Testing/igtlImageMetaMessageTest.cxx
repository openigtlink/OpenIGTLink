/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlImageMetaMessage.h"
#include "igtlutil/igtl_test_data_imgmeta.h"
#include "igtlMessageDebugFunction.h"
#include "igtl_imgmeta.h"
#include "igtl_header.h"
#include "igtlTestConfig.h"
#include "string.h"

igtl::ImageMetaElement::Pointer imageMetaElement0 = igtl::ImageMetaElement::New();
igtl::ImageMetaElement::Pointer imageMetaElement1 = igtl::ImageMetaElement::New();
igtl::ImageMetaElement::Pointer imageMetaElement2 = igtl::ImageMetaElement::New();
igtl::ImageMetaMessage::Pointer imageMetaSendMsg;
igtl::ImageMetaMessage::Pointer imageMetaReceiveMsg;

void BuildUpLabelElements()
{
  igtl::TimeStamp::Pointer timestamp = igtl::TimeStamp::New();
  timestamp->SetTime((igtlUint64)1234567892);//1234567850,1234567870,1234567890 are all fractions
  imageMetaElement0->SetName("IMAGE_DESCRIPTION_0");
  imageMetaElement0->SetDeviceName("IMAGE_0");
  imageMetaElement0->SetModality("CT");
  imageMetaElement0->SetPatientName("PATIENT_0");
  imageMetaElement0->SetPatientID("PATIENT_ID_0");
  imageMetaElement0->SetTimeStamp(timestamp);
  imageMetaElement0->SetScalarType(IGTL_IMAGE_STYPE_TYPE_UINT16);
  imageMetaElement0->SetSize(512,512,64);
  
  timestamp = igtl::TimeStamp::New();
  timestamp->SetTime((igtlUint64)1234567896);
  imageMetaElement1->SetName("IMAGE_DESCRIPTION_1");
  imageMetaElement1->SetDeviceName("IMAGE_1");
  imageMetaElement1->SetModality("MRI");
  imageMetaElement1->SetPatientName("PATIENT_1");
  imageMetaElement1->SetPatientID("PATIENT_ID_1");
  imageMetaElement1->SetTimeStamp(timestamp);
  imageMetaElement1->SetScalarType(IGTL_IMAGE_STYPE_TYPE_UINT16);
  imageMetaElement1->SetSize(256,128,32);
  
  timestamp = igtl::TimeStamp::New();
  timestamp->SetTime((igtl_uint64)1234567900);
  imageMetaElement2->SetName("IMAGE_DESCRIPTION_2");
  imageMetaElement2->SetDeviceName("IMAGE_2");
  imageMetaElement2->SetModality("PET");
  imageMetaElement2->SetPatientName("PATIENT_2");
  imageMetaElement2->SetPatientID("PATIENT_ID_2");
  imageMetaElement2->SetTimeStamp(timestamp);
  imageMetaElement2->SetScalarType(IGTL_IMAGE_STYPE_TYPE_UINT16);
  imageMetaElement2->SetSize(256,256,32);
  
  timestamp = igtl::TimeStamp::New();
  timestamp->SetTime((igtlUint64)1234567892);
  imageMetaSendMsg = igtl::ImageMetaMessage::New();
  imageMetaSendMsg->SetHeaderVersion(IGTL_HEADER_VERSION_1);
  imageMetaSendMsg->SetDeviceName("DeviceName");
  imageMetaSendMsg->SetTimeStamp(timestamp);
  imageMetaSendMsg->AddImageMetaElement(imageMetaElement0);
  imageMetaSendMsg->AddImageMetaElement(imageMetaElement1);
  imageMetaSendMsg->AddImageMetaElement(imageMetaElement2);
  imageMetaSendMsg->Pack();
}

TEST(ImageMetaMessageTest, TimeStampTrival)
{
  igtl::TimeStamp::Pointer timestamp = igtl::TimeStamp::New();
  igtlUint32 tm = 1234567892;
  igtlUint32 tm_forward = igtl_frac_to_nanosec(tm);
  igtlUint32 tm_return; //= igtl_nanosec_to_frac(tm_forward);
  tm_return = igtl_nanosec_to_frac(tm_forward);
  EXPECT_EQ(tm_return, tm);
}

TEST(ImageMetaMessageTest, Pack)
{
  BuildUpLabelElements();
  int r = memcmp((const void*)imageMetaSendMsg->GetPackPointer(), (const void*)test_imgmeta_message,
                 (size_t)(IGTL_HEADER_SIZE));
  TestDebugCharArrayCmp(imageMetaSendMsg->GetPackPointer(),test_imgmeta_message, 58);

  EXPECT_EQ(r, 0);
  r = memcmp((const void*)imageMetaSendMsg->GetPackBodyPointer(), (const void*)(test_imgmeta_message+(size_t)(IGTL_HEADER_SIZE)), IGTL_IMGMETA_ELEMENT_SIZE*3 );
  EXPECT_EQ(r, 0);
}



TEST(ImageMetaMessageTest, Unpack)
{
  BuildUpLabelElements();
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->AllocatePack();
  memcpy(headerMsg->GetPackPointer(), (const void*)imageMetaSendMsg->GetPackPointer(), IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  imageMetaReceiveMsg = igtl::ImageMetaMessage::New();
  imageMetaReceiveMsg->InitPack();
  imageMetaReceiveMsg->SetMessageHeader(headerMsg);
  imageMetaReceiveMsg->AllocatePack();
  memcpy(imageMetaReceiveMsg->GetPackBodyPointer(), imageMetaSendMsg->GetPackBodyPointer(), IGTL_IMGMETA_ELEMENT_SIZE*3);
  imageMetaReceiveMsg->Unpack();
  
  igtl_header *messageHeader = (igtl_header *)imageMetaReceiveMsg->GetPackPointer();
  EXPECT_STREQ(messageHeader->device_name, "DeviceName");
  EXPECT_STREQ(messageHeader->name, "IMGMETA");
  EXPECT_EQ(messageHeader->header_version, 1);
  EXPECT_EQ(messageHeader->timestamp, 1234567892);
  EXPECT_EQ(messageHeader->body_size, IGTL_IMGMETA_ELEMENT_SIZE*3);
  
  std::vector<std::vector<igtlUint16> > groundTruthSize(3,std::vector<igtlUint16>(3));
  igtlUint16 tempIni1[3] = {512,512,64};
  groundTruthSize[0].assign(tempIni1,tempIni1+3);
  igtlUint16 tempIni2[3] = {256,128,32};
  groundTruthSize[1].assign(tempIni2,tempIni2+3);
  igtlUint16 tempIni3[3] = {256,256,32};
  groundTruthSize[2].assign(tempIni3,tempIni3+3);
  
  std::vector<char*> name;
  name.push_back((char*)"IMAGE_DESCRIPTION_0");
  name.push_back((char*)"IMAGE_DESCRIPTION_1");
  name.push_back((char*)"IMAGE_DESCRIPTION_2");
  std::vector<char*> deviceName;
  deviceName.push_back((char*)"IMAGE_0");
  deviceName.push_back((char*)"IMAGE_1");
  deviceName.push_back((char*)"IMAGE_2");
  std::vector<char*> modality;
  modality.push_back((char*)"CT\0");
  modality.push_back((char*)"MRI");
  modality.push_back((char*)"PET");
  std::vector<char*> patientName;
  patientName.push_back((char*)"PATIENT_0");
  patientName.push_back((char*)"PATIENT_1");
  patientName.push_back((char*)"PATIENT_2");
  std::vector<char*> patientID;
  patientID.push_back((char*)"PATIENT_ID_0");
  patientID.push_back((char*)"PATIENT_ID_1");
  patientID.push_back((char*)"PATIENT_ID_2");
  std::vector<igtlUint32>  nanoSecond(3);
  nanoSecond[0]=287445240;
  nanoSecond[1]=287445241;
  nanoSecond[2]=287445242;
  
  for (int i = 0; i<3;++i)
    {
    igtl::ImageMetaElement::Pointer elem = igtl::ImageMetaElement::New();
    imageMetaSendMsg->GetImageMetaElement(i, elem);
    EXPECT_EQ(strncmp((char*)(elem->GetName()), name[i], 19),0);
    EXPECT_EQ(strncmp((char*)(elem->GetDeviceName()), deviceName[i], 7),0);
    EXPECT_EQ(strncmp((char*)(elem->GetModality()), modality[i], 3),0);
    EXPECT_EQ(strncmp((char*)(elem->GetPatientName()), patientName[i], 9),0);
    EXPECT_EQ(strncmp((char*)(elem->GetPatientID()), patientID[i], 12),0);
    igtl::TimeStamp::Pointer timestamp = igtl::TimeStamp::New();
    elem->GetTimeStamp(timestamp);
    EXPECT_EQ(timestamp->GetNanosecond(), nanoSecond[i]);
    EXPECT_EQ(timestamp->GetSecond(), 0);
    
    igtlUint16 returnedSize[3] ={0,0,0};
    elem->GetSize(returnedSize);
    EXPECT_THAT(returnedSize, testing::ElementsAreArray(groundTruthSize[i]));
    EXPECT_EQ(elem->GetScalarType(), (int)IGTL_IMAGE_STYPE_TYPE_UINT16);
    }
}


int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

