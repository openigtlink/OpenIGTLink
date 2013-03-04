/*=========================================================================

  Program:   OpenIGTLink -- Example for Sending Capability Messasge
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
#include "igtlCapabilityMessage.h"
#include "igtlClientSocket.h"

#include "igtlTransformMessage.h"
#include "igtlImageMessage.h"
#include "igtlLabelMetaMessage.h"

//
// Test comment
//


int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 3) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <hostname> <port>"    << std::endl;
    std::cerr << "    <hostname> : IP or host name"                    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    exit(0);
    }

  char*  hostname = argv[1];
  int    port     = atoi(argv[2]);
  int    interval = (int) (1000);

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
  // Allocate Capability Message Class

  igtl::CapabilityMessage::Pointer capabilityMsg;
  capabilityMsg = igtl::CapabilityMessage::New();
  capabilityMsg->SetDeviceName("Device");

  std::vector<std::string> types;
  types.push_back(std::string("TRANSFORM"));
  types.push_back(std::string("GET_IMAGE"));
  types.push_back(std::string("GET_LBMETA"));
  capabilityMsg->SetTypes(types);
  capabilityMsg->Pack();
  socket->Send(capabilityMsg->GetPackPointer(), capabilityMsg->GetPackSize());
  //------------------------------------------------------------
  // Close connection

  socket->CloseSocket();

}


