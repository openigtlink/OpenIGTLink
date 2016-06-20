/*=========================================================================

  Program:   Open IGT Link -- Example for Full Tracking Data With Errors
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
#include "igtlTrackingDataMessage.h"
#include "igtlFullTrackingDataWithErrorMessage.h"
#include "igtlServerSocket.h"

void GetRandomTestMatrix(igtl::Matrix4x4& matrix);

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

  igtl::FullTrackingDataWithErrorMessage::Pointer msg;
  msg = igtl::FullTrackingDataWithErrorMessage::New();

  igtl::TrackingDataWithErrorElement::Pointer elem;
  elem = igtl::TrackingDataWithErrorElement::New();
  elem->SetType(igtl::TrackingDataElement::TYPE_6D);
  elem->SetName("Tracker");

  igtl::TrackingDataElement::Pointer elemBase;
  elemBase = dynamic_cast<igtl::TrackingDataElement*>(elem.GetPointer());

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
        igtl::Matrix4x4 matrix;
        GetRandomTestMatrix(matrix);

        // Geometrically a bit meaningless,
        // but the whole point of this class is to transmit
        // the full matrix. So we want to see these numbers at the client end.
        matrix[3][0] = i;
        matrix[3][1] = i;
        matrix[3][2] = i;

        msg->ClearTrackingDataElements();

        elem->SetMatrix(matrix);
        elem->SetError(i);

        msg->AddTrackingDataElement(elemBase);

        msg->Pack();

        socket->Send(msg->GetPackPointer(), msg->GetPackSize());
        igtl::Sleep(interval); // wait
        }
      }
    }

  //------------------------------------------------------------
  // Close connection (The example code never reachs to this section ...)

  socket->CloseSocket();

}


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

