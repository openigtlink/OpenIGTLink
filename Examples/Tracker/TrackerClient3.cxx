/*=========================================================================

  Program:   Open IGT Link -- Example for Tracker Client Program
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
#include "igtlTransformMessage.h"
#include "igtlClientSocket.h"

#define MAX_DEVICE 4

void GetRandomTestMatrix(igtl::Matrix4x4& matrix, float phi, float theta);

int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc < 4 || argc > 4 + MAX_DEVICE) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <hostname> <port> <fps> [<devname 1> .. <devname N>]"    << std::endl;
    std::cerr << "    <hostname> : IP or host name"                    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    std::cerr << "    <fps>      : Frequency (fps) to send coordinate" << std::endl;
    std::cerr << "    <devname 1> .. <devname N>: Device names (N < " << MAX_DEVICE << ")" << std::endl;
    exit(0);
    }

  char*  hostname = argv[1];
  int    port     = atoi(argv[2]);
  double fps      = atof(argv[3]);
  int    interval = (int) (1000.0 / fps);
  int    numdev   = argc - 4;
  const char*  devicename[4];

  if (numdev == 0)
    {
    numdev = 1;
    devicename[0] = "Tracker";
    }
  else
    {
    for (int i = 0; i < numdev; i ++)
      {
      devicename[i] = argv[i+4];
      }
    }

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

  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();

  float phi[MAX_DEVICE];
  float theta[MAX_DEVICE];
  float incr[MAX_DEVICE];
  for (int i = 0; i < numdev; i ++)
    {
    phi[i]   = 0.3*(float)i;
    theta[i] = 0.2*(float)i;
    incr[i]  = 0.5*(float)(i+1);
    }


  //------------------------------------------------------------
  // loop
  igtl::TimeStamp::Pointer ts = igtl::TimeStamp::New();

  while (1)
    {
    for (int i = 0; i < numdev; i ++)
      {
      transMsg->SetDeviceName(devicename[i]);
      igtl::Matrix4x4 matrix;
      GetRandomTestMatrix(matrix, phi[i], theta[i]);
      transMsg->SetMatrix(matrix);
      ts->GetTime();
      transMsg->SetTimeStamp(ts);
      transMsg->Pack();

      igtlUint32 sec;
      igtlUint32 nsec;
      ts->GetTimeStamp(&sec, &nsec);
      std::cerr << "Time Stamp: sec = " << sec << ", nsec = " << nsec << std::endl;
      socket->Send(transMsg->GetPackPointer(), transMsg->GetPackSize());
      phi[i]   = phi[i] + 0.2*incr[i];
      theta[i] = theta[i] + 0.1*incr[i];
      }
    igtl::Sleep(interval); // wait
    }

  //------------------------------------------------------------
  // Close connection
  socket->CloseSocket();

}

//------------------------------------------------------------
// Function to generate random matrix.

void GetRandomTestMatrix(igtl::Matrix4x4& matrix, float phi, float theta)
{
  float position[3];
  float orientation[4];

  // random position
  position[0] = 50.0 * cos(phi);
  position[1] = 50.0 * sin(phi);
  position[2] = 50.0 * cos(phi);

  // random orientation
  orientation[0]=0.0;
  orientation[1]=0.6666666666*cos(theta);
  orientation[2]=0.577350269189626;
  orientation[3]=0.6666666666*sin(theta);

  //igtl::Matrix4x4 matrix;
  igtl::QuaternionToMatrix(orientation, matrix);

  matrix[0][3] = position[0];
  matrix[1][3] = position[1];
  matrix[2][3] = position[2];
  
  igtl::PrintMatrix(matrix);
}

