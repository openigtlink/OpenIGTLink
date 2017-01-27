/*=========================================================================

  Program:   OpenIGTLink -- Example for Tracker Client Program
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
#include "igtlTrackingDataMessage.h"
#include "igtlClientSocket.h"


int ReceiveTrackingData(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header);

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
  // Ask the server to start pushing tracking data
  std::cerr << "Sending STT_TDATA message....." << std::endl; 
  igtl::StartTrackingDataMessage::Pointer startTrackingMsg;
  startTrackingMsg = igtl::StartTrackingDataMessage::New();
  startTrackingMsg->SetDeviceName("TDataClient");
  startTrackingMsg->SetResolution(interval);
  startTrackingMsg->SetCoordinateName("Patient");
  startTrackingMsg->Pack();
  socket->Send(startTrackingMsg->GetPackPointer(), startTrackingMsg->GetPackSize());

  int loop = 0;

  while (1)
    {
    //------------------------------------------------------------
    // Wait for a reply
    igtl::MessageHeader::Pointer headerMsg;
    headerMsg = igtl::MessageHeader::New();
    headerMsg->InitPack();
    int rs = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
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
    if (strcmp(headerMsg->GetDeviceType(), "TDATA") == 0)
      {
      ReceiveTrackingData(socket, headerMsg);
      }
    else
      {
      std::cerr << "Receiving : " << headerMsg->GetDeviceType() << std::endl;
      socket->Skip(headerMsg->GetBodySizeToRead(), 0);
      }
    if (++loop >= 10) // if received 100 times
      {
      //------------------------------------------------------------
      // Ask the server to stop pushing tracking data
      std::cerr << "Sending STP_TDATA message....." << std::endl; 
      igtl::StopTrackingDataMessage::Pointer stopTrackingMsg;
      stopTrackingMsg = igtl::StopTrackingDataMessage::New();
      stopTrackingMsg->SetDeviceName("TDataClient");
      stopTrackingMsg->Pack();
      socket->Send(stopTrackingMsg->GetPackPointer(), stopTrackingMsg->GetPackSize());
      loop = 0;
      }
    }
}


int ReceiveTrackingData(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{
  std::cerr << "Receiving TDATA data type." << std::endl;
  
  //------------------------------------------------------------
  // Allocate TrackingData Message Class

  igtl::TrackingDataMessage::Pointer trackingData;
  trackingData = igtl::TrackingDataMessage::New();
  trackingData->SetMessageHeader(header);
  trackingData->AllocatePack();

  // Receive body from the socket
  socket->Receive(trackingData->GetPackBodyPointer(), trackingData->GetPackBodySize());

  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = trackingData->Unpack(1);

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


