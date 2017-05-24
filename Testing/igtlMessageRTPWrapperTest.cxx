/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlMessageRTPWrapper.h"
#include "igtlImageMessage.h"
#include "igtlutil/igtl_test_data_rtpwrapper.h"
#include "igtlMessageDebugFunction.h"
#include "igtl_types.h"
#include "igtl_header.h"
#include "igtl_image.h"
#include "igtl_util.h"
#include "igtlTestConfig.h"
#include "string.h"

igtl::ImageMessage::Pointer imageSendMsg = igtl::ImageMessage::New();
igtl::ImageMessage::Pointer imageReceiveMsg = igtl::ImageMessage::New();
igtl::MessageRTPWrapper::Pointer messageWrapperSenderSide = igtl::MessageRTPWrapper::New();
igtl::MessageRTPWrapper::Pointer messageWrapperReceiverSide = igtl::MessageRTPWrapper::New();
float inT[4] = {-0.954892f, 0.196632f, -0.222525f, 0.0};
float inS[4] = {-0.196632f, 0.142857f, 0.970014f, 0.0};
float inN[4] = {0.222525f, 0.970014f, -0.0977491f, 0.0};
float inOrigin[4] = {46.0531f,19.4709f,46.0531f, 1.0};
igtl::Matrix4x4 inMatrix = {{inT[0],inS[0],inN[0],inOrigin[0]},
  {inT[1],inS[1],inN[1],inOrigin[1]},
  {inT[2],inS[2],inN[2],inOrigin[2]},
  {inT[3],inS[3],inN[3],inOrigin[3]}};
int   size[3]     = {50, 50, 1};       // image dimension
float spacing[3]  = {1.0f, 1.0f, 1.0f};     // spacing (mm/pixel)
int   svsize[3]   = {50, 50, 1};       // sub-volume size
int   svoffset[3] = {0, 0, 0};           // sub-volume offset
int   scalarType = igtl::ImageMessage::TYPE_UINT8;// scalar type
int UDPPacketLength = 1300;

#if OpenIGTLink_PROTOCOL_VERSION >= 3
#include "igtlMessageFormat2TestMacro.h"
void BuildUp()
{
  imageSendMsg = igtl::ImageMessage::New();
  imageSendMsg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
  imageSendMsg->SetTimeStamp(0, 1234567892);
  imageSendMsg->SetDeviceName("DeviceName");
  //Initialization of a image message
  imageSendMsg->SetDimensions(size);
  imageSendMsg->SetSpacing(spacing);
  imageSendMsg->SetScalarType(scalarType);
  imageSendMsg->SetSubVolume(svsize, svoffset);
  imageSendMsg->SetNumComponents(1);
  imageSendMsg->SetScalarType(IGTL_IMAGE_STYPE_TYPE_UINT8);
  imageSendMsg->SetEndian(IGTL_IMAGE_ENDIAN_LITTLE);
  imageSendMsg->SetCoordinateSystem(IGTL_IMAGE_COORD_RAS);
  imageSendMsg->SetMatrix(inMatrix);
  igtlMetaDataAddElementMacro(imageSendMsg);
  imageSendMsg->AllocateScalars();
  memcpy((void*)imageSendMsg->GetScalarPointer(), test_image, TEST_IMAGE_MESSAGE_SIZE);//here m_Image is set.
  imageSendMsg->Pack();
  messageWrapperSenderSide = igtl::MessageRTPWrapper::New();
  messageWrapperSenderSide->SetRTPPayloadType(0); // 0 corresponding to PCMU payload type, default is 96 for dynamic allocation.
  messageWrapperSenderSide->SetRTPPayloadLength(UDPPacketLength);
  messageWrapperSenderSide->SetSeqNum(0);
  messageWrapperSenderSide->WrapMessageAndPushToBuffer((igtl_uint8*)imageSendMsg->GetPackPointer(), imageSendMsg->GetPackSize());
}

