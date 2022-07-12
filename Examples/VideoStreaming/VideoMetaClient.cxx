/*=========================================================================

  Program:   OpenIGTLink -- Example for Video Meta Data Client
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <math.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "igtlOSUtil.h"
#include "igtlVideoMessage.h"
#include "igtlVideoMetaMessage.h"
#include "igtlClientSocket.h"


int ReceiveVideoMeta(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header);


int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 3) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <hostname> <port>"    << std::endl;
    std::cerr << "    <hostname> : IP or host name"                    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    exit(0);
    }

  char*  hostname = argv[1];
  int    port     = atoi(argv[2]);

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
  // loop
  for (int i = 0; i < 10; i ++)
    {
    //------------------------------------------------------------
    // Send request data
    igtl::GetVideoMetaMessage::Pointer getVideoMetaMsg;
    getVideoMetaMsg = igtl::GetVideoMetaMessage::New();
    getVideoMetaMsg->SetDeviceName("Client");
    getVideoMetaMsg->Pack();
    socket->Send(getVideoMetaMsg->GetPackPointer(), getVideoMetaMsg->GetPackSize());
    
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
      exit(0);
      }

    headerMsg->Unpack();
    if (strcmp(headerMsg->GetDeviceType(), "VIDEOMETA") == 0)
      {
      ReceiveVideoMeta(socket, headerMsg);
      }
    else
      {
      std::cerr << "Invalid response from the server:" << headerMsg->GetDeviceName() << std::endl; 
      exit(0);
      }

    igtl::Sleep(500); // wait
    }
    
  //------------------------------------------------------------
  // Close connection
  socket->CloseSocket();

}


int ReceiveVideoMeta(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{

  std::cerr << "Receiving VIDEOMETA data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::VideoMetaMessage::Pointer videoMeta;
  videoMeta = igtl::VideoMetaMessage::New();
  videoMeta->SetMessageHeader(header);
  videoMeta->AllocatePack();
  
  // Receive transform data from the socket
  bool timeout(false);
  socket->Receive(videoMeta->GetPackBodyPointer(), videoMeta->GetPackBodySize(), timeout);
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = videoMeta->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    int nElements = videoMeta->GetNumberOfVideoMetaElement();
    for (int i = 0; i < nElements; i ++)
      {
      igtl::VideoMetaElement::Pointer videoMetaElement;
      videoMeta->GetVideoMetaElement(i, videoMetaElement);
      igtlUint16 size[3];
      videoMetaElement->GetSize(size[0],size[1],size[2]);
      igtl::Matrix4x4 mat;
      videoMetaElement->GetMatrix(mat);

      std::cerr << "========== Element #" << i << " ==========" << std::endl;
      std::cerr << " Name       : " << videoMetaElement->GetName() << std::endl;
      std::cerr << " DeviceName : " << videoMetaElement->GetDeviceName() << std::endl;
      std::cerr << " PatientName: " << videoMetaElement->GetPatientName() << std::endl;
      std::cerr << " PatientID  : " << videoMetaElement->GetPatientID() << std::endl;
      igtl::PrintMatrix(mat);
      std::cerr << " Size       : ( " << size[0] << ", " << size[1] << ", " << size[2] << ")" << std::endl;
      std::cerr << " ScalarType : " << (int) videoMetaElement->GetScalarType() << std::endl;
      std::cerr << "================================" << std::endl;
      }
    return 1;
    }

  return 0;

}



