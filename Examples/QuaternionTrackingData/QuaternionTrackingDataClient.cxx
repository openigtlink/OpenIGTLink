/*=========================================================================

  Program:   OpenIGTLink -- Example for Quaternion Tracking Data Client
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <math.h>
#include <cstdlib>
#include <cstring>

#include "igtlOSUtil.h"
#include "igtlQuaternionTrackingDataMessage.h"
#include "igtlClientSocket.h"


int ReceiveQuaternionTrackingData(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header);

int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 4) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <hostname> <port> <fps>"    << std::endl;
    std::cerr << "    <hostname> : IP or host name"                    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    std::cerr << "    <fps>      : Frequency (fps) to send coordinate" << std::endl;
    exit(0);
    }

  char*  hostname = argv[1];
  int    port     = atoi(argv[2]);
  double fps      = atof(argv[3]);
  int    interval = (int) (1000.0 / fps);

  //------------------------------------------------------------
  // Establish Connection

  igtl::ClientSocket::Pointer socket;
  socket = igtl::ClientSocket::New();
  int r = socket->ConnectToServer(hostname, port);

  if (r != 0)
    {
    std::cerr << "Cannot connect to the server." << std::endl;
    exit(0);
    }

  //------------------------------------------------------------
  // Ask the server to start pushing quaternion tracking data
  std::cerr << "Sending STT_QTDATA message....." << std::endl; 
  igtl::StartQuaternionTrackingDataMessage::Pointer startQuaternionTrackingMsg;
  startQuaternionTrackingMsg = igtl::StartQuaternionTrackingDataMessage::New();
  startQuaternionTrackingMsg->SetDeviceName("QTDataClient");
  startQuaternionTrackingMsg->SetResolution(interval);
  startQuaternionTrackingMsg->SetCoordinateName("Patient");
  startQuaternionTrackingMsg->Pack();
  socket->Send(startQuaternionTrackingMsg->GetPackPointer(), startQuaternionTrackingMsg->GetPackSize());

  int loop = 0;

  while (1)
    {
    //------------------------------------------------------------
    // Wait for a reply
    igtl::MessageHeader::Pointer headerMsg;
    headerMsg = igtl::MessageHeader::New();
    headerMsg->InitPack();
    bool timeout(false);
    igtlUint64 rs = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize(), timeout);
    if (rs == 0)
      {
      std::cerr << "Connection closed." << std::endl;
      socket->CloseSocket();
      exit(0);
      }
    if (rs != headerMsg->GetPackSize())
      {
      std::cerr << "Message size information and actual data size don't match." << std::endl; 
      socket->CloseSocket();
      exit(0);
      }
    
    headerMsg->Unpack();
    if (strcmp(headerMsg->GetDeviceType(), "QTDATA") == 0)
      {
      ReceiveQuaternionTrackingData(socket, headerMsg);
      }
    else
      {
      std::cerr << "Receiving : " << headerMsg->GetDeviceType() << std::endl;
      socket->Skip(headerMsg->GetBodySizeToRead(), 0);
      }
    if (++loop >= 10) // if received 100 times
      {
      //------------------------------------------------------------
      // Ask the server to stop pushing quaternion tracking data
      std::cerr << "Sending STP_QTDATA message....." << std::endl; 
      igtl::StopQuaternionTrackingDataMessage::Pointer stopQuaternionTrackingMsg;
      stopQuaternionTrackingMsg = igtl::StopQuaternionTrackingDataMessage::New();
      stopQuaternionTrackingMsg->SetDeviceName("QTDataClient");
      stopQuaternionTrackingMsg->Pack();
      socket->Send(stopQuaternionTrackingMsg->GetPackPointer(), stopQuaternionTrackingMsg->GetPackSize());
      loop = 0;
      }
    }
}


int ReceiveQuaternionTrackingData(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{
  std::cerr << "Receiving QTDATA data type." << std::endl;
  
  //------------------------------------------------------------
  // Allocate QuaternionTrackingData Message Class

  igtl::QuaternionTrackingDataMessage::Pointer quaternionTrackingData;
  quaternionTrackingData = igtl::QuaternionTrackingDataMessage::New();
  quaternionTrackingData->SetMessageHeader(header);
  quaternionTrackingData->AllocatePack();

  // Receive body from the socket
  bool timeout(false);
  socket->Receive(quaternionTrackingData->GetPackBodyPointer(), quaternionTrackingData->GetPackBodySize(), timeout);

  // Deserialize position and quaternion (orientation) data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = quaternionTrackingData->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    int nElements = quaternionTrackingData->GetNumberOfQuaternionTrackingDataElements();
    for (int i = 0; i < nElements; i ++)
      {
      igtl::QuaternionTrackingDataElement::Pointer quaternionTrackingElement;
      quaternionTrackingData->GetQuaternionTrackingDataElement(i, quaternionTrackingElement);

      float position[3];
      float quaternion[4];
      quaternionTrackingElement->GetPosition(position);
      quaternionTrackingElement->GetQuaternion(quaternion);

      std::cerr << "========== Element #" << i << " ==========" << std::endl;
      std::cerr << " Name       : " << quaternionTrackingElement->GetName() << std::endl;
      std::cerr << " Type       : " << (int) quaternionTrackingElement->GetType() << std::endl;
      std::cerr << " Position   : "; igtl::PrintVector3(position);
      std::cerr << " Quaternion : "; igtl::PrintVector4(quaternion);
      std::cerr << "================================" << std::endl;
      }
    return 1;
    }
  return 0;
}


