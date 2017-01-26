/*=========================================================================

  Program:   OpenIGTLink
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

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
#include <cstring>
#include <stdlib.h>
#include "sha1.h"
#include "igtl_header.h"
#include "igtl_video.h"
#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlVideoMessage.h"
#include "igtlServerSocket.h"
#include "igtlUDPServerSocket.h"
#include "igtlMultiThreader.h"
#include "igtlConditionVariable.h"
#include "igtlMessageRTPWrapper.h"
#include "igtlTimeStamp.h"
#include "codec_def.h"
#include "codec_app_def.h"
#include "read_config.h"
#include "wels_const.h"
#include "welsencUtil.h"

class ISVCEncoder;

class VideoStreamIGTLinkServer
{
public:
  VideoStreamIGTLinkServer(char *argv[]);
  ~VideoStreamIGTLinkServer(){};
  
  /**
   Start the server, this function will be holding the main program for a client connection.
   */
  int StartTCPServer();
  
  /**
   Start the server, this function will be holding the main program for a client connection.
   */
  int StartUDPServer();
  
  /**
   Parse the configuration file to initialize the encoder and server.
   */
  bool InitializeEncoderAndServer();
  
  /**
   Set the input frame pointer , the encoder will read the frame from the pointer
   */
  void SetInputFramePointer(uint8_t* picPointer);
  
  /**
   Encode a frame, for performance issue, before encode the frame, make sure the frame pointer is updated with a new frame.
   Otherwize, the old frame will be encoded.
   */
  int EncodeSingleFrame();
  
  /**
   Pack the encoded frame into a OpenIGTLink message and send the message to a client.
   */
  void SendCompressedData();
  
  /**
   Pack the original frame into a OpenIGTLink message and send the message to a client.
   */
  void SendOriginalData();
  
  /**
   Set the server to wait for STT command or just send the bitstream when connection is setup.
   */
  void SetWaitSTTCommand(bool needSTTCommand){ waitSTTCommand = needSTTCommand;};
  
  /**
   Get the encoder and server initialization status.
   */
  bool GetInitializationStatus(){return InitializationDone;};
  
  /**
   Get Server connection status, true for connected, false for not connected
   */
  bool GetServerConnectStatus(){return serverConnected;}
  
  /**
   Encode the video stream from a source file
   */
  void* EncodeFile(void);
  
  /**
   Get the type of encoded frame
   */
  int GetVideoFrameType(){return videoFrameType;};
  
  int ParseConfigForServer();
  
  enum TransportMethod
  {
    UseTCP,
    UseUDP
  };
  
  
  //void* ThreadFunctionServer(void*);
  
  static bool CompareHash (const unsigned char* digest, const char* hashStr);
  
  igtl::SimpleMutexLock* glock;
  
  igtl::Socket::Pointer socket;
  
  igtl::ServerSocket::Pointer serverSocket;
  
  igtl::UDPServerSocket::Pointer serverUDPSocket;
  
  igtl::MessageRTPWrapper::Pointer rtpWrapper;
  
  ISVCEncoder*  pSVCEncoder;
  
  SEncParamExt sSvcParam;
  
  SFilesSet fs;
  // for configuration file
  
  CReadConfig cRdCfg;
  
  SFrameBSInfo sFbi;
  
  SSourcePicture* pSrcPic;
  
  igtlUint64 encodeStartTime;
  
  igtlUint64 encodeEndTime;
  
  igtl::ConditionVariable::Pointer conditionVar;
  
  int videoFrameType;
  
  int   interval;
  
  bool  useCompress;
  
  int   netWorkBandWidth; // in kbps
  
  char  codecName[IGTL_VIDEO_CODEC_NAME_SIZE];
  
  int   serverConnected;
  
  int   serverPortNumber;
  
  char* clientIPAddress;
  
  int   clientPortNumber;
  
  int argc;
  
  std::string augments;
    
  bool waitSTTCommand;
  
  std::string deviceName;
  
  int transportMethod;
  
  bool InitializationDone;
  
  igtl::TimeStamp::Pointer ServerTimer;
  
  class encodedFrame
  {
  public:
    encodedFrame(){messageDataLength = 0;};
    encodedFrame(encodedFrame const &anotherFrame){};
    ~encodedFrame(){};
    unsigned char messagePackPointer[RTP_PAYLOAD_LENGTH*(16384-2)];  // we use 14 bits for fragment number, 2^14 = 16384. maximum
    uint32_t messageDataLength;
  };
  
  std::map<igtlUint32, uint8_t*> incommingFrames;
  
  std::map<igtlUint32, encodedFrame*> encodedFrames;
  
  int iTotalFrameToEncode;
  
  int StartSendPacketThread();
  
  int StartReadFrameThread(int frameRate);
  
private:
  void CheckEncodedFrameMap();
  
  void ReadInFileWithFrameRate(int rate);
  
  unsigned long totalCompressedDataSize;
};