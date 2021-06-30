/*=========================================================================

  Program:   OpenIGTLink -- Example for Image Meta Data Server
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
#include "igtlMessageHeader.h"
#include "igtlImageMessage.h"
#include "igtlServerSocket.h"
#include "igtlImageMetaMessage.h"


int SendImageMeta(igtl::Socket::Pointer& socket, const char* name);

int main(int argc, char* argv[])
{

  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 2) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <port>"    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    exit(0);
    }

  int    port     = atoi(argv[1]);

  igtl::ServerSocket::Pointer serverSocket;
  serverSocket = igtl::ServerSocket::New();
  int r = serverSocket->CreateServer(port);

  if (r < 0)
    {
    std::cerr << "Cannot create a server socket." << std::endl;
    exit(0);
    }

  igtl::Socket::Pointer socket;
  
  while (1)
    {
    //------------------------------------------------------------
    // Waiting for Connection
    socket = serverSocket->WaitForConnection(1000);
    
    if (socket.IsNotNull()) // if client connected
      {
      std::cerr << "A client is connected." << std::endl;

      // Create a message buffer to receive header
      igtl::MessageHeader::Pointer headerMsg;
      headerMsg = igtl::MessageHeader::New();

      //------------------------------------------------------------
      // loop
      for (int i = 0; i < 100; i ++)
        {

        // Initialize receive buffer
        headerMsg->InitPack();

        // Receive generic header from the socket
        bool timeout(false);
        igtlUint64 rs = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize(), timeout);
        if (rs == 0)
          {
          socket->CloseSocket();
          }
        if (rs != headerMsg->GetPackSize())
          {
          continue;
          }

        // Deserialize the header
        headerMsg->Unpack();

        // Check data type and receive data body
        if (strcmp(headerMsg->GetDeviceType(), "GET_IMGMETA") == 0)
          {
          std::cerr << "Received a GET_IMGMETA message." << std::endl;
          //socket->Skip(headerMsg->GetBodySizeToRead(), 0);
          SendImageMeta(socket, headerMsg->GetDeviceName());
          }
        else
          {
          // if the data type is unknown, skip reading.
          std::cerr << "Receiving : " << headerMsg->GetDeviceType() << std::endl;
          socket->Skip(headerMsg->GetBodySizeToRead(), 0);
          }
        }
      }
    }
    
  //------------------------------------------------------------
  // Close connection (The example code never reaches to this section ...)
  
  socket->CloseSocket();

}


int SendImageMeta(igtl::Socket::Pointer& socket, const char* name)
{

  //------------------------------------------------------------
  // Allocate Status Message Class

  igtl::ImageMetaMessage::Pointer imgMetaMsg;
  imgMetaMsg = igtl::ImageMetaMessage::New();
  // NOTE: the server should send a message with the same device name
  // as the received query message.
  imgMetaMsg->SetDeviceName(name);

  //---------------------------
  // Create 1st meta data
  igtl::ImageMetaElement::Pointer imgMeta0;
  imgMeta0 = igtl::ImageMetaElement::New();
  imgMeta0->SetName("IMAGE_DESCRIPTION_0");
  imgMeta0->SetDeviceName("IMAGE_0");
  imgMeta0->SetModality("CT");
  imgMeta0->SetPatientName("PATIENT_0");
  imgMeta0->SetPatientID("PATIENT_ID_0");
  
  igtl::TimeStamp::Pointer ts0;
  ts0 = igtl::TimeStamp::New();
  ts0->SetTime(1.2345);

  imgMeta0->SetTimeStamp(ts0);
  imgMeta0->SetSize(512, 512, 64);
  imgMeta0->SetScalarType(igtl::ImageMessage::TYPE_UINT16);

  //---------------------------
  // Create 2nd meta data
  igtl::ImageMetaElement::Pointer imgMeta1;
  imgMeta1 = igtl::ImageMetaElement::New();
  imgMeta1->SetName("IMAGE_DESCRIPTION_1");
  imgMeta1->SetDeviceName("IMAGE_1");
  imgMeta1->SetModality("MRI");
  imgMeta1->SetPatientName("PATIENT_1");
  imgMeta1->SetPatientID("PATIENT_ID_1");
  
  igtl::TimeStamp::Pointer ts1;
  ts1 = igtl::TimeStamp::New();
  ts1->SetTime(2.3456);

  imgMeta1->SetTimeStamp(ts1);
  imgMeta1->SetSize(256, 128, 32);
  imgMeta1->SetScalarType(igtl::ImageMessage::TYPE_UINT16);

  //---------------------------
  // Create 3rd meta data
  igtl::ImageMetaElement::Pointer imgMeta2;
  imgMeta2 = igtl::ImageMetaElement::New();
  imgMeta2->SetName("IMAGE_DESCRIPTION_2");
  imgMeta2->SetDeviceName("IMAGE_2");
  imgMeta2->SetModality("PET");
  imgMeta2->SetPatientName("PATIENT_2");
  imgMeta2->SetPatientID("PATIENT_ID_2");
  
  igtl::TimeStamp::Pointer ts2;
  ts2 = igtl::TimeStamp::New();
  ts2->SetTime(3.4567);

  imgMeta2->SetTimeStamp(ts2);
  imgMeta2->SetSize(256, 256, 32);
  imgMeta2->SetScalarType(igtl::ImageMessage::TYPE_UINT16);

  imgMetaMsg->AddImageMetaElement(imgMeta0);
  imgMetaMsg->AddImageMetaElement(imgMeta1);
  imgMetaMsg->AddImageMetaElement(imgMeta2);

  imgMetaMsg->Pack();
  std::cerr << "Size of pack: " << imgMetaMsg->GetPackSize() << std::endl;
  std::cerr << "Name of type: " << imgMetaMsg->GetDeviceType() << std::endl;
  std::cerr << "Sending a IMGMETA message..." << std::endl;

  socket->Send(imgMetaMsg->GetPackPointer(), imgMetaMsg->GetPackSize());

  return 1;

}








