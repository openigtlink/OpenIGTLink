/*=========================================================================
 
 Program:   VideoStreamIGTLinkServer
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlVideoStreamIGTLinkServer.h"

namespace igtl {

static void* ThreadFunctionServer(void* ptr);

VideoStreamIGTLinkServer::VideoStreamIGTLinkServer(char *argv)
{
  this->videoEncoder = NULL;
  pSrcPic = new SourcePicture();
  pSrcPic->colorFormat = FormatI420;
  pSrcPic->timeStamp = 0;
  this->serverConnected = false;
  this->waitSTTCommand = true;
  this->serverIsSet = false;
  this->serverPortNumber = -1;
  this->deviceName = "";
  this->serverSocket = igtl::ServerSocket::New();
  this->serverUDPSocket = igtl::UDPServerSocket::New();
  this->socket = igtl::Socket::New();;
  this->conditionVar = igtl::ConditionVariable::New();
  this->glock = igtl::SimpleMutexLock::New();
  this->glockInFrame = igtl::SimpleMutexLock::New();
  this->threader = igtl::MultiThreader::New();
  this->rtpWrapper = igtl::MessageRTPWrapper::New();
  this->ServerTimer = igtl::TimeStamp::New();
  this->incommingFrames.clear();
  this->encodedFrames.clear();
  this->netWorkBandWidth = 10000; // in Kbps
  this->interval = 30; //in ms
  this->transportMethod = UseTCP;
  this->totalCompressedDataSize = 0;
  this->iTotalFrameToEncode = -1;
  this->sendPacketThreadID = -1;
  this->readFrameThreadID = -1;
  this->serverThreadID = -1;
  this->augments = std::string(argv);
  if(this->augments.c_str())
    {
    SetupServer();
    }
}

int VideoStreamIGTLinkServer::SetEncoder(GenericEncoder* encoder)
{
  this->videoEncoder = encoder;
  videoEncoder->SetPicWidthAndHeight(pSrcPic->picWidth,pSrcPic->picHeight);
  return 0;
}

int VideoStreamIGTLinkServer::StartTCPServer ()
{
  if (this->serverIsSet)
    {

    if(this->transportMethod==VideoStreamIGTLinkServer::UseTCP)
      {
      serverThreadID = threader->SpawnThread((igtl::ThreadFunctionType)&ThreadFunctionServer, this);
      this->glock->Lock();
      while(!this->serverConnected)
        {
        this->conditionVar->Wait(this->glock);
        igtl::Sleep(10);
        }
      this->glock->Unlock();
      return true;
      }
    }
  else
    {
    return false;
    }
  return false;
}

int VideoStreamIGTLinkServer::StartUDPServer ()
{
  if (this->serverIsSet)
    {
    
    if (this->transportMethod == VideoStreamIGTLinkServer::UseUDP)
      {
      int r = -1;
      if (this->serverUDPSocket.IsNotNull())
        {
        this->serverUDPSocket->CloseSocket();
        }
      r = this->serverUDPSocket->CreateUDPServer();
      if (r < 0)
        {
        std::cerr << "Cannot create a server socket." << std::endl;
        exit(0);
        }
      return true;
      }
    }
  else
    {
    return false;
    }
  return false;
}

int VideoStreamIGTLinkServer::ParseConfigForServer()
{
  std::string strTag[4];
  while (!cRdCfg.EndOfFile()) {
    strTag->clear();
    long iRd = cRdCfg.ReadLine (&strTag[0]);
    if (iRd > 0) {
      if (strTag[0].empty())
        continue;
      if (strTag[0].compare ("ServerPortNumber") == 0) {
        this->serverPortNumber = atoi (strTag[1].c_str());
        if(this->serverPortNumber<0 || this->serverPortNumber>65535)
          {
          fprintf (stderr, "Invalid parameter for server port number should between 0 and 65525.");
          return 1;
          }
      }
      if (strTag[0].compare ("ClientIPAddress") == 0) {
        this->clientIPAddress = new char[IP4AddressStrLen];
        memcpy(this->clientIPAddress, strTag[1].c_str(), IP4AddressStrLen);
        if(!inet_addr(this->clientIPAddress))
          {
          fprintf (stderr, "Invalid parameter for IP address");
          return 1;
          }
      }
      if (strTag[0].compare ("ClientPortNumber") == 0) {
        this->clientPortNumber = atoi (strTag[1].c_str());
        if(this->clientPortNumber<0 || this->clientPortNumber>65535)
          {
          fprintf (stderr, "Invalid parameter for server port number should between 0 and 65525.");
          return 1;
          }
      }
      if (strTag[0].compare ("InputFile") == 0)
        {
        this->strSeqFile =strTag[1].c_str();
        }
      if (strTag[0].compare ("SourceWidth") == 0)
        {
        this->pSrcPic->picWidth = atoi(strTag[1].c_str());
        pSrcPic->stride[0] = this->pSrcPic->picWidth;
        pSrcPic->stride[1] = pSrcPic->stride[2] = pSrcPic->stride[0] >> 1;
        }
      if (strTag[0].compare ("SourceHeight") == 0)
        {
        this->pSrcPic->picHeight = atoi(strTag[1].c_str());
        }
      if (strTag[0].compare ("TotalFrameToEncode") == 0)
        {
        this->iTotalFrameToEncode = atoi(strTag[1].c_str());
        }
      if (strTag[0].compare ("OutputBitStreamFile") == 0)
        {
        this->strOutputFile =strTag[1].c_str();
        }
      if (strTag[0].compare ("CodecConfigurationFile") == 0)
        {
        this->encoderConfigFile = strTag[1];
        }
      if (strTag[0].compare ("DeviceName") == 0)
        {
        this->deviceName =strTag[1].c_str();
        }
      if (strTag[0].compare ("TransportMethod") == 0)
        {
        this->transportMethod = atoi(strTag[1].c_str());
        }
      if (strTag[0].compare ("NetWorkBandWidth") == 0)
        {
        this->netWorkBandWidth = atoi(strTag[1].c_str());
        int netWorkBandWidthInBPS = netWorkBandWidth * 1000; //networkBandwidth is in kbps
        int time = floor(8*RTP_PAYLOAD_LENGTH*1e9/netWorkBandWidthInBPS+ 1.0); // the needed time in nanosecond to send a RTP payload.
        this->rtpWrapper->packetIntervalTime = time; // in nanoSecond
        }
    }
  }
  pSrcPic->data[0] = new unsigned char[pSrcPic->picHeight*pSrcPic->picWidth*3/2];
  pSrcPic->data[1] = pSrcPic->data[0]+pSrcPic->picHeight*pSrcPic->picWidth;
  pSrcPic->data[2] = pSrcPic->data[1]+pSrcPic->picHeight*pSrcPic->picWidth/4;
  if (this->transportMethod == 1 )
    {
    if (this->clientIPAddress && this->clientPortNumber)
      {
      this->serverUDPSocket->AddClient(this->clientIPAddress, this->clientPortNumber, 0);
      }
    else
      {
      return 1;
      }
    }
  return 0;
}

int VideoStreamIGTLinkServer::InitializeEncoder()
{
  int iRet = 1;
  if( this->videoEncoder && (pSrcPic->picWidth*pSrcPic->picHeight>0) && this->encoderConfigFile.c_str())
    {
    this->videoEncoder->SetConfigurationFile(encoderConfigFile);
    this->videoEncoder->SetPicWidthAndHeight(pSrcPic->picWidth,pSrcPic->picHeight);
    iRet = this->videoEncoder->InitializeEncoder();
    }
  return iRet;
}

static void* ThreadFunctionServer(void* ptr)
{
  //------------------------------------------------------------
  // Get thread information
  igtl::MultiThreader::ThreadInfo* info =
  static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);
  
  VideoStreamIGTLinkServer* parentObj = static_cast<VideoStreamIGTLinkServer*>(info->UserData);
  int r = -1;
  if (parentObj->serverSocket.IsNotNull())
    {
    parentObj->serverSocket->CloseSocket();
    }
  r = parentObj->serverSocket->CreateServer(parentObj->serverPortNumber);
  if (r < 0)
    {
    std::cerr << "Cannot create a server socket." << std::endl;
    exit(0);
    }
  parentObj->socket = igtl::Socket::New();
  
  while (1)
    {
    //------------------------------------------------------------
    // Waiting for Connection
    parentObj->serverConnected     = false;
    parentObj->socket = parentObj->serverSocket->WaitForConnection(1000);
    
    if (parentObj->socket.IsNotNull()) // if client connected
      {
      std::cerr << "A client is connected." << std::endl;
      igtl::MessageHeader::Pointer headerMsg;
      headerMsg = igtl::MessageHeader::New();
      if (!parentObj->waitSTTCommand)
        {
        // Create a message buffer to receive header
        strncpy(parentObj->codecName, "H264", IGTL_VIDEO_CODEC_NAME_SIZE);
        parentObj->serverIsSet = false;
        parentObj->serverConnected     = true;
        parentObj->conditionVar->Signal();
        while (parentObj->serverConnected)
          {
          headerMsg->InitPack();
          int rs = parentObj->socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
          if (rs == 0)
            {
            std::cerr << "Disconnecting the client." << std::endl;
            break;
            }
          else
            {
            igtl::Sleep(10);
            }
          }
        }
      else if (parentObj->waitSTTCommand)
        {
        //------------------------------------------------------------
        // loop
        for (;;)
          {
          // Initialize receive buffer
          headerMsg->InitPack();
          
          // Receive generic header from the socket
          int rs = parentObj->socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
          if (rs == 0)
            {
            std::cerr << "Disconnecting the client." << std::endl;
            break;
            }
          if (rs != headerMsg->GetPackSize())
            {
            continue;
            }
          
          // Deserialize the header
          headerMsg->Unpack();
          
          // Check data type and receive data body
          if (strcmp(headerMsg->GetDeviceType(), "STP_VIDEO") == 0)
            {
            parentObj->glock->Lock();
            parentObj->socket->Skip(headerMsg->GetBodySizeToRead(), 0);
            std::cerr << "Received a STP_VIDEO message." << std::endl;
            std::cerr << "Disconnecting the client." << std::endl;
            parentObj->serverIsSet = false;
            parentObj->serverConnected = false;
            if (parentObj->socket.IsNotNull())
              {
              parentObj->socket->CloseSocket();
              }
            parentObj->glock->Unlock();
            parentObj->socket = NULL;  // VERY IMPORTANT. Completely remove the instance.
            break;
            }
          else if (strcmp(headerMsg->GetDeviceType(), "STT_VIDEO") == 0)
            {
            std::cerr << "Received a STT_VIDEO message." << std::endl;
            
            igtl::StartVideoMessage::Pointer startVideoMsg;
            startVideoMsg = igtl::StartVideoMessage::New();
            startVideoMsg->SetMessageHeader(headerMsg);
            startVideoMsg->AllocatePack();
            parentObj->glock->Lock();
            parentObj->socket->Receive(startVideoMsg->GetPackBodyPointer(), startVideoMsg->GetPackBodySize());
            parentObj->glock->Unlock();
            int c = startVideoMsg->Unpack(1);
            if (c & igtl::MessageHeader::UNPACK_BODY && strcmp(startVideoMsg->GetCodecType().c_str(), "H264")) // if CRC check is OK
              {
              parentObj->interval = startVideoMsg->GetTimeInterval();
              strncpy(parentObj->codecName, startVideoMsg->GetCodecType().c_str(), IGTL_VIDEO_CODEC_NAME_SIZE);
              parentObj->serverConnected     = true;
              parentObj->conditionVar->Signal();
              }
            }
          else
            {
            std::cerr << "Receiving : " << headerMsg->GetDeviceType() << std::endl;
            parentObj->glock->Lock();
            parentObj->socket->Skip(headerMsg->GetBodySizeToRead(), 0);
            parentObj->glock->Unlock();
            }
          igtl::Sleep(100);
          }
        }
      }
    }
  
  //------------------------------------------------------------
  // Close connection (The example code never reaches to this section ...)
  parentObj->serverSocket->CloseSocket();
  parentObj->glock->Lock();
  if (parentObj->socket.IsNotNull())
    {
    parentObj->socket->CloseSocket();
    }
  parentObj->glock->Unlock();
  parentObj->socket = NULL;  // VERY IMPORTANT. Completely remove the instance.
  parentObj->serverSocket = NULL;
}

void VideoStreamIGTLinkServer::SetSrcPicWidth(int width)
{
  pSrcPic->picWidth = width;
}

void VideoStreamIGTLinkServer::SetSrcPicHeight(int height)
{
  pSrcPic->picHeight = height;
}

int VideoStreamIGTLinkServer::SetupServer()
{
  //------------------------------------------------------------
  int iRet = 0;
  if (this->videoEncoder == NULL)
    iRet = 1;
  
  this->iTotalFrameToEncode = 0;
  
  // Preparing encoding process
  
  // Inactive with sink with output file handler
  FILE* pFpBs = NULL;
  if (pSrcPic == NULL) {
    iRet = 1;
    goto INSIDE_MEM_FREE;
  }
  //fill default pSrcPic
  pSrcPic->colorFormat = FormatI420;//videoFormatI420;
  pSrcPic->timeStamp = 0;
  
  // if configure file exit, reading configure file firstly
  cRdCfg.OpenFile(this->augments.c_str());// to do get the first augments from this->augments.
  if (cRdCfg.ExistFile())
    {
    cRdCfg.OpenFile(this->augments.c_str());// reset the file read pointer to the beginning.
    iRet = ParseConfigForServer();
    if (iRet) {
      fprintf (stderr, "parse server parameter config file failed.\n");
      iRet = 1;
      goto INSIDE_MEM_FREE;
    }
    }
  else
    {
    fprintf (stderr, "Specified file: %s not exist, maybe invalid path or parameter settting.\n",
             cRdCfg.GetFileName().c_str());
    iRet = 1;
    goto INSIDE_MEM_FREE;
    }
  
  //finish reading the configurations
  igtl_uint32 iSourceWidth, iSourceHeight, kiPicResSize;
  iSourceWidth = pSrcPic->picWidth;
  iSourceHeight = pSrcPic->picHeight;
  kiPicResSize = iSourceWidth * iSourceHeight * 3 >> 1;
  
  //update pSrcPic
  pSrcPic->stride[0] = iSourceWidth;
  pSrcPic->stride[1] = pSrcPic->stride[2] = pSrcPic->stride[0] >> 1;
  pSrcPic->stride[3] = 0;
  
  this->serverIsSet = true;
  return true;
INSIDE_MEM_FREE:
  if (pFpBs) {
    fclose (pFpBs);
    pFpBs = NULL;
  }
  if (pSrcPic) {
    delete pSrcPic;
    pSrcPic = NULL;
  }
  this->serverConnected = false;
  this->serverIsSet = false;
  return iRet;
}

struct serverPointer
{
  VideoStreamIGTLinkServer* server;
};

static void* ThreadFunctionReadFrameFromFile(void* ptr)
{
  // Get thread information
  igtl::MultiThreader::ThreadInfo* info =
  static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);
  serverPointer parentObj = *(static_cast<serverPointer*>(info->UserData));
  int kiPicResSize = parentObj.server->pSrcPic->picWidth*parentObj.server->pSrcPic->picHeight*3>>1;
  igtl_int32 iFrameNumInFile = -1;
  FILE* pFileYUV = NULL;
  pFileYUV = fopen (parentObj.server->strSeqFile.c_str(), "rb");
  if (pFileYUV != NULL) {
#if defined(_WIN32) || defined(_WIN64)
#if _MSC_VER >= 1400
    if (!_fseeki64 (pFileYUV, 0, SEEK_END)) {
      igtl_int64 i_size = _ftelli64 (pFileYUV);
      _fseeki64 (pFileYUV, 0, SEEK_SET);
      iFrameNumInFile = max ((igtl_int32) (i_size / kiPicResSize), iFrameNumInFile);
    }
#else
    if (!fseek (pFileYUV, 0, SEEK_END)) {
      igtl_int64 i_size = ftell (pFileYUV);
      fseek (pFileYUV, 0, SEEK_SET);
      iFrameNumInFile = max ((igtl_int32) (i_size / kiPicResSize), iFrameNumInFile);
    }
#endif
#else
    if (!fseeko (pFileYUV, 0, SEEK_END)) {
      igtl_int64 i_size = ftello (pFileYUV);
      fseeko (pFileYUV, 0, SEEK_SET);
      iFrameNumInFile = std::max((igtl_int32) (i_size / kiPicResSize), iFrameNumInFile);
    }
#endif
    igtl_int32 iFrameIdx = 0;
    while((igtl_int32)parentObj.server->iTotalFrameToEncode > 0)
      {
      iFrameIdx = 0;
#if defined(_WIN32) || defined(_WIN64)
#if _MSC_VER >= 1400
      _fseeki64(pFileYUV, 0, SEEK_SET);
#else
      fseek(pFileYUV, 0, SEEK_SET);
#endif
#else
      fseeko(pFileYUV, 0, SEEK_SET);
#endif
      while (iFrameIdx < iFrameNumInFile && iFrameIdx < parentObj.server->iTotalFrameToEncode) {
        bool bCanBeRead = false;
        igtl_uint8* pYUV = new igtl_uint8[kiPicResSize];
        bCanBeRead = (fread (pYUV, 1, kiPicResSize, pFileYUV) == kiPicResSize);
        parentObj.server->glockInFrame->Lock();
        if (parentObj.server->incommingFrames.size()>3)
          {
          std::map<igtlUint32, igtl_uint8*>::iterator it = parentObj.server->incommingFrames.begin();
          delete it->second;
          it->second = NULL;
          parentObj.server->incommingFrames.erase(it);
          }
        parentObj.server->incommingFrames.insert(std::pair<igtl_uint32, igtl_uint8*>(0, pYUV));
        parentObj.server->glockInFrame->Unlock();
        parentObj.server->iTotalFrameToEncode = parentObj.server->iTotalFrameToEncode - 1; // excluding skipped frame time
        igtl::Sleep(parentObj.server->interval);
        if (!bCanBeRead)
          break;
      }
      }
    if (pFileYUV)
      {
      fclose(pFileYUV);
      pFileYUV = NULL;
      }
  } else {
    fprintf (stderr, "Unable to open source sequence file (%s), check corresponding path!\n",
             parentObj.server->strSeqFile.c_str());
  }
  
  return NULL;
}


static void* ThreadFunctionSendPacket(void* ptr)
{
  // Get thread information
  igtl::MultiThreader::ThreadInfo* info =
  static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);
  serverPointer parentObj = *(static_cast<serverPointer*>(info->UserData));
  while(1)
    {
    parentObj.server->glock->Lock();
    unsigned int frameNum = parentObj.server->encodedFrames.size();
    parentObj.server->glock->Unlock();
    if(frameNum)
      {
      parentObj.server->glock->Lock();
      VideoStreamIGTLinkServer::encodedFrame* frameCopy = new VideoStreamIGTLinkServer::encodedFrame();
      std::map<igtlUint32, VideoStreamIGTLinkServer::encodedFrame*>::iterator it = parentObj.server->encodedFrames.begin();
      frameCopy->messageDataLength = it->second->messageDataLength;
      memcpy(frameCopy->messagePackPointer,it->second->messagePackPointer,it->second->messageDataLength);
      delete it->second;
      it->second = NULL;
      parentObj.server->encodedFrames.erase(it);
      if (parentObj.server->transportMethod == VideoStreamIGTLinkServer::UseUDP)
        {
        parentObj.server->rtpWrapper->WrapMessageAndSend(parentObj.server->serverUDPSocket, frameCopy->messagePackPointer, frameCopy->messageDataLength);
        }
      else if(parentObj.server->transportMethod == VideoStreamIGTLinkServer::UseTCP)
        {
        if(parentObj.server->socket)
          {
          parentObj.server->socket->Send(frameCopy->messagePackPointer, frameCopy->messageDataLength);
          }
        }
      parentObj.server->glock->Unlock();
      delete frameCopy;
      }
    igtl::Sleep(1);
    }
}

int VideoStreamIGTLinkServer::StartReadFrameThread(int frameRate)
{
  this->interval = 1000/frameRate;
  serverPointer ptr;
  ptr.server = this;
  readFrameThreadID = threader->SpawnThread((igtl::ThreadFunctionType)&ThreadFunctionReadFrameFromFile, &ptr);
  igtl::Sleep(10);
  return readFrameThreadID;
}

int VideoStreamIGTLinkServer::StartSendPacketThread()
{
  serverPointer ptr;
  ptr.server = this;
  sendPacketThreadID = threader->SpawnThread((igtl::ThreadFunctionType)&ThreadFunctionSendPacket, &ptr);
  igtl::Sleep(10);
  return sendPacketThreadID;
}

void VideoStreamIGTLinkServer::CheckEncodedFrameMap()
{
  if(this->encodedFrames.size()>5)
    {
    std::map<igtlUint32, VideoStreamIGTLinkServer::encodedFrame*>::iterator it = this->encodedFrames.begin();
    delete it->second;
    it->second = NULL;
    this->encodedFrames.erase(it);
    }
}

void VideoStreamIGTLinkServer::SendOriginalData()
{
  int kiPicResSize = pSrcPic->picWidth*pSrcPic->picHeight*3>>1;
  igtl_uint8* pYUV = new igtl_uint8[kiPicResSize];
  static int messageID = -1;
  while(this->iTotalFrameToEncode)
    {
    this->glockInFrame->Lock();
    int frameNum = this->incommingFrames.size();
    this->glockInFrame->Unlock();
    while(frameNum)
      {
      this->glockInFrame->Lock();
      std::map<igtlUint32, igtl_uint8*>::iterator it = this->incommingFrames.begin();
      memcpy(pYUV,it->second,kiPicResSize);
      delete it->second;
      it->second = NULL;
      this->incommingFrames.erase(it);
      frameNum = this->incommingFrames.size();
      this->glockInFrame->Unlock();
      messageID ++;
      igtl::VideoMessage::Pointer videoMsg;
      videoMsg = igtl::VideoMessage::New();
      videoMsg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
      videoMsg->SetDeviceName(this->deviceName.c_str());
      videoMsg->SetBitStreamSize(kiPicResSize);
      videoMsg->AllocateScalars();
      int endian = (igtl_is_little_endian() == 1 ? IGTL_VIDEO_ENDIAN_LITTLE : IGTL_VIDEO_ENDIAN_BIG);
      videoMsg->SetEndian(endian); //little endian is 2 big endian is 1
      videoMsg->SetWidth(pSrcPic->picWidth);
      videoMsg->SetHeight(pSrcPic->picHeight);
      videoMsg->SetMessageID(messageID);
      memcpy(videoMsg->GetPackFragmentPointer(2), pYUV, kiPicResSize);
      ServerTimer->GetTime();
      videoMsg->SetTimeStamp(ServerTimer);
      videoMsg->Pack();
      encodedFrame* frame = new encodedFrame();
      memcpy(frame->messagePackPointer, videoMsg->GetPackPointer(), videoMsg->GetBufferSize());
      frame->messageDataLength = videoMsg->GetBufferSize();
      this->glock->Lock();
      this->CheckEncodedFrameMap();
      this->encodedFrames.insert(std::pair<igtlUint32, VideoStreamIGTLinkServer::encodedFrame*>(messageID, frame));
      this->glock->Unlock();
      }
    }
  delete[] pYUV;
  pYUV = NULL;
}

int VideoStreamIGTLinkServer::EncodeFile(void)
{
  if(!this->videoEncoder)
    {
    return -1;
    }
  if(!this->serverIsSet)
    {
    this->SetupServer();
    }
  igtl_int64 iStart = 0, iTotal = 0;
  
  int picSize = pSrcPic->picWidth*pSrcPic->picHeight;
  
  igtl_int32 iFrameIdx = 0;
  this->totalCompressedDataSize = 0;
  int iActualFrameEncodedCount = 0;
  while(this->iTotalFrameToEncode)
    {
    this->glockInFrame->Lock();
    int frameNum = this->incommingFrames.size();
    this->glockInFrame->Unlock();
    while(frameNum)
      {
      // To encoder this frame
      this->glockInFrame->Lock();
      std::map<igtlUint32, igtl_uint8*>::iterator it = this->incommingFrames.begin();
      memcpy(this->pSrcPic->data[0],it->second,picSize*3/2);
      delete it->second;
      it->second = NULL;
      this->incommingFrames.erase(it);
      frameNum = this->incommingFrames.size();
      this->glockInFrame->Unlock();
      this->ServerTimer->GetTime();
      this->encodeStartTime = this->ServerTimer->GetTimeStampInNanoseconds();
      iStart = this->encodeStartTime;
      igtl::VideoMessage::Pointer videoMsg = igtl::VideoMessage::New();
      videoMsg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
      videoMsg->SetDeviceName(this->deviceName.c_str());
      int iEncFrames = this->videoEncoder->EncodeSingleFrameIntoVideoMSG(this->pSrcPic, videoMsg, false);
      this->ServerTimer->GetTime();
      this->encodeEndTime = this->ServerTimer->GetTimeStampInNanoseconds();
      iTotal += this->encodeEndTime - iStart;
      ++ iFrameIdx;
      if (this->videoEncoder->GetVideoFrameType() == FrameTypeSkip) {
        continue;
      }
      
      if (iEncFrames == ResultSuccess ) {
        static int messageID = -1;
        messageID++;
        this->totalCompressedDataSize += videoMsg->GetPackedBitStreamSize();
        encodedFrame* frame = new encodedFrame();
        memcpy(frame->messagePackPointer, videoMsg->GetPackPointer(), videoMsg->GetBufferSize());
        frame->messageDataLength = videoMsg->GetBufferSize();
        this->glock->Lock();
        this->CheckEncodedFrameMap();
        this->encodedFrames.insert(std::pair<igtlUint32, VideoStreamIGTLinkServer::encodedFrame*>(messageID, frame));
        this->glock->Unlock();
        igtl::Sleep(5);
        iActualFrameEncodedCount ++;
      } else {
        fprintf (stderr, "EncodeFrame(), ret: %d, frame index: %d.\n", iEncFrames, iFrameIdx);
      }
      if (iActualFrameEncodedCount%10 == 0) {
        double dElapsed = iTotal / 1e9;
        float totalFrameSize = iActualFrameEncodedCount*3/2.0*this->pSrcPic->picWidth*this->pSrcPic->picHeight;
        printf ("Width:\t\t%d\nHeight:\t\t%d\nFrames:\t\t%d\nencode time:\t%f sec\nFPS:\t\t%f fps\nCompressionRate:\t\t%f\n",
                this->pSrcPic->picWidth, this->pSrcPic->picHeight,
                iActualFrameEncodedCount, dElapsed, (iActualFrameEncodedCount * 1.0) / dElapsed, totalCompressedDataSize/totalFrameSize);
      }
      }
    }
  return 0;
}

void VideoStreamIGTLinkServer::Stop()
{
  if(serverThreadID>=0)
    threader->TerminateThread(serverThreadID);
  if(readFrameThreadID>=0)
    threader->TerminateThread(readFrameThreadID);
  if(sendPacketThreadID>=0)
    threader->TerminateThread(sendPacketThreadID);
  this->serverSocket->CloseSocket();
}

int VideoStreamIGTLinkServer::EncodeSingleFrame(igtl_uint8* picPointer, bool isGrayImage)
{
  int encodeRet = -1;
  if (this->serverIsSet == true && this->videoEncoder)
    {
    int iSourceWidth = pSrcPic->picWidth;
    int iSourceHeight = pSrcPic->picHeight;
    pSrcPic->data[0] = picPointer;
    pSrcPic->data[1] = pSrcPic->data[0] + (iSourceWidth * iSourceHeight);
    pSrcPic->data[2] = pSrcPic->data[1] + (iSourceWidth * iSourceHeight >> 2);
    igtl::VideoMessage::Pointer videoMsg = igtl::VideoMessage::New();
    encodeRet = this->videoEncoder->EncodeSingleFrameIntoVideoMSG(this->pSrcPic, videoMsg, isGrayImage);
    this->videoFrameType = videoEncoder->GetVideoFrameType();
    }
  return encodeRet;
}

}//namespace igtl

