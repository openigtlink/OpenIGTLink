/*=========================================================================

  Program:   OpenIGTLink -- Example for Imager Client Program
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
#include "igtlClientSocket.h"

int ReceiveImageData(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header, int loop);
int SaveTestImage(igtl::ImageMessage::Pointer& msg, int i);

int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 5) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <hostname> <port> <fps> <imgdir>"    << std::endl;
    std::cerr << "    <hostname> : IP or host name"                    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    std::cerr << "    <fps>      : Frequency (fps) to send coordinate" << std::endl;
    std::cerr << "    Version    : Version number send to the server" << std::endl;
    exit(0);
    }
  
  char*  hostname = argv[1];
  int    port     = atoi(argv[2]);
  double fps      = atof(argv[3]);
  int    version  = atoi(argv[4]);
  int    interval = (int) (1000.0 / fps);
  //------------------------------------------------------------
  // Establish Connection
  igtl::ClientSocket::Pointer socket;
  socket = igtl::ClientSocket::New();
  int r = socket->ConnectToServer(hostname, port);
  if (version > IGTL_HEADER_VERSION_2 || version < IGTL_HEADER_VERSION_1)
  {
    std::cerr << "Invalid version number" << std::endl;
    exit(0);
  }
  if (r != 0)
  {
    std::cerr << "Cannot connect to the server." << std::endl;
    exit(0);
  }

  //------------------------------------------------------------
  // Ask the server to start pushing tracking data
  std::cerr << "Sending Get_Image message....." << std::endl;
  igtl::GetImageMessage::Pointer getImageMsg;
  getImageMsg = igtl::GetImageMessage::New();
  getImageMsg->SetDeviceName("ImageClient");
  getImageMsg->SetHeaderVersion(version);
  getImageMsg->Pack();
  socket->Send(getImageMsg->GetPackPointer(), getImageMsg->GetPackSize());
  
  int loop = 0;
  
  while (1)
  {
    //------------------------------------------------------------
    // Wait for a reply
    igtl::MessageHeader::Pointer headerMsg;
    headerMsg = igtl::MessageHeader::New();
    headerMsg->InitPack();
    bool timeout(false);
    igtlUint64 rs = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize(), timeout);
    if (rs == 0)
    {
      std::cerr << "Connection closed." << std::endl;
      socket->CloseSocket();
      exit(0);
    }
    if (rs != headerMsg->GetPackSize())
    {
      std::cerr << "Message size information and actual data size don't match." << std::endl;
      socket->CloseSocket();
      exit(0);
    }
    
    headerMsg->Unpack();
    if (headerMsg->GetHeaderVersion() != version)
    {
      std::cerr << "Version of the client and server don't match." << std::endl;
      socket->CloseSocket();
      exit(0);
    }
    if (strcmp(headerMsg->GetDeviceName(), "ImagerClient") == 0)
    {
      ReceiveImageData(socket, headerMsg, loop);
      igtl::Sleep(interval);
    }
    else
    {
      std::cerr << "Receiving : " << headerMsg->GetDeviceType() << std::endl;
      socket->Skip(headerMsg->GetBodySizeToRead(), 0);
    }
    if (++loop >= 100) // if received 100 times
    {
      //------------------------------------------------------------
      // Ask the server to stop pushing tracking data
      std::cerr << "Sending STP_IMAGE message....." << std::endl;
      igtl::StopImageMessage::Pointer stopImageMsg;
      stopImageMsg = igtl::StopImageMessage::New();
      stopImageMsg->SetDeviceName("ImageClient");
      stopImageMsg->Pack();
      socket->Send(stopImageMsg->GetPackPointer(), stopImageMsg->GetPackSize());
      loop = 0;
    }
  }

}


//------------------------------------------------------------
// Function to read test image data
int ReceiveImageData(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header,int loop)
{

  std::cerr << "Receiving TDATA data type." << std::endl;
  
  //------------------------------------------------------------
  // Allocate TrackingData Message Class
  
  igtl::ImageMessage::Pointer imageData;
  imageData = igtl::ImageMessage::New();
  imageData->SetMessageHeader(header);//Here the version is also set
  imageData->AllocatePack();
  
  // Receive body from the socket
  bool timeout(false);
  socket->Receive(imageData->GetPackBodyPointer(), imageData->GetPackBodySize(), timeout);
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = imageData->Unpack(1);
  
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
  {
#if OpenIGTLink_HEADER_VERSION >= 2
    if (imageData->GetHeaderVersion() >= IGTL_HEADER_VERSION_2)
    {
      int i = 0;
      for (std::map<std::string, std::pair<IANA_ENCODING_TYPE, std::string> >::const_iterator it = imageData->GetMetaData().begin(); it != imageData->GetMetaData().end(); ++it, ++i)
      {
        std::cerr<<"The message ID is:"<< " " << imageData->GetMessageID() << std::endl;
        std::cerr<< it->first << " coding scheme: " << it->second.first << " " << it->second.second << std::endl;
      }
    }
#endif
    SaveTestImage(imageData, loop);
    return 1;
  }
  return 0;
}


//------------------------------------------------------------
// Function to read test image data
int SaveTestImage(igtl::ImageMessage::Pointer& msg, int i)
{
  
  //------------------------------------------------------------
  // Check if image index is in the range
  if (i < 0 || i >= 100)
  {
    std::cerr << "Image index is invalid." << std::endl;
    return 0;
  }
  
  //------------------------------------------------------------
  // Generate path to the raw image file
  char filename[128];
  sprintf(filename, "igtlSaveImage%d.raw", i+1);
  std::cerr << "Saving " << filename << "...";
  
  //------------------------------------------------------------
  // Load raw data from the file
  FILE *fp = fopen(filename, "wb");
  if (fp == NULL)
  {
    std::cerr << "File opeining error: " << filename << std::endl;
    return 0;
  }
  int fsize = msg->GetImageSize();
  size_t b = fwrite(msg->GetScalarPointer(), 1, fsize, fp);
  
  fclose(fp);
  
  std::cerr << "done." << std::endl;
  
  return 1;
}

