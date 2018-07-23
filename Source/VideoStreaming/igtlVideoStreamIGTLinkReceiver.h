/*=========================================================================
 
 Program:   OpenIGTLink
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#ifndef __igtVideoStreamIGTLinkReceiver_h
#define __igtVideoStreamIGTLinkReceiver_h

#if defined(_WIN32) && !defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
// need link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#endif

#include <fstream>
#include <climits>
#include <cstring>
#include <stdlib.h>
#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlVideoMessage.h"
#include "igtlServerSocket.h"
#include "igtlMultiThreader.h"
#include "igtlUDPClientSocket.h"
#include "igtlMessageRTPWrapper.h"
#include "igtlConditionVariable.h"
#include "igtlTimeStamp.h"
#include "igtlCodecCommonClasses.h"

namespace igtl {

class IGTLCommon_EXPORT VideoStreamIGTLinkReceiver
{
public:
  VideoStreamIGTLinkReceiver(char * fileName);
  
  ~VideoStreamIGTLinkReceiver();
  
  int ProcessVideoStream(igtl_uint8* bitStream, int streamLength);
  
  void SetDecoder(GenericDecoder* decoder);
  
  bool InitializeClient();
  
  void SendStopMessage();
  
  int ParseConfigForClient();
  
  void SetWidth(int iWidth);
  
  void SetHeight(int iHeight);
  
  int GetWidth(){return Width;};
  
  int GetHeight(){return Height;};
  
  void InitializeDecodedFrame();
  
  int RunOnTCPSocket();
  
  int RunOnUDPSocket();
  
  enum
  {
    RunOnTCP = 0,
    RunOnUDP = 1
  };
  
  int GetTransportMethod(){return transportMethod;}
  
  std::string GetDeviceName(){return deviceName;}
  
private:
  
  GenericDecoder* decodeInstance;
  
  std::string deviceName;
  
  int transportMethod;
  
  unsigned char* decodedFrame;
  
  std::string kpOuputFileName;
  
  igtl::MutexLock::Pointer glock;
  
  igtl::ClientSocket::Pointer socket;
  
  igtl::UDPClientSocket::Pointer UDPSocket;
  
  igtl_uint8 * videoMessageBuffer;
  
  int   interval;
  
  ReadConfigFile cRdCfg;
  
  int TCPServerPort;
  
  int UDPClientPort;
  
  char * TCPServerIPAddress;
  
  igtl::MessageRTPWrapper::Pointer rtpWrapper;
  
  char codecType[IGTL_VIDEO_CODEC_NAME_SIZE];
  
  int Width;
  
  int Height;
  
  std::string configFile;

};


} // namespace igtl
#endif
