/*=========================================================================

  Program:   OpenIGTLink -- Example for String Echo Server Program
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
#include "igtlStringMessage.h"
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

  igtl::ServerSocket::Pointer serverSocket = igtl::ServerSocket::New();
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
    igtl::MessageHeader::Pointer hdrMsg = igtl::MessageHeader::New();

    while (socket.IsNotNull() && socket->GetConnected())
      {      
      hdrMsg->InitPack();
      bool timeout(false);
      igtlUint64 r = socket->Receive(hdrMsg->GetPackPointer(), hdrMsg->GetPackSize(), timeout);

      // check message
      if (r == 0) 
        {
        socket->CloseSocket();
        continue;
        }
      if (r != hdrMsg->GetPackSize())
        continue;

      // get data
      hdrMsg->Unpack();
      igtl::StringMessage::Pointer strMsg(igtl::StringMessage::New());
      strMsg->SetMessageHeader(hdrMsg);
      strMsg->AllocatePack();
      timeout = false;
      socket->Receive(strMsg->GetPackBodyPointer(), strMsg->GetPackBodySize(), timeout);
      int c = strMsg->Unpack();

      // echo message back
      std::cout << "Echoing: " << strMsg->GetString() << std::endl;
      strMsg->SetDeviceName("StringEchoServer");
      strMsg->Pack();
      socket->Send(strMsg->GetPackPointer(), strMsg->GetPackSize());
      }
    }
    
  //------------------------------------------------------------
  // Close connection (The example code never reachs to this section ...)
  
  socket->CloseSocket();

}

