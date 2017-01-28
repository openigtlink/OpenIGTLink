/*=========================================================================

  Program:   OpenIGTLink -- Example for Tracker Client Program II
                              (POSITION data type)
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
#include "igtlPositionMessage.h"
#include "igtlClientSocket.h"

void GetRandomTestVectors(float* position, float* quaternion);

int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 4) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <hostname> <port> <fps>"    << std::endl;
    std::cerr << "    <hostname> : IP or host name"                    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    std::cerr << "    <fps>      : Frequency (fps) to send coordinate" << std::endl;
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

  igtl::PositionMessage::Pointer positionMsg;
  positionMsg = igtl::PositionMessage::New();
  positionMsg->SetDeviceName("Tracker");
  positionMsg->SetPackType(igtl::PositionMessage::ALL); // default

  //------------------------------------------------------------
  // loop
  while (1)
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

  //------------------------------------------------------------
  // Close connection

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

  std::cerr << "position   = (" << position[0] << ", " << position[1] << ", " << position[2] << ")" << std::endl;
  std::cerr << "quaternion = (" << quaternion[0] << ", " << quaternion[1] << ", "
            << quaternion[2] << ", " << quaternion[3] << ")" << std::endl << std::endl;
}



