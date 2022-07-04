/*=========================================================================

  Program:   OpenIGTLink -- Example for Video Meta Data Server
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
#include "igtlVideoMessage.h"
#include "igtlServerSocket.h"
#include "igtlVideoMetaMessage.h"


int SendVideoMeta(igtl::Socket::Pointer& socket, const char* name);

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
        if (strcmp(headerMsg->GetDeviceType(), "GET_VMETA") == 0)
          {
          std::cerr << "Received a GET_VMETA message." << std::endl;
          //socket->Skip(headerMsg->GetBodySizeToRead(), 0);
          SendVideoMeta(socket, headerMsg->GetDeviceName());
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


int SendVideoMeta(igtl::Socket::Pointer& socket, const char* name)
{

  //------------------------------------------------------------
  // Allocate Status Message Class

  igtl::VideoMetaMessage::Pointer videoMetaMsg;
  videoMetaMsg = igtl::VideoMetaMessage::New();
  // NOTE: the server should send a message with the same device name
  // as the received query message.
  videoMetaMsg->SetDeviceName(name);

  //---------------------------
  // Create 1st meta data
  igtl::VideoMetaElement::Pointer videoMeta0;
  videoMeta0 = igtl::VideoMetaElement::New();
  videoMeta0->SetName("VIDEO_DESCRIPTION_0");
  videoMeta0->SetDeviceName("VIDEO_0");
  videoMeta0->SetPatientName("PATIENT_0");
  videoMeta0->SetPatientID("PATIENT_ID_0");
  videoMeta0->SetSize(512, 512, 64);
  igtl::Matrix4x4 matrix;
  matrix[0][0] = 1.0;  matrix[1][0] = 0.0;  matrix[2][0] = 0.0; matrix[3][0] = -75.0;
  matrix[0][1] = 0.0;  matrix[1][1] = -1.0;  matrix[2][1] = 0.0; matrix[3][1] = -50.0;
  matrix[0][2] = 0.0;  matrix[1][2] = 0.0;  matrix[2][2] = 1.0; matrix[3][2] = 10.5;
  matrix[0][3] = 0.0;  matrix[1][3] = 0.0;  matrix[2][3] = 0.0; matrix[3][3] = 1.0;
  videoMeta0->SetScalarType(igtl::VideoMessage::TYPE_UINT8);

  //---------------------------
  // Create 2nd meta data
  igtl::VideoMetaElement::Pointer videoMeta1;
  videoMeta1 = igtl::VideoMetaElement::New();
  videoMeta1->SetName("VIDEO_DESCRIPTION_1");
  videoMeta1->SetDeviceName("VIDEO_1");
  videoMeta1->SetPatientName("PATIENT_1");
  videoMeta1->SetPatientID("PATIENT_ID_1");
  videoMeta1->SetSize(256, 256, 64);
  igtl::Matrix4x4 matrix1;
  matrix1[0][0] = -1.0;  matrix1[1][0] = 0.0;  matrix1[2][0] = 0.0; matrix1[3][0] = -15.0;
  matrix1[0][1] = 0.0;  matrix1[1][1] = 1.0;  matrix1[2][1] = 0.0; matrix1[3][1] = 50.0;
  matrix1[0][2] = 0.0;  matrix1[1][2] = 0.0;  matrix1[2][2] = 1.0; matrix1[3][2] = 30.5;
  matrix1[0][3] = 0.0;  matrix1[1][3] = 0.0;  matrix1[2][3] = 0.0; matrix1[3][3] = 1.0;
  videoMeta1->SetScalarType(igtl::VideoMessage::TYPE_UINT16);
  
  //---------------------------
  // Create 3rd meta data
  igtl::VideoMetaElement::Pointer videoMeta2;
  videoMeta2 = igtl::VideoMetaElement::New();
  videoMeta2->SetName("VIDEO_DESCRIPTION_2");
  videoMeta2->SetDeviceName("VIDEO_2");
  videoMeta2->SetPatientName("PATIENT_2");
  videoMeta2->SetPatientID("PATIENT_ID_2");
  videoMeta2->SetSize(256, 128, 1);
  igtl::Matrix4x4 matrix2;
  matrix2[0][0] = 1.0;  matrix2[1][0] = 0.0;  matrix2[2][0] = 0.0; matrix2[3][0] = 15.0;
  matrix2[0][1] = 0.0;  matrix2[1][1] = 1.0;  matrix2[2][1] = 0.0; matrix2[3][1] = 250.0;
  matrix2[0][2] = 0.0;  matrix2[1][2] = 0.0;  matrix2[2][2] = -1.0; matrix2[3][2] = 7.6;
  matrix2[0][3] = 0.0;  matrix2[1][3] = 0.0;  matrix2[2][3] = 0.0; matrix2[3][3] = 1.0;
  videoMeta2->SetScalarType(igtl::VideoMessage::TYPE_UINT16);
  
  videoMetaMsg->AddVideoMetaElement(videoMeta0);
  videoMetaMsg->AddVideoMetaElement(videoMeta1);
  videoMetaMsg->AddVideoMetaElement(videoMeta2);

  videoMetaMsg->Pack();
  std::cerr << "Size of pack: " << videoMetaMsg->GetPackSize() << std::endl;
  std::cerr << "Name of type: " << videoMetaMsg->GetDeviceType() << std::endl;
  std::cerr << "Sending a VIDEOMETA message..." << std::endl;

  socket->Send(videoMetaMsg->GetPackPointer(), videoMetaMsg->GetPackSize());

  return 1;

}








