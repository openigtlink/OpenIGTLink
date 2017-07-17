/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlVideoMetaMessage.h"
#include "igtlutil/igtl_test_data_videometa.h"
#include "igtlMessageDebugFunction.h"
#include "igtl_videometa.h"
#include "igtl_header.h"
#include "igtlTestConfig.h"
#include "string.h"

float inT0[4] = {-0.954892f, 0.196632f, -0.222525f, 0.0};
float inS0[4] = {-0.196632f, 0.142857f, 0.970014f, 0.0};
float inN0[4] = {0.222525f, 0.970014f, -0.0977491f, 0.0};
float inOrigin0[4] = {46.0531f,19.4709f,46.0531f, 1.0};
igtl::Matrix4x4 inMatrix0 = {{inT0[0],inS0[0],inN0[0],inOrigin0[0]},
  {inT0[1],inS0[1],inN0[1],inOrigin0[1]},
  {inT0[2],inS0[2],inN0[2],inOrigin0[2]},
  {inT0[3],inS0[3],inN0[3],inOrigin0[3]}};

float inT1[4] = {-1.0f, 0.0f, 0.0f, 0.0};
float inS1[4] = {0.0f, 1.0f, 0.0f, 0.0};
float inN1[4] = {0.0f, 0.0f, 1.0f, 0.0};
float inOrigin1[4] = {50.0f,100.0f,16.0f, 1.0};
igtl::Matrix4x4 inMatrix1 = {{inT1[0],inS1[0],inN1[0],inOrigin1[0]},
  {inT1[1],inS1[1],inN1[1],inOrigin1[1]},
  {inT1[2],inS1[2],inN1[2],inOrigin1[2]},
  {inT1[3],inS1[3],inN1[3],inOrigin1[3]}};

float inT2[4] = {-0.954892f, 0.196632f, -0.222525f, 0.0};
float inS2[4] = {-0.196632f, 0.142857f, 0.970014f, 0.0};
float inN2[4] = {0.222525f, 0.970014f, -0.0977491f, 0.0};
float inOrigin2[4] = {150.0f,0.0f,200.0f, 1.0};
igtl::Matrix4x4 inMatrix2 = {{inT2[0],inS2[0],inN2[0],inOrigin2[0]},
  {inT2[1],inS2[1],inN2[1],inOrigin2[1]},
  {inT2[2],inS2[2],inN2[2],inOrigin2[2]},
  {inT2[3],inS2[3],inN2[3],inOrigin2[3]}};


igtl::VideoMetaElement::Pointer videoMetaElement0 = igtl::VideoMetaElement::New();
igtl::VideoMetaElement::Pointer videoMetaElement1 = igtl::VideoMetaElement::New();
igtl::VideoMetaElement::Pointer videoMetaElement2 = igtl::VideoMetaElement::New();
igtl::VideoMetaMessage::Pointer videoMetaSendMsg;
igtl::VideoMetaMessage::Pointer videoMetaReceiveMsg;

