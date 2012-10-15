/*=========================================================================

  Program:   OpenIGTLink -- Example for Data Receiving Server Program
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <iomanip>
#include <math.h>
#include <cstdlib>
#include <cstring>

#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlMessageHandler.h"
#include "igtlMessageHandlerMacro.h"
#include "igtlSessionManager.h"
#include "igtlTransformMessage.h"
#include "igtlPositionMessage.h"
#include "igtlImageMessage.h"


//------------------------------------------------------------
// Define message handler classes for TransformMessage,
// PositionMessage and ImageMessage
// igtlMessageHandlerClassMacro() defines a child class of
// igtl::MessageHandler to handle OpenIGTLink messages for
// the message type specified as the first argument. The
// second argument will be used for the name of this 
// message handler class.
igtlMessageHandlerClassMacro(igtl::TransformMessage, TransformHandler);
igtlMessageHandlerClassMacro(igtl::PositionMessage,  PositionHandler);
igtlMessageHandlerClassMacro(igtl::ImageMessage,     ImageHandler);

//------------------------------------------------------------
// You need to describe how the received message is processed
// in Process() function of the message handler class.

// -- Transform message
int TransformHandler::Process(igtl::TransformMessage * transMsg)
{
  // Retrive the transform data
  igtl::Matrix4x4 matrix;
  transMsg->GetMatrix(matrix);
  igtl::PrintMatrix(matrix);
  return 1;
}

// -- Position message
int PositionHandler::Process(igtl::PositionMessage * positionMsg)
{
  // Retrive the transform data
  float position[3];
  float quaternion[4];
  
  positionMsg->GetPosition(position);
  positionMsg->GetQuaternion(quaternion);
  
  std::cerr << "position   = (" << position[0] << ", " << position[1] << ", " << position[2] << ")" << std::endl;
  std::cerr << "quaternion = (" << quaternion[0] << ", " << quaternion[1] << ", "
            << quaternion[2] << ", " << quaternion[3] << ")" << std::endl << std::endl;
  return 1;
}

// -- Image message
int ImageHandler::Process(igtl::ImageMessage * imgMsg)
{
  // Retrive the image data
  int   size[3];          // image dimension
  float spacing[3];       // spacing (mm/pixel)
  int   svsize[3];        // sub-volume size
  int   svoffset[3];      // sub-volume offset
  int   scalarType;       // scalar type
  
  scalarType = imgMsg->GetScalarType();
  imgMsg->GetDimensions(size);
  imgMsg->GetSpacing(spacing);
  imgMsg->GetSubVolume(svsize, svoffset);
  
  std::cerr << "Device Name           : " << imgMsg->GetDeviceName() << std::endl;
  std::cerr << "Scalar Type           : " << scalarType << std::endl;
  std::cerr << "Dimensions            : ("
            << size[0] << ", " << size[1] << ", " << size[2] << ")" << std::endl;
  std::cerr << "Spacing               : ("
            << spacing[0] << ", " << spacing[1] << ", " << spacing[2] << ")" << std::endl;
  std::cerr << "Sub-Volume dimensions : ("
            << svsize[0] << ", " << svsize[1] << ", " << svsize[2] << ")" << std::endl;
  std::cerr << "Sub-Volume offset     : ("
            << svoffset[0] << ", " << svoffset[1] << ", " << svoffset[2] << ")" << std::endl;
  return 1;
}


int main(int argc, char* argv[])
{

  //------------------------------------------------------------
  // Parse Arguments
  if (argc != 2) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    exit(0);
    }

  int    port     = atoi(argv[1]);

  //------------------------------------------------------------
  // Create a session manager
  igtl::SessionManager::Pointer sm;
  sm = igtl::SessionManager::New();
  sm->SetMode(igtl::SessionManager::MODE_SERVER);
  sm->SetPort(port);

  //------------------------------------------------------------
  // Create message handlers
  TransformHandler::Pointer tmh = TransformHandler::New();
  PositionHandler::Pointer pmh  = PositionHandler::New();
  ImageHandler::Pointer imh     = ImageHandler::New();

  //------------------------------------------------------------
  // Register the message handlers to the session manager
  sm->AddMessageHandler(tmh);
  sm->AddMessageHandler(pmh);
  sm->AddMessageHandler(imh);

  //------------------------------------------------------------
  // Start session
  if (sm->Connect())
    {
    while (1)
      {
      int r = sm->ProcessMessage();
      if (r == 0) // Disconnected
        {
        break;
        }
      igtl::Sleep(400); // Wait for 400 ms
      }
    // Stop session
    sm->Disconnect();
    }
  
}



