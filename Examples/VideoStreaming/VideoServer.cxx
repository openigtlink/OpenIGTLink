/*=========================================================================

  Program:   OpenIGTLink -- Example for Tracker Server Program
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <sstream>
#include <map>
#include <stdlib.h>
#include "igtlVideoMessage.h"
#include "igtlMessageDebugFunction.h"
#include "igtl_video.h"
#include "igtl_types.h"
#include "igtl_header.h"
#include "igtl_util.h"
#include "igtlOSUtil.h"
#include "string.h"
#include <cstring>
#include <string>

#if defined(OpenIGTLink_USE_H264)
#include "igtlH264Encoder.h"
#include "igtlH264Decoder.h"
#endif

#if defined(OpenIGTLink_USE_VP9)
#include "igtlVP9Encoder.h"
#include "igtlVP9Decoder.h"
#endif

#if defined (OpenIGTLink_USE_AV1)
#include "igtlAV1Encoder.h"
#include "igtlAV1Decoder.h"
#endif

#include "igtlServerSocket.h"
#include "igtlMultiThreader.h"

using namespace igtl;

int Width = 256;
int Height = 256;
std::string testFileName(OpenIGTLink_SOURCE_ROOTDIR);
int startIndex = 0;
int  inputFrameNum = 2000;

void* ThreadFunction(void* ptr);

typedef struct {
  int   nloop;
  igtl::MutexLock::Pointer glock;
  igtl::Socket::Pointer socket;
  int   interval;
  int   stop;
} ThreadData;

void SendEncodedVideoMessage(igtl::Socket::Pointer& videoMsg, igtl::MutexLock::Pointer& glock, int interval);

int main(int argc, char* argv[])
{
  //------------------------------------------------------------
  // Parse Arguments

  if (argc != 2) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <port> <fps>"    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    exit(0);
    }

  int    port     = atoi(argv[1]);

  igtl::ServerSocket::Pointer serverSocket;
  serverSocket = igtl::ServerSocket::New();
  int r = serverSocket->CreateServer(port);

  if (r < 0)
    {
    std::cerr << "Cannot create a server socket." << std::endl;
    exit(0);
    }
  igtl::MultiThreader::Pointer threader = igtl::MultiThreader::New();
  igtl::MutexLock::Pointer glock = igtl::MutexLock::New();
  ThreadData td;
  igtl::Socket::Pointer socket;
  
  while (1)
    {
    int threadID = -1;
    //------------------------------------------------------------
    // Waiting for Connection
    socket = serverSocket->WaitForConnection(1000);
    
    if (socket.IsNotNull()) // if client connected
      {
      std::cerr << "A client is connected." << std::endl;
      // Create a message buffer to receive header
      td.interval = 100;
      td.glock    = glock;
      td.socket   = socket;
      td.stop     = 0;
      threadID    = threader->SpawnThread((igtl::ThreadFunctionType) &ThreadFunction, &td);
      // Create a message buffer to receive header
      igtl::MessageHeader::Pointer headerMsg;
      headerMsg = igtl::MessageHeader::New();
      //------------------------------------------------------------
      // loop
      for (;;)
        {
        // Initialize receive buffer
        headerMsg->InitPack();

        // Receive generic header from the socket
        bool timeout(false);
        igtlUint64 rs = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize(), timeout);
        if (rs <= 0)
          {
          if (threadID >= 0)
            {
            td.stop = 1;
            threader->TerminateThread(threadID);
            threadID = -1;
            }
          std::cerr << "Disconnecting the client." << std::endl;
          td.socket = NULL;  // VERY IMPORTANT. Completely remove the instance.
          socket->CloseSocket();
          break;
          }
        if (rs != headerMsg->GetPackSize())
          {
          continue;
          }
        }
      }
    }
  
  //------------------------------------------------------------
  // Close connection (The example code never reachs to this section ...)
  
  socket->CloseSocket();

}


void* ThreadFunction(void * ptr)
{
  //------------------------------------------------------------
  // Get thread information
  igtl::MultiThreader::ThreadInfo* info =
  static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);
  
  ThreadData* td = static_cast<ThreadData*>(info->UserData);
  
  //------------------------------------------------------------
  // Get user data
  igtl::MutexLock::Pointer glock = td->glock;
  long interval = td->interval;
  std::cerr << "Interval = " << interval << " (ms)" << std::endl;
  igtl::Socket::Pointer& socket = td->socket;
  GenericEncoder * encoder = NULL;
  
#if defined(OpenIGTLink_USE_VP9)
  VP9Encoder* VP9StreamEncoder = new VP9Encoder();
  VP9StreamEncoder->SetPicWidthAndHeight(Width,Height);
  VP9StreamEncoder->InitializeEncoder();
  VP9StreamEncoder->SetLosslessLink(true);
  VP9StreamEncoder->SetKeyFrameDistance(50);
  encoder = VP9StreamEncoder;
#elif defined(OpenIGTLink_USE_AV1)
  igtlAV1Encoder* AV1StreamEncoder = new igtlAV1Encoder();
  AV1StreamEncoder->SetPicWidthAndHeight(Width, Height);
  AV1StreamEncoder->InitializeEncoder();
  AV1StreamEncoder->SetLosslessLink(true);
  encoder = AV1StreamEncoder;
#elif defined(OpenIGTLink_USE_H264)
  H264Encoder* H264StreamEncoder = new H264Encoder();
  H264StreamEncoder->SetPicWidthAndHeight(Width,Height);
  H264StreamEncoder->InitializeEncoder();
  encoder = H264StreamEncoder;
#endif
  // Get thread information
  int kiPicResSize = Width*Height;
  igtl_uint8* imagePointer = new igtl_uint8[kiPicResSize*3/2];
  memset(imagePointer, 0, Width * Height * 3 / 2);
  SourcePicture* pSrcPic = new SourcePicture();
  pSrcPic->colorFormat = FormatI420;
  pSrcPic->picWidth = Width; // check the test image
  pSrcPic->picHeight = Height;
  pSrcPic->data[0] = imagePointer;
  pSrcPic->data[1] = pSrcPic->data[0] + kiPicResSize;
  pSrcPic->data[2] = pSrcPic->data[1] + kiPicResSize/4;
  pSrcPic->stride[0] = pSrcPic->picWidth;
  pSrcPic->stride[1] = pSrcPic->stride[2] = pSrcPic->stride[0] >> 1;
  
  SourcePicture* pDecodedPic = new SourcePicture();
  pDecodedPic->data[0] = new igtl_uint8[kiPicResSize*3/2];
  memset(pDecodedPic->data[0], 0, Width * Height * 3 / 2);
  for(int i = 0; i <inputFrameNum; i++)
  {
    if(td->stop)
      {
      break;
      }
    std::string sep = "/";
#if defined(_WIN32) || defined(_WIN64)
    sep = "\\";
#endif
    std::stringstream stream;
    stream << (i%6+1);
    std::string imageIndexStr = stream.str();
    std::string testIndexedFileName = std::string(testFileName);
    testIndexedFileName.append(sep).append("Testing").append(sep).append("img").append(sep).append("igtlTestImage").append(imageIndexStr).append(".raw");
    FILE* pFileYUV = NULL;
    pFileYUV = fopen (testIndexedFileName.c_str(), "rb");
    if (pFileYUV != NULL) {
      if(fread (imagePointer, 1, kiPicResSize, pFileYUV ) == (kiPicResSize))
        {
        bool isGrayImage = true;
        igtl::VideoMessage::Pointer videoMessageSend = igtl::VideoMessage::New();
        videoMessageSend->SetDeviceName("Laparoscopy");
        videoMessageSend->SetHeaderVersion(IGTL_HEADER_VERSION_2);
        int iEncFrames = encoder->EncodeSingleFrameIntoVideoMSG(pSrcPic, videoMessageSend, isGrayImage);
        if(iEncFrames == 0 && !td->stop)
          {
          std::cerr<<"Encoding Frame index: " << i << std::endl;
          std::cerr<<"Packet size: " << videoMessageSend->GetPackSize() << std::endl;
          glock->Lock();
          socket->Send(videoMessageSend->GetPackPointer(), videoMessageSend->GetPackSize());
          glock->Unlock();
          igtl::Sleep(interval); // wait
          }
        else
          {
          break;
          }
        }
      if (pFileYUV)
        {
        fclose(pFileYUV);
        pFileYUV = NULL;
        }
      }
    else
      {
      fprintf (stderr, "Unable to open image file, check corresponding path!\n");
      }
    }
  delete pSrcPic;
  pSrcPic = NULL;
  return NULL;
}

  