void BuildUpLabelElements()
{
  videoMetaElement0->SetName("VIDEO_DESCRIPTION_0");
  videoMetaElement0->SetDeviceName("VIDEO_0");
  videoMetaElement0->SetPatientName("PATIENT_0");
  videoMetaElement0->SetPatientID("PATIENT_ID_0");
  videoMetaElement0->SetZoomLevel(4);
  videoMetaElement0->SetFocalLength((igtl_float64)50.0);
  videoMetaElement0->SetMatrix(inMatrix0);
  videoMetaElement0->SetScalarType(IGTL_VIDEO_STYPE_TYPE_UINT16);
  videoMetaElement0->SetSize(512,512,64);
  
  videoMetaElement1->SetName("VIDEO_DESCRIPTION_1");
  videoMetaElement1->SetDeviceName("VIDEO_1");
  videoMetaElement1->SetPatientName("PATIENT_1");
  videoMetaElement1->SetPatientID("PATIENT_ID_1");
  videoMetaElement1->SetZoomLevel(-2);
  videoMetaElement1->SetFocalLength((igtl_float64)100.0);
  videoMetaElement1->SetMatrix(inMatrix1);
  videoMetaElement1->SetScalarType(IGTL_VIDEO_STYPE_TYPE_UINT16);
  videoMetaElement1->SetSize(256,128,32);
  
  videoMetaElement2->SetName("VIDEO_DESCRIPTION_2");
  videoMetaElement2->SetDeviceName("VIDEO_2");
  videoMetaElement2->SetPatientName("PATIENT_2");
  videoMetaElement2->SetPatientID("PATIENT_ID_2");
  videoMetaElement2->SetZoomLevel(0);
  videoMetaElement2->SetMatrix(inMatrix2);
  videoMetaElement2->SetFocalLength((igtl_float64)20.5);
  videoMetaElement2->SetScalarType(IGTL_VIDEO_STYPE_TYPE_UINT16);
  videoMetaElement2->SetSize(256,256,32);
  
  igtl::TimeStamp::Pointer timestamp = igtl::TimeStamp::New();
  timestamp->SetTime((igtlUint64)1234567892);//1234567850,1234567870,1234567890 are all fractions
  videoMetaSendMsg = igtl::VideoMetaMessage::New();
  videoMetaSendMsg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
  videoMetaSendMsg->SetDeviceName("DeviceName");
  videoMetaSendMsg->SetTimeStamp(timestamp);
  videoMetaSendMsg->AddVideoMetaElement(videoMetaElement0);
  videoMetaSendMsg->AddVideoMetaElement(videoMetaElement1);
  videoMetaSendMsg->AddVideoMetaElement(videoMetaElement2);
  videoMetaSendMsg->Pack();
}

TEST(VideoMetaMessageTest, Pack)
{
  BuildUpLabelElements();
  int r = memcmp((const void*)videoMetaSendMsg->GetPackPointer(), (const void*)test_videometa_message,
                 (size_t)(IGTL_HEADER_SIZE));
  //TestDebugCharArrayCmp(videoMetaSendMsg->GetPackPointer(),test_videometa_message, 58);

  EXPECT_EQ(r, 0);
  r = memcmp((const void*)videoMetaSendMsg->GetPackBodyPointer(), (const void*)(test_videometa_message+(size_t)(IGTL_HEADER_SIZE)), videoMetaSendMsg->GetPackBodySize());
  EXPECT_EQ(r, 0);
}



