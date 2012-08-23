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
#include "igtlImageMessage.h"
#include "igtlServerSocket.h"
#include "igtlStatusMessage.h"
#include "igtlPositionMessage.h"

namespace igtl {
igtlMessageHandlerClassMacro(TransformMessage, TestTransformMessageHandler);
igtlMessageHandlerClassMacro(PositionMessage,  TestPositionMessageHandler);
igtlMessageHandlerClassMacro(ImageMessage,     TestImageMessageHandler);

template <class TransformMessage>
void TestTransformMessageHandler::Process(TransformMessage * transMsg)
{
  // Retrive the transform data
  igtl::Matrix4x4 matrix;
  transMsg->GetMatrix(matrix);
  igtl::PrintMatrix(matrix);
  return 1;
}

template <class PositionMessage>
void TestPositionMessageHandler::Process(PositionMessage * positionMsg)
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

template <class ImageMessage>
void TestImageMessageHandler::Process(ImageMessage * imgMsg)
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
}


int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 3) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "    <hostname> : IP or host name"                    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    exit(0);
    }

  char*  hostname = argv[1];
  int    port     = atoi(argv[2]);

  // Connection
  igtl::SessionManager::Pointer sm;
  sm = igtl::SessionManager::New();
  sm->SetModeClient();
  sm->SetAddress(hostname);
  sm->SetPort(port);

  // Set message handers
  igtl::TestImageMessageHander::Pointer imh;
  imh = igtl::ImageMessageHander::New();
  igtl::TransformMessageHander::Pointer tmh;
  tmh = igtl::ImageMessageHander::New();

  sm->AddMessageHandler(tmh);
  sm->AddMessageHandler(imh);

  // Start session
  sm->Start();

  // Do something

  // Stop session
  sm->End();
}



