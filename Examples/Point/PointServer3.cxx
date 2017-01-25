/*=========================================================================

  Program:   OpenIGTLink -- Example for Point Server Program
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <math.h>
#include <cstdlib>

#include "igtlOSUtil.h"
#include "igtlPointMessage.h"
#include "igtlServerSocket.h"

int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 2) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <port> <fps>"    << std::endl;
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
        headerMsg->InitPack();
        
        // Receive generic header from the socket
        int rs = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
        if (rs == headerMsg->GetPackSize())
        {
          headerMsg->Unpack();
          int version = headerMsg->GetHeaderVersion();
          if (strcmp(headerMsg->GetDeviceType(), "GET_POINT") == 0)
          {
            //------------------------------------------------------------
            // loop
            for (int i = 0; i < 100; i ++)
            {
              //---------------------------
              // Create a point message
              igtl::PointMessage::Pointer pointMsg;
              pointMsg = igtl::PointMessage::New();
              pointMsg->SetHeaderVersion(version);
              pointMsg->SetDeviceName("PointSender");
              
              //---------------------------
              // Create 1st point
              igtl::PointElement::Pointer point0;
              point0 = igtl::PointElement::New();
              point0->SetName("POINT_0");
              point0->SetGroupName("GROUP_0");
              point0->SetRGBA(0xFF, 0x00, 0x00, 0xFF);
              point0->SetPosition(10.0, 20.0, 30.0);
              point0->SetRadius(15.0);
              point0->SetOwner("IMAGE_0");
              
              //---------------------------
              // Create 2nd point
              igtl::PointElement::Pointer point1;
              point1 = igtl::PointElement::New();
              point1->SetName("POINT_1");
              point1->SetGroupName("GROUP_0");
              point1->SetRGBA(0x00, 0xFF, 0x00, 0xFF);
              point1->SetPosition(40.0, 50.0, 60.0);
              point1->SetRadius(45.0);
              point1->SetOwner("IMAGE_0");
              
              //---------------------------
              // Create 3rd point
              igtl::PointElement::Pointer point2;
              point2 = igtl::PointElement::New();
              point2->SetName("POINT_2");
              point2->SetGroupName("GROUP_0");
              point2->SetRGBA(0x00, 0x00, 0xFF, 0xFF);
              point2->SetPosition(70.0, 80.0, 90.0);
              point2->SetRadius(75.0);
              point2->SetOwner("IMAGE_0");
              
              //---------------------------
              // Pack into the point message
              pointMsg->AddPointElement(point0);
              pointMsg->AddPointElement(point1);
              pointMsg->AddPointElement(point2);
              pointMsg->SetHeaderVersion(headerMsg->GetHeaderVersion());
#if OpenIGTLink_HEADER_VERSION >= 2
              if (headerMsg->GetHeaderVersion() == IGTL_HEADER_VERSION_2)
              {
                pointMsg->SetMetaDataElement("First patient age", IANA_TYPE_US_ASCII, "22");
                pointMsg->SetMetaDataElement("Second patient age", IANA_TYPE_US_ASCII, "25");
                pointMsg->SetMessageID(i);
              }
#endif
              pointMsg->Pack();
              
              //---------------------------
              // Send
              socket->Send(pointMsg->GetPackPointer(), pointMsg->GetPackSize());
            }
          }
        }
      }

    //------------------------------------------------------------
    // Close connection (The example code never reachs to this section ...)
    }

  socket->CloseSocket();

}


