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
#include "igtlServerSocket.h"
#include "igtlPolyDataMessage.h"


int SendPolyData(igtl::Socket::Pointer& socket, const char* name);

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
        int rs = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
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
        if (strcmp(headerMsg->GetDeviceType(), "GET_POLYDATA") == 0)
          {
          std::cerr << "Received a GET_POLYDATA message." << std::endl;
          //socket->Skip(headerMsg->GetBodySizeToRead(), 0);
          SendPolyData(socket, headerMsg->GetDeviceName());
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


int SendPolyData(igtl::Socket::Pointer& socket, const char* name)
{

  //------------------------------------------------------------
  // Allocate Status Message Class
  igtl::PolyDataMessage::Pointer polyDataMsg;
  polyDataMsg = igtl::PolyDataMessage::New();
  // NOTE: the server should send a message with the same device name
  // as the received query message.
  polyDataMsg->SetDeviceName(name);

  // Geometry data
  static igtlFloat32 pointsData[8][3]={{0,0,0}, {1,0,0}, {1,1,0}, {0,1,0},
                                    {0,0,1}, {1,0,1}, {1,1,1}, {0,1,1}};
  static igtlUint32 polyData[6][4]={{0,3,2,1}, {4,5,6,7}, {0,1,5,4},
                                 {1,2,6,5}, {2,3,7,6}, {3,0,4,7}};
  static igtlFloat32 attributeData[8]={0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};

  // Create point array
  igtl::PolyDataPointArray::Pointer pointArray;
  pointArray = igtl::PolyDataPointArray::New();
  for (unsigned int i = 0; i < 8; i ++)
    {
    pointArray->AddPoint(pointsData[i]);
    }
  polyDataMsg->SetPoints(pointArray);

  // Create polygon array
  igtl::PolyDataCellArray::Pointer polyArray;
  polyArray = igtl::PolyDataCellArray::New();
  for (unsigned int i = 0; i < 6; i ++)
    {
    polyArray->AddCell(4, polyData[i]);
    }
  polyDataMsg->SetPolygons(polyArray);

  // Create attribute array
  igtl::PolyDataAttribute::Pointer attribute;
  attribute = igtl::PolyDataAttribute::New();
  attribute->SetType(igtl::PolyDataAttribute::POINT_SCALAR);
  attribute->SetName("attr");
  attribute->SetSize(8);
  attribute->SetData(attributeData);
  polyDataMsg->ClearAttributes();
  polyDataMsg->AddAttribute(attribute);

  polyDataMsg->Pack();
  std::cerr << "Size of pack: " << polyDataMsg->GetPackSize() << std::endl;
  std::cerr << "Name of type: " << polyDataMsg->GetDeviceType() << std::endl;
  std::cerr << "Sending a POLYDATA message..." << std::endl;

  socket->Send(polyDataMsg->GetPackPointer(), polyDataMsg->GetPackSize());

  return 1;

}








