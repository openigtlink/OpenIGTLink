/*=========================================================================

  Program:   OpenIGTLink -- Example for Image Meta Data Server
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlPointMessage.h"
#include "igtlServerSocket.h"

using std::ifstream;
using std::stringstream;
using std::cerr;

typedef struct {
  double x;
  double y;
  double z;
} Point;

typedef std::vector<Point> PointList;

int SendPointList(igtl::Socket::Pointer& socket, const char *devicename, PointList points);
PointList DefaultPointList();
PointList ReadPointList(char* );

int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments
	PointList pointlist;

  if (argc < 2 || argc > 3)
		{
    // If not correct, print usage
    std::cerr << "Sends OpenIGTLink point list upon POINT_GET request(s)" << std::endl;
    std::cerr << "Usage: " << argv[0] << " <port>"    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    std::cerr << "    <imgdir>   : [optional] file name, wherein each line contains 3 points to send" << std::endl;
    exit(0);
    }

  int port     = atoi(argv[1]);

  if (argc == 2) // check number of arguments
		pointlist = DefaultPointList();
	else if (argc == 3)
    {
    char*  filename = argv[2];
		pointlist = ReadPointList(filename);
    }

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

        std::cerr << "Receiving a message: " << std::endl;
        std::cerr << "    Device Type: \"" << headerMsg->GetDeviceType() << "\"" << std::endl;
        std::cerr << "    Device Name: \"" << headerMsg->GetDeviceName() << "\"" << std::endl;

        // Check data type and receive data body
        if (strcmp(headerMsg->GetDeviceType(), "GET_POINT") == 0)
          {
          socket->Skip(headerMsg->GetBodySizeToRead(), 0);
          SendPointList(socket, headerMsg->GetDeviceName(), pointlist);
          }
        else
          {
          // if the data type is unknown, skip reading.
          socket->Skip(headerMsg->GetBodySizeToRead(), 0);
          }
        }
      }
    }

  //------------------------------------------------------------
  // Close connection (The example code never reaches to this section ...)
  socket->CloseSocket();
}


int SendPointList(igtl::Socket::Pointer& socket, const char* name, PointList points)
{
  std::cout << "Sending PointList" << std::endl;
  //------------------------------------------------------------
  // Allocate Point Message Class
  igtl::PointMessage::Pointer pointMsg;
  pointMsg = igtl::PointMessage::New();
  pointMsg->SetDeviceName("PointSender");

  //---------------------------
  // Create a point message
  int i = 0;
  PointList::iterator pt_iter;
  for (pt_iter = points.begin();
       pt_iter != points.end();
       ++pt_iter)
    {
    igtl::PointElement::Pointer point;
    point = igtl::PointElement::New();
    stringstream pt_name;
    pt_name << "POINT_" << i;

    point->SetName(pt_name.str().c_str());
    point->SetGroupName("GROUP_0");
    point->SetRGBA(0x00, 0x00, 0xFF, 0xFF);
    point->SetPosition(pt_iter->x, pt_iter->y, pt_iter->z);
    point->SetRadius(75.0);
    point->SetOwner("IMAGE_0");
    
    //---------------------------
    // Pack into the point message
    pointMsg->AddPointElement(point);
    i++;
    }
  pointMsg->Pack();
  
  //---------------------------
  // Send
  socket->Send(pointMsg->GetPackPointer(), pointMsg->GetPackSize());

  return 1;
}

//------------------------------------------------------------
// Function to create default point list
PointList DefaultPointList()
{
  PointList points;
  Point pt1 = {10.0, 20.0, 30.0};
  Point pt2 = {40.0, 50.0, 60.0};
  Point pt3 = {70.0, 80.0, 90.0};
  points.push_back(pt1);
  points.push_back(pt2);
  points.push_back(pt3);
  return points;
}


//------------------------------------------------------------
// Function to read test point data
PointList ReadPointList(char* filename)
{
	PointList points;
	
  //------------------------------------------------------------
  //  Load point list from file
  //    file format is x y z\n
	ifstream f_in(filename);
	std::string line;
  if (!f_in.is_open())
    {
    std::cerr << "Error opening file: " << filename << std::endl;
    exit(0);
    }

  Point pt;
  while ( f_in >> pt.x >> pt.y >> pt.z )
		points.push_back(pt);

  std::cerr << "Read: " << points.size() << " points from file." << std::endl;
  return points;
}
