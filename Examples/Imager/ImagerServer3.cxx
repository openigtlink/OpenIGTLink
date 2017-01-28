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
#include <cstdio>

#include "igtl_header.h"
#include "igtlOSUtil.h"
#include "igtlImageMessage.h"
#include "igtlServerSocket.h"



int GetTestImage(igtl::ImageMessage::Pointer& msg, const char* dir, int i);
void GetRandomTestMatrix(igtl::Matrix4x4& matrix);

int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 4) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <port> <fps> <imgdir>"    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    std::cerr << "    <fps>      : Frequency (fps) to send coordinate" << std::endl;
    std::cerr << "    <imgdir>   : file directory, where \"igtlTestImage[1-5].raw\" are placed." << std::endl;
    std::cerr << "                 (usually, in the Examples/Imager/img directory.)" << std::endl;
    exit(0);
    }

  int    port     = atoi(argv[1]);
  double fps      = atof(argv[2]);
  int    interval = (int) (1000.0 / fps);
  char*  filedir  = argv[3];


  //------------------------------------------------------------
  // Prepare server socket
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
        std::cerr << "A client is connected." << std::endl;
        // Create a message buffer to receive header
        igtl::MessageHeader::Pointer headerMsg;
        headerMsg = igtl::MessageHeader::New();
        headerMsg->InitPack();
        
        // Receive generic header from the socket
        int rs = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
        if (rs == headerMsg->GetPackSize())
        {
          headerMsg->Unpack();
          if (strcmp(headerMsg->GetDeviceType(), "GET_IMAGE") == 0)
          {
            //------------------------------------------------------------
            // loop
            for (int i = 0; i < 100; i ++)
            {
              //------------------------------------------------------------
              // size parameters
              int   size[]     = {256, 256, 1};       // image dimension
              float spacing[]  = {1.0, 1.0, 5.0};     // spacing (mm/pixel)
              int   svsize[]   = {256, 256, 1};       // sub-volume size
              int   svoffset[] = {0, 0, 0};           // sub-volume offset
              int   scalarType = igtl::ImageMessage::TYPE_UINT8;// scalar type
              
              //------------------------------------------------------------
              // Create a new IMAGE type message
              igtl::ImageMessage::Pointer imgMsg = igtl::ImageMessage::New();
              imgMsg->SetDimensions(size);
              imgMsg->SetSpacing(spacing);
              imgMsg->SetScalarType(scalarType);
              imgMsg->SetDeviceName("ImagerClient");
              imgMsg->SetSubVolume(svsize, svoffset);
              imgMsg->SetHeaderVersion(headerMsg->GetHeaderVersion());
#if OpenIGTLink_HEADER_VERSION >= 2
              if (headerMsg->GetHeaderVersion() == IGTL_HEADER_VERSION_2)
              {
                imgMsg->SetMetaDataElement("Patient age", IANA_TYPE_US_ASCII, "25");
                imgMsg->SetMessageID(i);
              }
#endif
              imgMsg->AllocateScalars();
              
              // Following line may be called in case of 16-, 32-, and 64-bit scalar types.
              // imgMsg->SetEndian(igtl::ImageMessage::ENDIAN_BIG);

              //------------------------------------------------------------
              // Set image data (See GetTestImage() bellow for the details)
              GetTestImage(imgMsg, filedir, i % 5);
              
              //------------------------------------------------------------
              // Get random orientation matrix and set it.
              igtl::Matrix4x4 matrix;
              GetRandomTestMatrix(matrix);
              imgMsg->SetMatrix(matrix);
              
              //------------------------------------------------------------
              // Pack (serialize) and send
              imgMsg->Pack();
              socket->Send(imgMsg->GetPackPointer(), imgMsg->GetPackSize());
              
              igtl::Sleep(interval); // wait
            }
          }
        }
      }
    }
  
  //------------------------------------------------------------
  // Close connection (The example code never reachs to this section ...)
  
  socket->CloseSocket();

}


//------------------------------------------------------------
// Function to read test image data
int GetTestImage(igtl::ImageMessage::Pointer& msg, const char* dir, int i)
{

  //------------------------------------------------------------
  // Check if image index is in the range
  if (i < 0 || i >= 5) 
    {
    std::cerr << "Image index is invalid." << std::endl;
    return 0;
    }

  //------------------------------------------------------------
  // Generate path to the raw image file
  char filename[128];
  sprintf(filename, "%s/igtlTestImage%d.raw", dir, i+1);
  std::cerr << "Reading " << filename << "...";

  //------------------------------------------------------------
  // Load raw data from the file
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL)
    {
    std::cerr << "File opeining error: " << filename << std::endl;
    return 0;
    }
  int fsize = msg->GetImageSize();
  size_t b = fread(msg->GetScalarPointer(), 1, fsize, fp);

  fclose(fp);

  std::cerr << "done." << std::endl;

  return 1;
}


//------------------------------------------------------------
// Function to generate random matrix.
void GetRandomTestMatrix(igtl::Matrix4x4& matrix)
{
  /*
  float position[3];
  float orientation[4];

  // random position
  static float phi = 0.0;
  position[0] = 50.0 * cos(phi);
  position[1] = 50.0 * sin(phi);
  position[2] = 0;
  phi = phi + 0.2;

  // random orientation
  static float theta = 0.0;
  orientation[0]=0.0;
  orientation[1]=0.6666666666*cos(theta);
  orientation[2]=0.577350269189626;
  orientation[3]=0.6666666666*sin(theta);
  theta = theta + 0.1;

  igtl::Matrix4x4 matrix;
  igtl::QuaternionToMatrix(orientation, matrix);

  matrix[0][3] = position[0];
  matrix[1][3] = position[1];
  matrix[2][3] = position[2];
  */

  matrix[0][0] = 1.0;  matrix[1][0] = 0.0;  matrix[2][0] = 0.0; matrix[3][0] = 0.0;
  matrix[0][1] = 0.0;  matrix[1][1] = -1.0;  matrix[2][1] = 0.0; matrix[3][1] = 0.0;
  matrix[0][2] = 0.0;  matrix[1][2] = 0.0;  matrix[2][2] = 1.0; matrix[3][2] = 0.0;
  matrix[0][3] = 0.0;  matrix[1][3] = 0.0;  matrix[2][3] = 0.0; matrix[3][3] = 1.0;
  
  igtl::PrintMatrix(matrix);
}

