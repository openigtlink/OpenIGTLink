/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlLabelMetaMessage.h"
#include "igtlutil/igtl_test_data_lbmeta.h"
#include "igtlutil/igtl_lbmeta.h"
#include "igtl_header.h"
#include "igtlTestConfig.h"
#include "string.h"

igtl::LabelMetaElement::Pointer labelMetaElement0 = igtl::LabelMetaElement::New();
igtl::LabelMetaElement::Pointer labelMetaElement1 = igtl::LabelMetaElement::New();
igtl::LabelMetaElement::Pointer labelMetaElement2 = igtl::LabelMetaElement::New();
igtl::LabelMetaMessage::Pointer labelMetaSendMsg = igtl::LabelMetaMessage::New();
igtl::LabelMetaMessage::Pointer labelMetaReceiveMsg = igtl::LabelMetaMessage::New();

void BuildUpLabelElements()
{
  labelMetaElement0->SetName("LABEL_DESCRIPTION_0");
  labelMetaElement0->SetDeviceName("LABEL_0");
  labelMetaElement0->SetLabel(1);
  labelMetaElement0->SetRGBA(255,0,0,255);
  labelMetaElement0->SetSize(256,128,32);
  labelMetaElement0->SetOwner("IMAGE_0");
  labelMetaElement1->SetName("LABEL_DESCRIPTION_1");
  labelMetaElement1->SetDeviceName("LABEL_1");
  labelMetaElement1->SetLabel(2);
  labelMetaElement1->SetRGBA(0,255,0,255);
  labelMetaElement1->SetSize(256,128,32);
  labelMetaElement1->SetOwner("IMAGE_0");
  labelMetaElement2->SetName("LABEL_DESCRIPTION_2");
  labelMetaElement2->SetDeviceName("LABEL_2");
  labelMetaElement2->SetLabel(3);
  labelMetaElement2->SetRGBA(0,0,255,255);
  labelMetaElement2->SetSize(256,128,32);
  labelMetaElement2->SetOwner("IMAGE_0");
  labelMetaSendMsg = igtl::LabelMetaMessage::New();
  labelMetaSendMsg->SetHeaderVersion(IGTL_HEADER_VERSION_1);
  labelMetaSendMsg->SetDeviceName("DeviceName");
  labelMetaSendMsg->SetTimeStamp(0, 1234567892);
  labelMetaSendMsg->AddLabelMetaElement(labelMetaElement0);
  labelMetaSendMsg->AddLabelMetaElement(labelMetaElement1);
  labelMetaSendMsg->AddLabelMetaElement(labelMetaElement2);
  labelMetaSendMsg->Pack();
}

TEST(LabelMetaMessageTest, Pack)
{
  BuildUpLabelElements();
  int r = memcmp((const void*)labelMetaSendMsg->GetPackPointer(), (const void*)test_lbmeta_message,
                 (size_t)(IGTL_HEADER_SIZE));
  //The header comparison, however, the crc is different. because the igtl_lbmeta_get_crc() is different from the crc generation in MessageBase::Pack()
  EXPECT_EQ(r, 0);
  r = memcmp((const void*)labelMetaSendMsg->GetPackBodyPointer(), (const void*)(test_lbmeta_message+(size_t)(IGTL_HEADER_SIZE)), IGTL_LBMETA_ELEMENT_SIZE*3 );
  EXPECT_EQ(r, 0);
}


TEST(LabelMetaMessageTest, Unpack)
{
  BuildUpLabelElements();
  igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
  headerMsg->AllocatePack();
  memcpy(headerMsg->GetPackPointer(), (const void*)labelMetaSendMsg->GetPackPointer(), IGTL_HEADER_SIZE);
  headerMsg->Unpack();
  labelMetaReceiveMsg->SetMessageHeader(headerMsg);
  labelMetaReceiveMsg->AllocatePack();
  memcpy(labelMetaReceiveMsg->GetPackBodyPointer(), labelMetaSendMsg->GetPackBodyPointer(), IGTL_LBMETA_ELEMENT_SIZE*3);
  labelMetaReceiveMsg->Unpack();
  igtl_header *messageHeader = (igtl_header *)labelMetaReceiveMsg->GetPackPointer();
  EXPECT_STREQ(messageHeader->device_name, "DeviceName");
  EXPECT_STREQ(messageHeader->name, "LBMETA");
  EXPECT_EQ(messageHeader->header_version, 1);
  EXPECT_EQ(messageHeader->timestamp, 1234567892);
  EXPECT_EQ(messageHeader->body_size, 348);
  
  std::vector<std::vector<igtlUint8> > groundTruthRGBA(3,std::vector<igtlUint8>(4));
  igtlUint8 tempIni1[4] = {255,0,0,255};
  groundTruthRGBA[0].assign(tempIni1,tempIni1+4);
  igtlUint8 tempIni2[4] = {0,255,0,255};
  groundTruthRGBA[1].assign(tempIni2,tempIni2+4);
  igtlUint8 tempIni3[4] = {0,0,255,255};
  groundTruthRGBA[2].assign(tempIni3,tempIni3+4);
  igtlUint16 groundTruthSize[3] = {256,128,32};
  
  std::vector<char*> labelDescription;
  labelDescription.push_back((char*)"LABEL_DESCRIPTION_0");
  labelDescription.push_back((char*)"LABEL_DESCRIPTION_1");
  labelDescription.push_back((char*)"LABEL_DESCRIPTION_2");
  std::vector<char*> label;
  label.push_back((char*)"LABEL_0");
  label.push_back((char*)"LABEL_1");
  label.push_back((char*)"LABEL_2");
  for (int i = 0; i<3;++i)
    {
    igtl::LabelMetaElement::Pointer elem = igtl::LabelMetaElement::New();
    labelMetaReceiveMsg->GetLabelMetaElement(i, elem);
    EXPECT_EQ(strncmp((char*)(elem->GetName()), labelDescription[i], 19),0);
    EXPECT_EQ(strncmp((char*)(elem->GetDeviceName()), label[i], 7),0);
    igtlUint8 returnedRGBA[4] ={0,0,0,0};
    elem->GetRGBA(returnedRGBA);
    EXPECT_THAT(returnedRGBA, testing::ElementsAreArray(groundTruthRGBA[i]));
    igtlUint16 returnedSize[3] ={0,0,0};
    elem->GetSize(returnedSize);
    EXPECT_THAT(returnedSize, testing::ElementsAreArray(groundTruthSize));
    EXPECT_EQ(strncmp((char*)elem->GetOwner(), "IMAGE_0", 7),0);
    }
}


int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

