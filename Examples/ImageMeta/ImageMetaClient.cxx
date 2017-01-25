/*=========================================================================

  Program:   OpenIGTLink -- Example for Image Meta Data Client
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
#include "igtlImageMessage.h"
#include "igtlImageMetaMessage.h"
#include "igtlClientSocket.h"


int ReceiveImageMeta(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header);


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
    igtl::GetImageMetaMessage::Pointer getImageMetaMsg;
    getImageMetaMsg = igtl::GetImageMetaMessage::New();
    getImageMetaMsg->SetDeviceName("Client");
    getImageMetaMsg->Pack();
    socket->Send(getImageMetaMsg->GetPackPointer(), getImageMetaMsg->GetPackSize());
    
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
      exit(0);
      }

    headerMsg->Unpack();
    if (strcmp(headerMsg->GetDeviceType(), "IMGMETA") == 0)
      {
      ReceiveImageMeta(socket, headerMsg);
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


int ReceiveImageMeta(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header)
{

  std::cerr << "Receiving IMGMETA data type." << std::endl;

  // Create a message buffer to receive transform data
  igtl::ImageMetaMessage::Pointer imgMeta;
  imgMeta = igtl::ImageMetaMessage::New();
  imgMeta->SetMessageHeader(header);
  imgMeta->AllocatePack();
  
  // Receive transform data from the socket
  socket->Receive(imgMeta->GetPackBodyPointer(), imgMeta->GetPackBodySize());
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = imgMeta->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    int nElements = imgMeta->GetNumberOfImageMetaElement();
    for (int i = 0; i < nElements; i ++)
      {
      igtl::ImageMetaElement::Pointer imgMetaElement;
      imgMeta->GetImageMetaElement(i, imgMetaElement);
      igtlUint16 size[3];
      imgMetaElement->GetSize(size);

      igtl::TimeStamp::Pointer ts;
      imgMetaElement->GetTimeStamp(ts);
      double time = ts->GetTimeStamp();

      std::cerr << "========== Element #" << i << " ==========" << std::endl;
      std::cerr << " Name       : " << imgMetaElement->GetName() << std::endl;
      std::cerr << " DeviceName : " << imgMetaElement->GetDeviceName() << std::endl;
      std::cerr << " Modality   : " << imgMetaElement->GetModality() << std::endl;
      std::cerr << " PatientName: " << imgMetaElement->GetPatientName() << std::endl;
      std::cerr << " PatientID  : " << imgMetaElement->GetPatientID() << std::endl;
      std::cerr << " TimeStamp  : " << std::fixed << time << std::endl;
      std::cerr << " Size       : ( " << size[0] << ", " << size[1] << ", " << size[2] << ")" << std::endl;
      std::cerr << " ScalarType : " << (int) imgMetaElement->GetScalarType() << std::endl;
      std::cerr << "================================" << std::endl;
      }
    return 1;
    }

  return 0;

}



