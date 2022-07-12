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
#include <cstring>

#include "igtlOSUtil.h"
#include "igtlVideoMessage.h"
#include "igtlClientSocket.h"


#if defined(OpenIGTLink_USE_H264)
#include "igtlH264Decoder.h"
#endif

#if defined(OpenIGTLink_USE_VP9)
#include "igtlVP9Decoder.h"
#endif

#if defined(OpenIGTLink_USE_AV1)
#include "igtlAV1Decoder.h"
#endif

using namespace igtl;

int ReceiveVideoData(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header, GenericDecoder * decoder);

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
    std::cerr << "    <fps>      : Frequency (fps) to send video frame" << std::endl;
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
  
  GenericDecoder * decoder = NULL;
#if defined(OpenIGTLink_USE_VP9)
  VP9Decoder* VP9StreamDecoder = new VP9Decoder();
  decoder = VP9StreamDecoder;
#endif
#if defined(OpenIGTLink_USE_H264)
  H264Decoder* H264StreamDecoder = new H264Decoder();
  decoder = H264StreamDecoder;
#endif
#if defined(OpenIGTLink_USE_AV1)
  igtlAV1Decoder* AV1StreamDecoder = new igtlAV1Decoder();
  decoder = AV1StreamDecoder;
#endif
  
  //------------------------------------------------------------
  // Ask the server to start pushing tracking data
  std::cerr << "Sending STT_VIDEO message....." << std::endl;
  igtl::StartVideoMessage::Pointer startVideoMsg;
  startVideoMsg = igtl::StartVideoMessage::New();
  startVideoMsg->SetDeviceName("Laparoscopy");
  startVideoMsg->SetTimeInterval(interval);
  startVideoMsg->Pack();
  socket->Send(startVideoMsg->GetPackPointer(), startVideoMsg->GetPackSize());
  
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
    if (strcmp(headerMsg->GetDeviceType(), "VIDEO") == 0)
      {
      ReceiveVideoData(socket, headerMsg, decoder);
      }
    else
      {
      std::cerr << "Receiving : " << headerMsg->GetDeviceType() << std::endl;
      socket->Skip(headerMsg->GetBodySizeToRead(), 0);
      }
    if (++loop >= 250) // if received 100 times
      {
      //------------------------------------------------------------
      // Ask the server to stop pushing tracking data
      std::cerr << "Sending STP_TDATA message....." << std::endl;
      igtl::StopVideoMessage::Pointer stopVideoMsg;
      stopVideoMsg = igtl::StopVideoMessage::New();
      stopVideoMsg->SetDeviceName("Laparoscopy");
      stopVideoMsg->Pack();
      socket->Send(stopVideoMsg->GetPackPointer(), stopVideoMsg->GetPackSize());
      loop = 0;
      }
    }
}


int ReceiveVideoData(igtl::ClientSocket::Pointer& socket, igtl::MessageHeader::Pointer& header, GenericDecoder * decoder)
{
  std::cerr << "Receiving TDATA data type." << std::endl;
  
  //------------------------------------------------------------
  // Allocate TrackingData Message Class
  
  igtl::VideoMessage::Pointer videoMsg;
  videoMsg = igtl::VideoMessage::New();
  videoMsg->SetMessageHeader(header);
  videoMsg->AllocatePack();
  
  // Receive body from the socket
  bool timeout(false);
  socket->Receive(videoMsg->GetPackBodyPointer(), videoMsg->GetPackBodySize(), timeout);
  
  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = videoMsg->Unpack(1);
  
  static int totalFrame = 0;
  SourcePicture* pDecodedPic = new SourcePicture();
  pDecodedPic->data[0] = new igtl_uint8[videoMsg->GetWidth()*videoMsg->GetHeight()*3/2];
  memset(pDecodedPic->data[0], 0, videoMsg->GetWidth()*videoMsg->GetHeight() * 3 / 2);
  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    int iRet= decoder->DecodeVideoMSGIntoSingleFrame(videoMsg.GetPointer(), pDecodedPic);
    if(iRet)
      {
      std::cerr<<"Decoding Frame Index: " << totalFrame << std::endl;
      totalFrame ++;
      }
    else
      {
      delete pDecodedPic;
      pDecodedPic = NULL;
      return 0;
      }
    }
  delete pDecodedPic;
  pDecodedPic = NULL;
  return 1;
}


