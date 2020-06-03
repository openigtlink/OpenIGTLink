/*=========================================================================

  Program:   OpenIGTLink -- Example for String Message Client Program
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <cstdlib>

#include "igtlOSUtil.h"
#include "igtlStringMessage.h"
#include "igtlClientSocket.h"

int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments
  if (argc != 5) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <hostname> <port> <device-name> <string>" << std::endl
	      << "    <hostname>    : IP or host name" << std::endl
	      << "    <port>        : Port # (18944 in Slicer default)" << std::endl
	      << "    <device-name> : Device Name"  << std::endl
	      << "    <string>      : Message payload" << std::endl;
    exit(0);
    }

  char*  hostname = argv[1];
  int    port     = atoi(argv[2]);
  char*  device   = argv[3];
  char*  string   = argv[4];

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

  std::cout << "Sending igtl::StringMessage to " << hostname
	    << ":" << port << " from device \"" << device
	    << "\". Message: " << string << std::endl;

  //------------------------------------------------------------
  // Allocate String Message Class
  igtl::StringMessage::Pointer stringMsg;
  stringMsg = igtl::StringMessage::New();
  stringMsg->SetDeviceName(device);
  stringMsg->SetString(string);
  stringMsg->Pack();
  socket->Send(stringMsg->GetPackPointer(), stringMsg->GetPackSize());
  igtl::Sleep(1000.0);

  //------------------------------------------------------------
  // Close connection
  socket->CloseSocket();
}
