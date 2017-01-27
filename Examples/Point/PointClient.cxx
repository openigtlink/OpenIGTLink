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

#include "igtlOSUtil.h"
#include "igtlPointMessage.h"
#include "igtlClientSocket.h"


int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 3) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <hostname> <port> <fps>"    << std::endl;
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
  // Allocate Transform Message Class

  igtl::PointMessage::Pointer pointMsg;
  pointMsg = igtl::PointMessage::New();
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
  pointMsg->Pack();
  
  //------------------------------------------------------------
  // Send
  socket->Send(pointMsg->GetPackPointer(), pointMsg->GetPackSize());
  
  
  //------------------------------------------------------------
  // Close the socket
  socket->CloseSocket();

}

