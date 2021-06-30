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
#include "igtlPointMessage.h"
#include "igtlClientSocket.h"

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
  std::cerr << "Sending GET_POINT message....." << std::endl;
  igtl::GetPointMessage::Pointer getPointMsg;
  getPointMsg = igtl::GetPointMessage::New();
  getPointMsg->SetDeviceName("PointClient");
  getPointMsg->SetHeaderVersion(version);
  getPointMsg->Pack();
  socket->Send(getPointMsg->GetPackPointer(), getPointMsg->GetPackSize());
  
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
    if (strcmp(headerMsg->GetDeviceName(), "PointSender") == 0)
    {
      std::cerr << "Receiving Point data type." << std::endl;
      
      //------------------------------------------------------------
      // Allocate Point Data Message Class
      
      igtl::PointMessage::Pointer pointData;
      pointData = igtl::PointMessage::New();
      pointData->SetMessageHeader(headerMsg);//Here the version is also set
      pointData->AllocatePack();
      
      // Receive body from the socket
      bool timeout(false);
      socket->Receive(pointData->GetPackBodyPointer(), pointData->GetPackBodySize(), timeout);
      
      // Deserialize the transform data
      // If you want to skip CRC check, call Unpack() without argument.
      int c = pointData->Unpack(1);
      
      if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
      {
        if (pointData->GetHeaderVersion() >= IGTL_HEADER_VERSION_2)
        {
#if OpenIGTLink_HEADER_VERSION >= 2
          int i = 0;
          for (std::map<std::string, std::pair<IANA_ENCODING_TYPE, std::string> >::const_iterator it = pointData->GetMetaData().begin(); it != pointData->GetMetaData().end(); ++it, ++i)
          {
            std::cerr<<"The message ID is:"<< " " << pointData->GetMessageID() << std::endl;
            std::cerr<< it->first << " coding scheme: " << it->second.first << " " << it->second.second << std::endl;
          }
#endif
        }
      }
      igtl::PointElement::Pointer point;
      for (int i = 0;i < pointData->GetNumberOfPointElement(); i++)
      {
        point = igtl::PointElement::New();
        pointData->GetPointElement(i, point);
        igtlFloat32 x,y,z;
        point->GetPosition(x,y,z);
        const char * groupName = point->GetGroupName();
        const char * ownerName = point->GetOwner();
        std::cerr<<"point position: "<<x<< " " << y << " "<< z << " " << std::endl;
        std::cerr<<"groupName: "<< groupName << std::endl;
        std::cerr<<"ownerName: "<< ownerName << std::endl;
      }
      igtl::Sleep(interval);
    }
    else
    {
      std::cerr << "Receiving : " << headerMsg->GetDeviceType() << std::endl;
      socket->Skip(headerMsg->GetBodySizeToRead(), 0);
    }
  }
  
}