TEST(MessageRTPWrapperTest, WrapMessageFormatVersion2)
{
  BuildUp();
  igtl::PacketBuffer bufferedMsg = messageWrapperSenderSide->GetOutGoingPackets();
  //First Packet excluding time stamp comparison
  int r = memcmp(bufferedMsg.pBsBuf.data(), (const void*)test_RTPWrapper_PacketBuffer, 4);
  EXPECT_EQ(r, 0);
  r = memcmp(bufferedMsg.pBsBuf.data()+8, (const void*)(test_RTPWrapper_PacketBuffer+8), 4);
  EXPECT_EQ(r, 0);
  r = memcmp(bufferedMsg.pBsBuf.data()+RTP_HEADER_LENGTH, (const void*)(test_RTPWrapper_PacketBuffer+RTP_HEADER_LENGTH), UDPPacketLength);
  EXPECT_EQ(r, 0);
  
  //Second Packet excluding time stamp comparison
  r = memcmp(bufferedMsg.pBsBuf.data()+UDPPacketLength+RTP_HEADER_LENGTH, (const void*)(test_RTPWrapper_PacketBuffer+UDPPacketLength+RTP_HEADER_LENGTH), 4);
  EXPECT_EQ(r, 0);
  r = memcmp(bufferedMsg.pBsBuf.data()+UDPPacketLength+RTP_HEADER_LENGTH+8, (const void*)(test_RTPWrapper_PacketBuffer+UDPPacketLength+RTP_HEADER_LENGTH+8), 4);
  EXPECT_EQ(r, 0);//
  r = memcmp(bufferedMsg.pBsBuf.data()+UDPPacketLength+2*RTP_HEADER_LENGTH, (const void*)(test_RTPWrapper_PacketBuffer+UDPPacketLength+2*RTP_HEADER_LENGTH), UDPPacketLength);
  EXPECT_EQ(r, 0);//
  
  //Third Packet excluding time stamp comparison
  r = memcmp(bufferedMsg.pBsBuf.data()+2*UDPPacketLength+2*RTP_HEADER_LENGTH, (const void*)(test_RTPWrapper_PacketBuffer+2*UDPPacketLength+2*RTP_HEADER_LENGTH), 4);
  EXPECT_EQ(r, 0);
  r = memcmp(bufferedMsg.pBsBuf.data()+2*UDPPacketLength+2*RTP_HEADER_LENGTH+8, (const void*)(test_RTPWrapper_PacketBuffer+2*UDPPacketLength+2*RTP_HEADER_LENGTH+8), 4);
  EXPECT_EQ(r, 0);//
  r = memcmp(bufferedMsg.pBsBuf.data()+2*UDPPacketLength+3*RTP_HEADER_LENGTH, (const void*)(test_RTPWrapper_PacketBuffer+2*UDPPacketLength+3*RTP_HEADER_LENGTH),bufferedMsg.totalLength-2*UDPPacketLength-3*RTP_HEADER_LENGTH);
  EXPECT_EQ(r, 0);
  //TestDebugCharArrayCmp(bufferedMsg.pBsBuf.data(), (igtlUint8*)test_RTPWrapper_PacketBuffer , bufferedMsg.totalLength);
}

