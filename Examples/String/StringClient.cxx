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
#include <math.h>
#include <cstdlib>

#include "igtlOSUtil.h"
#include "igtlStringMessage.h"
#include "igtlClientSocket.h"

#define N_STRINGS 5

const char * testString[N_STRINGS] = {
  "OpenIGTLink",
  "Network",
  "Communication",
  "Protocol",
  "Image Guided Therapy",
};

int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 4) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <hostname> <port> <string>"    << std::endl;
    std::cerr << "    <hostname> : IP or host name"                    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    std::cerr << "    <fps>      : Frequency (fps) to send string" << std::endl;
    exit(0);
    }

  char*  hostname = argv[1];
  int    port     = atoi(argv[2]);
  double fps      = atof(argv[3]);
  int    interval = (int) (1000.0 / fps);

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

  igtl::StringMessage::Pointer stringMsg;
  stringMsg = igtl::StringMessage::New();
  

  //------------------------------------------------------------
  // loop
  int i = 0;
  while (1)
    {
    stringMsg->SetDeviceName("StringMessage");
    std::cout << "Sending string: " << testString[i] << std::endl;
    stringMsg->SetString(testString[i]);
    stringMsg->Pack();
    socket->Send(stringMsg->GetPackPointer(), stringMsg->GetPackSize());
    igtl::Sleep(interval); // wait
    i = (i + 1) % N_STRINGS;
    }

  //------------------------------------------------------------
  // Close connection

  socket->CloseSocket();
}
