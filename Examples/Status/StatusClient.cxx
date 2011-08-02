/*=========================================================================

  Program:   Open IGT Link -- Example for Sending Status Messasge
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
#include "igtlStatusMessage.h"
#include "igtlClientSocket.h"

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
  // Allocate Status Message Class

  igtl::StatusMessage::Pointer statusMsg;
  statusMsg = igtl::StatusMessage::New();
  statusMsg->SetDeviceName("Device");

  //------------------------------------------------------------
  // loop
  for (int i = 0; i < 100; i ++)
    {
    statusMsg->SetCode(igtl::StatusMessage::STATUS_OK);
    statusMsg->SetSubCode(128);
    statusMsg->SetErrorName("OK!");
    statusMsg->SetStatusString("This is a test to send status message.");
    statusMsg->Pack();
    socket->Send(statusMsg->GetPackPointer(), statusMsg->GetPackSize());
    igtl::Sleep(interval); // wait
    }

  //------------------------------------------------------------
  // Close connection

  socket->CloseSocket();

}


