/*=========================================================================

  Program:   OpenIGTLink -- Example for Tracker Server Program
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
#include "igtlCapabilityMessage.h"
#include "igtlServerSocket.h"

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
  int    interval = (int) 1000;

  //------------------------------------------------------------
  // Allocate Capability Message Class

  igtl::CapabilityMessage::Pointer capabilityMsg;
  capabilityMsg = igtl::CapabilityMessage::New();
  capabilityMsg->SetDeviceName("Device");

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
      //------------------------------------------------------------
      // loop
      for (int i = 0; i < 100; i ++)
        {
        std::vector<std::string> types;
        types.push_back(std::string("TRANSFORM"));
        types.push_back(std::string("GET_IMAGE"));
        types.push_back(std::string("GET_LBMETA"));
        capabilityMsg->SetTypes(types);
        capabilityMsg->Pack();
        socket->Send(capabilityMsg->GetPackPointer(), capabilityMsg->GetPackSize());
        igtl::Sleep(interval); // wait
        }
      }
    }
    
  //------------------------------------------------------------
  // Close connection (The example code never reachs to this section ...)
  
  socket->CloseSocket();

}