TEST(VideoMetaMessageTest, Unpack)
{
  BuildUpLabelElements();
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->AllocatePack();
  memcpy(headerMsg->GetPackPointer(), (const void*)videoMetaSendMsg->GetPackPointer(), IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  videoMetaReceiveMsg = igtl::VideoMetaMessage::New();
  videoMetaReceiveMsg->InitPack();
  videoMetaReceiveMsg->SetMessageHeader(headerMsg);
  videoMetaReceiveMsg->AllocatePack();
  memcpy(videoMetaReceiveMsg->GetPackBodyPointer(), videoMetaSendMsg->GetPackBodyPointer(), videoMetaSendMsg->GetPackBodySize());
  videoMetaReceiveMsg->Unpack();
  
  igtl_header *messageHeader = (igtl_header *)videoMetaReceiveMsg->GetPackPointer();
  EXPECT_STREQ(messageHeader->device_name, "DeviceName");
  EXPECT_STREQ(messageHeader->name, "VIDEOMETA");
  EXPECT_EQ(messageHeader->header_version, IGTL_HEADER_VERSION_2);
  EXPECT_EQ(messageHeader->timestamp, 1234567892);
  EXPECT_EQ(messageHeader->body_size, IGTL_VIDEOMETA_ELEMENT_SIZE*3+IGTL_EXTENDED_HEADER_SIZE+videoMetaReceiveMsg->GetMetaDataHeaderSize() + videoMetaReceiveMsg->GetMetaDataSize());
  
  std::vector<std::vector<igtlUint16> > groundTruthSize(3,std::vector<igtlUint16>(3));
  igtlUint16 tempIni1[3] = {512,512,64};
  groundTruthSize[0].assign(tempIni1,tempIni1+3);
  igtlUint16 tempIni2[3] = {256,128,32};
  groundTruthSize[1].assign(tempIni2,tempIni2+3);
  igtlUint16 tempIni3[3] = {256,256,32};
  groundTruthSize[2].assign(tempIni3,tempIni3+3);
  
  std::vector<char*> name;
  name.push_back((char*)"VIDEO_DESCRIPTION_0");
  name.push_back((char*)"VIDEO_DESCRIPTION_1");
  name.push_back((char*)"VIDEO_DESCRIPTION_2");
  std::vector<char*> deviceName;
  deviceName.push_back((char*)"VIDEO_0");
  deviceName.push_back((char*)"VIDEO_1");
  deviceName.push_back((char*)"VIDEO_2");
  std::vector<char*> patientName;
  patientName.push_back((char*)"PATIENT_0");
  patientName.push_back((char*)"PATIENT_1");
  patientName.push_back((char*)"PATIENT_2");
  std::vector<char*> patientID;
  patientID.push_back((char*)"PATIENT_ID_0");
  patientID.push_back((char*)"PATIENT_ID_1");
  patientID.push_back((char*)"PATIENT_ID_2");
  std::vector<igtlInt16>  zoomLevels(3);
  zoomLevels[0]=4;
  zoomLevels[1]=-2;
  zoomLevels[2]=0;
  std::vector<igtl_float64>  focalLength(3);
  focalLength[0]=50.0;
  focalLength[1]=100.0;
  focalLength[2]=20.5;
  
  for (int i = 0; i<3;++i)
    {
    igtl::VideoMetaElement::Pointer elem = igtl::VideoMetaElement::New();
    videoMetaReceiveMsg->GetVideoMetaElement(i, elem);
    EXPECT_EQ(strncmp((char*)(elem->GetName()), name[i], 19),0);
    EXPECT_EQ(strncmp((char*)(elem->GetDeviceName()), deviceName[i], 7),0);
    EXPECT_EQ(strncmp((char*)(elem->GetPatientName()), patientName[i], 9),0);
    EXPECT_EQ(strncmp((char*)(elem->GetPatientID()), patientID[i], 12),0);
    EXPECT_EQ(elem->GetZoomLevel(), zoomLevels[i]);
    EXPECT_EQ(elem->GetFocalLength(), focalLength[i]);
    igtlUint16 returnedSize[3] ={0,0,0};
    elem->GetSize(returnedSize[0],returnedSize[1],returnedSize[2]);
    EXPECT_THAT(returnedSize, testing::ElementsAreArray(groundTruthSize[i]));
    EXPECT_EQ(elem->GetScalarType(), (int)IGTL_VIDEO_STYPE_TYPE_UINT16);
    igtl::Matrix4x4 outMatrix = {{0.0,0.0,0.0,0.0},
                                {0.0,0.0,0.0,0.0},
                                {0.0,0.0,0.0,0.0},
                                {0.0,0.0,0.0,0.0}};
    elem->GetMatrix(outMatrix);
    if(i==0)
      {
      EXPECT_TRUE(MatrixComparison(outMatrix, inMatrix0, ABS_ERROR));
      }
    else if(i == 1)
      {
      EXPECT_TRUE(MatrixComparison(outMatrix, inMatrix1, ABS_ERROR));
      }
    else if(i == 2)
      {
      EXPECT_TRUE(MatrixComparison(outMatrix, inMatrix2, ABS_ERROR));
      }
    }
}


int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

