/*=========================================================================

  Program:   Open IGT Link -- Example for Tracker Client Program
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <math.h>
#include <cstdlib>
#include <cstring>

#include <string.h>

#include "igtlOSUtil.h"
#include "igtlTrackingDataMessage.h"
#include "igtlMessageRTPWrapper.h"
#include "igtlUDPClientSocket.h"

int ReceiveTrackingData(igtl::TrackingDataMessage::Pointer& msgData);

int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 2) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <hostname> <port> <fps>"    << std::endl;
    std::cerr << "    <port>     : Port # (18944 or 18945 in Slicer default)"   << std::endl;
    exit(0);
    }

  int    port     = atoi(argv[1]);

  //------------------------------------------------------------
  // Establish Connection

  igtl::UDPClientSocket::Pointer socket;
  socket = igtl::UDPClientSocket::New();
  int success = socket->JoinNetwork("127.0.0.1", port);
  if (success<0)
    {
      std::cerr << "unable to join network, check if your local machine joined the host more than once. " << std::endl;
      exit(0);
    }
  unsigned char* bufferPKT = new unsigned char[RTP_PAYLOAD_LENGTH+RTP_HEADER_LENGTH];
  igtl::MessageRTPWrapper::Pointer rtpWrapper = igtl::MessageRTPWrapper::New();
  igtl::SimpleMutexLock* glock = igtl::SimpleMutexLock::New();
  int loop = 0;
  for (loop = 0; loop<100; loop++)
  {
    int totMsgLen = socket->ReadSocket(bufferPKT, RTP_PAYLOAD_LENGTH+RTP_HEADER_LENGTH);
    rtpWrapper->PushDataIntoPacketBuffer(bufferPKT, totMsgLen);
    rtpWrapper->UnWrapPacketWithTypeAndName("TDATA", "Tracker");
    glock->Lock();
    unsigned int messageNum = rtpWrapper->unWrappedMessages.size();
    glock->Unlock();
    if(messageNum)// to do: glock this session
      {
      igtl::TrackingDataMessage::Pointer trackingMultiPKTMSG = igtl::TrackingDataMessage::New();
      glock->Lock();
      std::map<igtl_uint32, igtl::UnWrappedMessage*>::iterator it = rtpWrapper->unWrappedMessages.begin();
      igtlUint8 * message = new igtlUint8[it->second->messageDataLength];
      int MSGLength = it->second->messageDataLength;
      memcpy(message, it->second->messagePackPointer, it->second->messageDataLength);
      delete it->second;
      it->second = NULL;
      rtpWrapper->unWrappedMessages.erase(it);
      glock->Unlock();
      igtl::MessageHeader::Pointer header = igtl::MessageHeader::New();
      header->InitPack();
      memcpy(header->GetPackPointer(), message, IGTL_HEADER_SIZE);
      header->Unpack();
      trackingMultiPKTMSG->SetMessageHeader(header);
      trackingMultiPKTMSG->AllocateBuffer();
      if (MSGLength == trackingMultiPKTMSG->GetPackSize())
        {
        memcpy(trackingMultiPKTMSG->GetPackPointer(), message, MSGLength);
        ReceiveTrackingData(trackingMultiPKTMSG);
        }
      }
    }
}


int ReceiveTrackingData(igtl::TrackingDataMessage::Pointer& msgData)
{
  // Receive body from the socket
  igtl::TrackingDataMessage::Pointer trackingData;
  trackingData = igtl::TrackingDataMessage::New();
  //trackingData->SetMessageHeader(header);
  trackingData->Copy(msgData);

  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = trackingData->Unpack(0);

  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    int nElements = trackingData->GetNumberOfTrackingDataElements();
    for (int i = 0; i < nElements; i ++)
      {
      igtl::TrackingDataElement::Pointer trackingElement;
      trackingData->GetTrackingDataElement(i, trackingElement);

      igtl::Matrix4x4 matrix;
      trackingElement->GetMatrix(matrix);


      std::cerr << "========== Element #" << i << " ==========" << std::endl;
      std::cerr << " Name       : " << trackingElement->GetName() << std::endl;
      std::cerr << " Type       : " << (int) trackingElement->GetType() << std::endl;
      std::cerr << " Matrix : " << std::endl;
      igtl::PrintMatrix(matrix);
      std::cerr << "================================" << std::endl;
      }
    return 1;
    }
  return 0;
}