TEST(MessageRTPWrapperTest, UnwrapMessageFormatVersion2)
{
  BuildUp();
  messageWrapperReceiverSide = igtl::MessageRTPWrapper::New();
  messageWrapperReceiverSide->SetRTPPayloadLength(UDPPacketLength);
  igtl::PacketBuffer bufferedMsg = messageWrapperSenderSide->GetOutGoingPackets();
  igtlUint8* UDPPacket = bufferedMsg.pBsBuf.data();
  for (int i= 0; i<bufferedMsg.pPacketLengthInByte.size();i++)
  {
    messageWrapperReceiverSide->PushDataIntoPacketBuffer(UDPPacket, bufferedMsg.pPacketLengthInByte[i]);
    UDPPacket += bufferedMsg.pPacketLengthInByte[i];
  }
  while(1)
  {
    int iRet = messageWrapperReceiverSide->UnWrapPacketWithTypeAndName("IMAGE", "DeviceName");
    if(iRet == 0)
      break;
  }
  EXPECT_EQ(messageWrapperReceiverSide->unWrappedMessages.size(), 1);
  igtl::ImageMessage::Pointer imageReceiveMsg = igtl::ImageMessage::New();
  std::map<igtl_uint32, igtl::UnWrappedMessage*>::iterator it = messageWrapperReceiverSide->unWrappedMessages.begin();
  igtl::MessageHeader::Pointer header = igtl::MessageHeader::New();
  header->InitPack();
  memcpy(header->GetPackPointer(), it->second->messagePackPointer, IGTL_HEADER_SIZE);
  header->Unpack();
  imageReceiveMsg->SetMessageHeader(header);
  imageReceiveMsg->AllocateBuffer();
  memcpy(imageReceiveMsg->GetPackBodyPointer(), it->second->messagePackPointer + IGTL_HEADER_SIZE, it->second->messageDataLength - IGTL_HEADER_SIZE);
  int c = imageReceiveMsg->Unpack(1);
  EXPECT_EQ(c, 2);
  igtl_header *messageHeader = (igtl_header *)imageReceiveMsg->GetPackPointer();
  EXPECT_STREQ(messageHeader->device_name, "DeviceName");
  EXPECT_STREQ(messageHeader->name, "IMAGE");
  EXPECT_EQ(messageHeader->header_version, IGTL_HEADER_VERSION_2);
  EXPECT_EQ(messageHeader->timestamp, 1234567892);
  EXPECT_EQ(messageHeader->body_size, imageReceiveMsg->GetPackBodySize());

  int returnSize[3] = { 0, 0, 0 };
  imageReceiveMsg->GetDimensions(returnSize);
  EXPECT_THAT(returnSize, testing::ElementsAreArray(size));
  float returnSpacing[3] = { 0.0f, 0.0f, 0.0f };
  imageReceiveMsg->GetSpacing(returnSpacing);
  EXPECT_TRUE(ArrayFloatComparison(returnSpacing, spacing, 3, ABS_ERROR));
  int returnSvsize[3] = { 0, 0, 0 }, returnSvoffset[3] = { 0, 0, 0 };
  imageReceiveMsg->GetSubVolume(returnSvsize, returnSvoffset);
  EXPECT_THAT(returnSvsize, testing::ElementsAreArray(svsize));
  EXPECT_THAT(returnSvoffset, testing::ElementsAreArray(svoffset));
  EXPECT_EQ(imageReceiveMsg->GetScalarType(), IGTL_IMAGE_STYPE_TYPE_UINT8);
  EXPECT_EQ(imageReceiveMsg->GetEndian(), IGTL_IMAGE_ENDIAN_LITTLE);
  EXPECT_EQ(imageReceiveMsg->GetCoordinateSystem(), IGTL_IMAGE_COORD_RAS);
  EXPECT_EQ(imageReceiveMsg->GetMessageID(), 1);


  igtl::Matrix4x4 outMatrix = { { 0.0, 0.0, 0.0, 0.0 },
  { 0.0, 0.0, 0.0, 0.0 },
  { 0.0, 0.0, 0.0, 0.0 },
  { 0.0, 0.0, 0.0, 0.0 } };
  imageReceiveMsg->GetMatrix(outMatrix);
  EXPECT_TRUE(MatrixComparison(outMatrix, inMatrix, ABS_ERROR));
  //The imageHeader is byte-wized converted, so we skip the comparison of the image header.
  int r = memcmp((const char*)imageReceiveMsg->GetPackBodyPointer() + IGTL_IMAGE_HEADER_SIZE + IGTL_EXTENDED_HEADER_SIZE, (const void*)(test_image), (size_t)(TEST_IMAGE_MESSAGE_SIZE));
  EXPECT_EQ(r, 0);
  // Test the packet reorder function of the RTP Wrapper. In the following lines, we reverse the order of received UDPPackets.
  messageWrapperReceiverSide = igtl::MessageRTPWrapper::New();
  messageWrapperReceiverSide->SetRTPPayloadLength(UDPPacketLength);
  UDPPacket = bufferedMsg.pBsBuf.data()+bufferedMsg.totalLength;
  for (int i = bufferedMsg.pPacketLengthInByte.size()-1; i>=0;i--)
  {
    UDPPacket -= bufferedMsg.pPacketLengthInByte[i];
    messageWrapperReceiverSide->PushDataIntoPacketBuffer(UDPPacket, bufferedMsg.pPacketLengthInByte[i]);
  }
  while(1)
  {
    int iRet = messageWrapperReceiverSide->UnWrapPacketWithTypeAndName("IMAGE", "DeviceName");
    if(iRet == 0)
      break;
  }
  EXPECT_EQ(messageWrapperReceiverSide->unWrappedMessages.size(), 1);
  imageReceiveMsg = igtl::ImageMessage::New();
  it = messageWrapperReceiverSide->unWrappedMessages.begin();
  header = igtl::MessageHeader::New();
  header->InitPack();
  memcpy(header->GetPackPointer(), it->second->messagePackPointer, IGTL_HEADER_SIZE);
  header->Unpack();
  imageReceiveMsg->SetMessageHeader(header);
  imageReceiveMsg->AllocateBuffer();
  memcpy(imageReceiveMsg->GetPackBodyPointer(), it->second->messagePackPointer + IGTL_HEADER_SIZE, it->second->messageDataLength - IGTL_HEADER_SIZE);
  c = imageReceiveMsg->Unpack(1);
  EXPECT_EQ(c, 2);
  messageHeader = (igtl_header *)imageReceiveMsg->GetPackPointer();
  EXPECT_STREQ(messageHeader->device_name, "DeviceName");
  EXPECT_STREQ(messageHeader->name, "IMAGE");
  EXPECT_EQ(messageHeader->header_version, IGTL_HEADER_VERSION_2);
  EXPECT_EQ(messageHeader->timestamp, 1234567892);
  EXPECT_EQ(messageHeader->body_size, imageReceiveMsg->GetPackBodySize());

  imageReceiveMsg->GetDimensions(returnSize);
  EXPECT_THAT(returnSize, testing::ElementsAreArray(size));
  imageReceiveMsg->GetSpacing(returnSpacing);
  EXPECT_TRUE(ArrayFloatComparison(returnSpacing, spacing, 3, ABS_ERROR));
  imageReceiveMsg->GetSubVolume(returnSvsize, returnSvoffset);
  EXPECT_THAT(returnSvsize, testing::ElementsAreArray(svsize));
  EXPECT_THAT(returnSvoffset, testing::ElementsAreArray(svoffset));
  EXPECT_EQ(imageReceiveMsg->GetScalarType(), IGTL_IMAGE_STYPE_TYPE_UINT8);
  EXPECT_EQ(imageReceiveMsg->GetEndian(), IGTL_IMAGE_ENDIAN_LITTLE);
  EXPECT_EQ(imageReceiveMsg->GetCoordinateSystem(), IGTL_IMAGE_COORD_RAS);
  EXPECT_EQ(imageReceiveMsg->GetMessageID(), 1);


  igtl::Matrix4x4 outMatrix2 = { { 0.0, 0.0, 0.0, 0.0 },
  { 0.0, 0.0, 0.0, 0.0 },
  { 0.0, 0.0, 0.0, 0.0 },
  { 0.0, 0.0, 0.0, 0.0 } };
  imageReceiveMsg->GetMatrix(outMatrix2);
  EXPECT_TRUE(MatrixComparison(outMatrix2, inMatrix, ABS_ERROR));
  //The imageHeader is byte-wized converted, so we skip the comparison of the image header.
  r = memcmp((const char*)imageReceiveMsg->GetPackBodyPointer() + IGTL_IMAGE_HEADER_SIZE + IGTL_EXTENDED_HEADER_SIZE, (const void*)(test_image), (size_t)(TEST_IMAGE_MESSAGE_SIZE));
  EXPECT_EQ(r, 0);
}
#endif

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

