/*=========================================================================

  Program:   Open IGT Link -- Example for Tracker Server Program
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
#include "igtlPositionMessage.h"
#include "igtlServerSocket.h"

void GetRandomTestVectors(float* position, float* quaternion);

int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 3) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <port> <fps>"    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    std::cerr << "    <fps>      : Frequency (fps) to send coordinate" << std::endl;
    exit(0);
    }

  int    port     = atoi(argv[1]);
  double fps      = atof(argv[2]);
  int    interval = (int) (1000.0 / fps);

  igtl::PositionMessage::Pointer positionMsg;
  positionMsg = igtl::PositionMessage::New();
  positionMsg->SetDeviceName("Tracker");
  positionMsg->SetPackType(igtl::PositionMessage::ALL); // default

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
        float position[3];
        float quaternion[4];
        
        GetRandomTestVectors(position, quaternion);
        positionMsg->SetPosition(position);
        positionMsg->SetQuaternion(quaternion);
        positionMsg->Pack();
        socket->Send(positionMsg->GetPackPointer(), positionMsg->GetPackSize());
        igtl::Sleep(interval); // wait
        }
      }
    }
    
  //------------------------------------------------------------
  // Close connection (The example code never reachs to this section ...)
  
  socket->CloseSocket();

}


//------------------------------------------------------------
// Function to generate random matrix.

void GetRandomTestVectors(float* position, float* quaternion)
{

  // random position
  static float phi = 0.0;
  position[0] = 50.0 * cos(phi);
  position[1] = 50.0 * sin(phi);
  position[2] = 50.0 * cos(phi);
  phi = phi + 0.2;

  // random orientation
  static float theta = 0.0;
  quaternion[0]=0.0;
  quaternion[1]=0.6666666666*cos(theta);
  quaternion[2]=0.577350269189626;
  quaternion[3]=0.6666666666*sin(theta);
  theta = theta + 0.1;

}



