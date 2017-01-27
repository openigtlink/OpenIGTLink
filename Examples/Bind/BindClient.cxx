/*=========================================================================

  Program:   OpenIGTLink -- Example for Tracker Client Program
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
#include "igtlBindMessage.h"
#include "igtlStringMessage.h"
#include "igtlTransformMessage.h"
#include "igtlClientSocket.h"

#define N_STRINGS 5
const char * testString[N_STRINGS] = {
  "OpenIGTLink",
  "Network",
  "Communication",
  "Protocol",
  "Image Guided Therapy",
};

void GetRandomTestMatrix(igtl::Matrix4x4& matrix);

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
  // Allocate Child Message Classes (String and Transform)

  igtl::StringMessage::Pointer stringMsg;
  stringMsg = igtl::StringMessage::New();
  stringMsg->SetDeviceName("StringMessage");

  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();
  transMsg->SetDeviceName("Tracker");

  //------------------------------------------------------------
  // Allocate Bind message class

  igtl::BindMessage::Pointer bindMsg;
  bindMsg = igtl::BindMessage::New();
  bindMsg->SetDeviceName("BindMessage");
  
  //------------------------------------------------------------
  // loop
  int i = 0;
  while (1)
    {
    stringMsg->SetString(testString[i]);
    stringMsg->Pack();
    
    igtl::Matrix4x4 matrix;
    GetRandomTestMatrix(matrix);
    transMsg->SetMatrix(matrix);
    transMsg->Pack();

    bindMsg->Init();
    bindMsg->AppendChildMessage(stringMsg);
    bindMsg->AppendChildMessage(transMsg);
    bindMsg->Pack();
    
    socket->Send(bindMsg->GetPackPointer(), bindMsg->GetPackSize());
    igtl::Sleep(interval); // wait
    }

  //------------------------------------------------------------
  // Close connection

  socket->CloseSocket();

}

//------------------------------------------------------------
// Function to generate random matrix.

void GetRandomTestMatrix(igtl::Matrix4x4& matrix)
{
  float position[3];
  float orientation[4];

  // random position
  static float phi = 0.0;
  position[0] = 50.0 * cos(phi);
  position[1] = 50.0 * sin(phi);
  position[2] = 50.0 * cos(phi);
  phi = phi + 0.2;

  // random orientation
  static float theta = 0.0;
  orientation[0]=0.0;
  orientation[1]=0.6666666666*cos(theta);
  orientation[2]=0.577350269189626;
  orientation[3]=0.6666666666*sin(theta);
  theta = theta + 0.1;

  //igtl::Matrix4x4 matrix;
  igtl::QuaternionToMatrix(orientation, matrix);

  matrix[0][3] = position[0];
  matrix[1][3] = position[1];
  matrix[2][3] = position[2];
  
  igtl::PrintMatrix(matrix);
}

