/*=========================================================================
 
 Program:   VideoStreamIGTLinkServer
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "VideoStreamIGTLinkServer.h"
#include "welsencUtil.cpp"

static void* ThreadFunctionServer(void* ptr);

void UpdateHashFromFrame (SFrameBSInfo& info, SHA1Context* ctx) {
  for (int i = 0; i < info.iLayerNum; ++i) {
    const SLayerBSInfo& layerInfo = info.sLayerInfo[i];
    int layerSize = 0;
    for (int j = 0; j < layerInfo.iNalCount; ++j) {
      layerSize += layerInfo.pNalLengthInByte[j];
    }
    SHA1Input (ctx, layerInfo.pBsBuf, layerSize);
  }
}

VideoStreamIGTLinkServer::VideoStreamIGTLinkServer(char *argv[])
{
  this->pSVCEncoder = NULL;
  memset (&sFbi, 0, sizeof (SFrameBSInfo));
  pSrcPic = new SSourcePicture;
  
#ifdef _MSC_VER
  _setmode (_fileno (stdin), _O_BINARY);  /* thanks to Marcoss Morais <morais at dee.ufcg.edu.br> */
  _setmode (_fileno (stdout), _O_BINARY);
  
  // remove the LOCK_TO_SINGLE_CORE micro, user need to enable it with manual
  // LockToSingleCore();
#endif
  
  /* Control-C handler */
  signal (SIGINT, SigIntHandler);
  
  this->serverConnected = false;
  this->augments = std::string(argv[1]);
  this->waitSTTCommand = true;
  this->InitializationDone = false;
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
  this->totalCompressedDataSize = 0;
}

int VideoStreamIGTLinkServer::StartTCPServer ()
{
  if (this->InitializationDone)
  {
    //----------------------------
    
    if(this->transportMethod==VideoStreamIGTLinkServer::UseTCP)
    {
      threader->SpawnThread((igtl::ThreadFunctionType)&ThreadFunctionServer, this);
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
  if (this->InitializationDone)
  {
    
    if (this->transportMethod == VideoStreamIGTLinkServer::UseUDP)
    {
      int r = -1;
      if (this->serverUDPSocket.IsNotNull())
      {
        this->serverUDPSocket->CloseSocket();
      }
      r = this->serverUDPSocket->CreateUDPServer(this->serverPortNumber);
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
  int iRet = 1;
  int arttributNum = 0;
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
          iRet = 1;
          arttributNum ++;
        }
        else
        {
          iRet = 0;
        }
      }
      if (strTag[0].compare ("ClientIPAddress") == 0) {
        this->clientIPAddress = new char[IP4AddressStrLen];
        memcpy(this->clientIPAddress, strTag[1].c_str(), IP4AddressStrLen);
        if(inet_addr(this->clientIPAddress))
        {
          iRet = 0;
        }
        else
        {
          fprintf (stderr, "Invalid parameter for IP address");
          iRet = 1;
          arttributNum ++;
        }
      }
      if (strTag[0].compare ("ClientPortNumber") == 0) {
        this->clientPortNumber = atoi (strTag[1].c_str());
        if(this->clientPortNumber<0 || this->clientPortNumber>65535)
        {
          fprintf (stderr, "Invalid parameter for server port number should between 0 and 65525.");
          iRet = 1;
          arttributNum ++;
        }
        else
        {
          iRet = 0;
        }
        
      }
      if (strTag[0].compare ("DeviceName") == 0)
      {
        this->deviceName =strTag[1].c_str();
        arttributNum ++;
      }
      if (strTag[0].compare ("TransportMethod") == 0)
      {
        this->transportMethod = atoi(strTag[1].c_str());
        arttributNum ++;
      }
      if (strTag[0].compare ("UseCompress") == 0)
      {
        this->useCompress = atoi(strTag[1].c_str());
        arttributNum ++;
      }
      if (strTag[0].compare ("NetWorkBandWidth") == 0)
      {
        this->netWorkBandWidth = atoi(strTag[1].c_str());
        int netWorkBandWidthInBPS = netWorkBandWidth * 1000; //networkBandwidth is in kbps
        int time = floor(8*RTP_PAYLOAD_LENGTH*1e9/netWorkBandWidthInBPS+ 1.0); // the needed time in nanosecond to send a RTP payload.
        this->rtpWrapper->packetIntervalTime = time; // in nanoSecond
        arttributNum ++;
      }
    }
    if (arttributNum ==20) // only parse the first 20 arttribute
    {
      break;
    }
  }
  if (this->transportMethod == 1 )
  {
    if (this->clientIPAddress && this->clientPortNumber)
    {
      this->serverUDPSocket->AddClient(this->clientIPAddress, this->clientPortNumber, 0);
    }
    else
    {
      iRet = 1;
    }
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
        parentObj->InitializationDone = false;
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
            parentObj->InitializationDone = false;
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
            
            igtl::StartVideoDataMessage::Pointer startVideoMsg;
            startVideoMsg = igtl::StartVideoDataMessage::New();
            startVideoMsg->SetMessageHeader(headerMsg);
            startVideoMsg->AllocatePack();
            parentObj->glock->Lock();
            parentObj->socket->Receive(startVideoMsg->GetPackBodyPointer(), startVideoMsg->GetPackBodySize());
            parentObj->glock->Unlock();
            int c = startVideoMsg->Unpack(1);
            if (c & igtl::MessageHeader::UNPACK_BODY && strcmp(startVideoMsg->GetCodecType().c_str(), "H264")) // if CRC check is OK
            {
              parentObj->interval = startVideoMsg->GetTimeInterval();
              parentObj->useCompress = startVideoMsg->GetUseCompress();
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


bool VideoStreamIGTLinkServer::CompareHash (const unsigned char* digest, const char* hashStr) {
  char hashStrCmp[SHA_DIGEST_LENGTH * 2 + 1];
  for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
    sprintf (&hashStrCmp[i * 2], "%.2x", digest[i]);
  }
  hashStrCmp[SHA_DIGEST_LENGTH * 2] = '\0';
  if (hashStr == hashStrCmp)
  {
    return true;
  }
  return false;
}

bool VideoStreamIGTLinkServer::InitializeEncoderAndServer()
{
  //------------------------------------------------------------
  int iRet = 0;
  iRet = CreateSVCEncHandle(&(this->pSVCEncoder));
  if (this->pSVCEncoder == NULL)
    this->serverConnected = false;
  
  this->iTotalFrameToEncode = 0;
  
  // Preparing encoding process
  
  // Inactive with sink with output file handler
  FILE* pFpBs = NULL;
#if defined(COMPARE_DATA)
  //For getting the golden file handle
  FILE* fpGolden = NULL;
#endif
#if defined ( STICK_STREAM_SIZE )
  FILE* fTrackStream = fopen ("coding_size.stream", "wb");
#endif
  this->pSVCEncoder->GetDefaultParams (&sSvcParam);
  memset (&fs.sRecFileName[0][0], 0, sizeof (fs.sRecFileName));
  
  FillSpecificParameters (sSvcParam);
  if (pSrcPic == NULL) {
    iRet = 1;
    goto INSIDE_MEM_FREE;
  }
  //fill default pSrcPic
  pSrcPic->iColorFormat = videoFormatI420;
  pSrcPic->uiTimeStamp = 0;
  
  // if configure file exit, reading configure file firstly
  cRdCfg.Openf (this->augments.c_str());// to do get the first augments from this->augments.
  if (cRdCfg.ExistFile())
  {
    iRet = ParseConfig (cRdCfg, pSrcPic, sSvcParam, fs);
    if (iRet) {
      fprintf (stderr, "parse svc parameter config file failed.\n");
      iRet = 1;
      goto INSIDE_MEM_FREE;
    }
    cRdCfg.Openf (this->augments.c_str());// reset the file read pointer to the beginning.
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
  this->iTotalFrameToEncode = (igtl_int32)fs.uiFrameToBeCoded;
  this->pSVCEncoder->SetOption (ENCODER_OPTION_TRACE_LEVEL, &g_LevelSetting);
  //finish reading the configurations
  igtl_uint32 iSourceWidth, iSourceHeight, kiPicResSize;
  iSourceWidth = pSrcPic->iPicWidth;
  iSourceHeight = pSrcPic->iPicHeight;
  kiPicResSize = iSourceWidth * iSourceHeight * 3 >> 1;
  
  //update pSrcPic
  pSrcPic->iStride[0] = iSourceWidth;
  pSrcPic->iStride[1] = pSrcPic->iStride[2] = pSrcPic->iStride[0] >> 1;
  
  //update sSvcParam
  sSvcParam.iPicWidth = 0;
  sSvcParam.iPicHeight = 0;
  for (int iLayer = 0; iLayer < sSvcParam.iSpatialLayerNum; iLayer++) {
    SSpatialLayerConfig* pDLayer = &sSvcParam.sSpatialLayers[iLayer];
    sSvcParam.iPicWidth = WELS_MAX (sSvcParam.iPicWidth, pDLayer->iVideoWidth);
    sSvcParam.iPicHeight = WELS_MAX (sSvcParam.iPicHeight, pDLayer->iVideoHeight);
  }
  //if target output resolution is not set, use the source size
  sSvcParam.iPicWidth = (!sSvcParam.iPicWidth) ? iSourceWidth : sSvcParam.iPicWidth;
  sSvcParam.iPicHeight = (!sSvcParam.iPicHeight) ? iSourceHeight : sSvcParam.iPicHeight;
  
  //  sSvcParam.bSimulcastAVC = true;
  if (cmResultSuccess != this->pSVCEncoder->InitializeExt (&sSvcParam)) { // SVC encoder initialization
    fprintf (stderr, "SVC encoder Initialize failed\n");
    iRet = 1;
    goto INSIDE_MEM_FREE;
  }
  for (int iLayer = 0; iLayer < MAX_DEPENDENCY_LAYER; iLayer++) {
    if (fs.sRecFileName[iLayer][0] != 0) {
      SDumpLayer sDumpLayer;
      sDumpLayer.iLayer = iLayer;
      sDumpLayer.pFileName = fs.sRecFileName[iLayer];
      if (cmResultSuccess != this->pSVCEncoder->SetOption (ENCODER_OPTION_DUMP_FILE, &sDumpLayer)) {
        fprintf (stderr, "SetOption ENCODER_OPTION_DUMP_FILE failed!\n");
        iRet = 1;
        goto INSIDE_MEM_FREE;
      }
    }
  }
  // Inactive with sink with output file handler
  if (fs.strBsFile.length() > 0) {
    pFpBs = fopen (fs.strBsFile.c_str(), "wb");
    if (pFpBs == NULL) {
      fprintf (stderr, "Can not open file (%s) to write bitstream!\n", fs.strBsFile.c_str());
      iRet = 1;
      goto INSIDE_MEM_FREE;
    }
  }
  
#if defined(COMPARE_DATA)
  //For getting the golden file handle
  if ((fpGolden = fopen (argv[3], "rb")) == NULL) {
    fprintf (stderr, "Unable to open golden sequence file, check corresponding path!\n");
    iRet = 1;
    goto INSIDE_MEM_FREE;
  }
#endif
  this->InitializationDone = true;
  return true;
INSIDE_MEM_FREE:
  if (pFpBs) {
    fclose (pFpBs);
    pFpBs = NULL;
  }
#if defined (STICK_STREAM_SIZE)
  if (fTrackStream) {
    fclose (fTrackStream);
    fTrackStream = NULL;
  }
#endif
#if defined (COMPARE_DATA)
  if (fpGolden) {
    fclose (fpGolden);
    fpGolden = NULL;
  }
#endif
  if (pSrcPic) {
    delete pSrcPic;
    pSrcPic = NULL;
  }
  this->serverConnected = false;
  this->InitializationDone = false;
  return false;
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
  int kiPicResSize = parentObj.server->pSrcPic->iPicWidth*parentObj.server->pSrcPic->iPicHeight*3>>1;
  igtl_int32 iActualFrameEncodedCount = 0;
  igtl_int32 iFrameNumInFile = -1;
  FILE* pFileYUV = NULL;
  pFileYUV = fopen (parentObj.server->fs.strSeqFile.c_str(), "rb");
  if (pFileYUV != NULL) {
#if defined(_WIN32) || defined(_WIN64)
#if _MSC_VER >= 1400
    if (!_fseeki64 (pFileYUV, 0, SEEK_END)) {
      igtl_int64 i_size = _ftelli64 (pFileYUV);
      _fseeki64 (pFileYUV, 0, SEEK_SET);
      iFrameNumInFile = WELS_MAX ((igtl_int32) (i_size / kiPicResSize), iFrameNumInFile);
    }
#else
    if (!fseek (pFileYUV, 0, SEEK_END)) {
      igtl_int64 i_size = ftell (pFileYUV);
      fseek (pFileYUV, 0, SEEK_SET);
      iFrameNumInFile = WELS_MAX ((igtl_int32) (i_size / kiPicResSize), iFrameNumInFile);
    }
#endif
#else
    if (!fseeko (pFileYUV, 0, SEEK_END)) {
      igtl_int64 i_size = ftello (pFileYUV);
      fseeko (pFileYUV, 0, SEEK_SET);
      iFrameNumInFile = WELS_MAX ((igtl_int32) (i_size / kiPicResSize), iFrameNumInFile);
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
        
#ifdef ONLY_ENC_FRAMES_NUM
        // Only encoded some limited frames here
        if (iActualFrameEncodedCount >= ONLY_ENC_FRAMES_NUM) {
          break;
        }
#endif//ONLY_ENC_FRAMES_NUM
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
             parentObj.server->fs.strSeqFile.c_str());
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
      if (parentObj.server->transportMethod == VideoStreamIGTLinkServer::TransportMethod::UseUDP)
      {
        parentObj.server->rtpWrapper->WrapMessageAndSend(parentObj.server->serverUDPSocket, frameCopy->messagePackPointer, frameCopy->messageDataLength);
      }
      else if(parentObj.server->transportMethod == VideoStreamIGTLinkServer::TransportMethod::UseTCP)
      {
        if(parentObj.server->socket)
        {
         int success =  parentObj.server->socket->Send(frameCopy->messagePackPointer, frameCopy->messageDataLength);
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
  int threadID = threader->SpawnThread((igtl::ThreadFunctionType)&ThreadFunctionReadFrameFromFile, &ptr);
  igtl::Sleep(10);
  return threadID;
}

int VideoStreamIGTLinkServer::StartSendPacketThread()
{
  serverPointer ptr;
  ptr.server = this;
  int threadID = threader->SpawnThread((igtl::ThreadFunctionType)&ThreadFunctionSendPacket, &ptr);
  igtl::Sleep(10);
  return threadID;
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
  if(this->useCompress == 0)
  {
    int kiPicResSize = pSrcPic->iPicWidth*pSrcPic->iPicHeight*3>>1;
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
        videoMsg->AllocateBuffer();
        videoMsg->SetScalarType(videoMsg->TYPE_UINT8);
        videoMsg->SetEndian(igtl_is_little_endian()==true?2:1); //little endian is 2 big endian is 1
        videoMsg->SetWidth(pSrcPic->iPicWidth);
        videoMsg->SetHeight(pSrcPic->iPicHeight);
        videoMsg->SetMessageID(messageID);
        memcpy(videoMsg->m_Frame, pYUV, kiPicResSize);
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
}

void VideoStreamIGTLinkServer::SendCompressedData()
{
  if (sFbi.iFrameSizeInBytes > 0)
  {
    igtl::VideoMessage::Pointer videoMsg;
    videoMsg = igtl::VideoMessage::New();
    videoMsg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
    videoMsg->SetDeviceName(this->deviceName.c_str());
    ServerTimer->SetTime(this->encodeEndTime);
    videoMsg->SetTimeStamp(ServerTimer);
    static igtl_uint32 messageID = -1;
    int frameSize = 0;
    int iLayer = 0;
    videoMsg->SetBitStreamSize(sFbi.iFrameSizeInBytes);
    videoMsg->AllocateBuffer();
    videoMsg->SetScalarType(videoMsg->TYPE_UINT8);
    videoMsg->SetEndian(igtl_is_little_endian()==true?2:1); //little endian is 2 big endian is 1
    videoMsg->SetWidth(pSrcPic->iPicWidth);
    videoMsg->SetHeight(pSrcPic->iPicHeight);
    videoMsg->SetFrameType(this->videoFrameType);
    messageID ++;
    videoMsg->SetMessageID(messageID);
    while (iLayer < sFbi.iLayerNum) {
      SLayerBSInfo* pLayerBsInfo = &sFbi.sLayerInfo[iLayer];
      if (pLayerBsInfo != NULL) {
        int iLayerSize = 0;
        int iNalIdx = pLayerBsInfo->iNalCount - 1;
        do {
          iLayerSize += pLayerBsInfo->pNalLengthInByte[iNalIdx];
          -- iNalIdx;
        } while (iNalIdx >= 0);
        frameSize += iLayerSize;
        for (int i = 0; i < iLayerSize ; i++)
        {
          videoMsg->GetPackFragmentPointer(2)[frameSize-iLayerSize+i] = pLayerBsInfo->pBsBuf[i];
        }
      }
      ++ iLayer;
    }
    this->totalCompressedDataSize += frameSize;
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

void* VideoStreamIGTLinkServer::EncodeFile(void)
{
  if(!this->InitializationDone)
  {
    this->InitializeEncoderAndServer();
  }
  igtl_int64 iStart = 0, iTotal = 0;
  
  int kiPicResSize = pSrcPic->iPicWidth*pSrcPic->iPicHeight*3>>1;
  
  igtl_int32 iFrameIdx = 0;
  igtl_uint8* pYUV = new igtl_uint8[kiPicResSize];
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
      memcpy(pYUV,it->second,kiPicResSize);
      delete it->second;
      it->second = NULL;
      this->incommingFrames.erase(it);
      frameNum = this->incommingFrames.size();
      this->glockInFrame->Unlock();
      iStart = WelsTime();
      this->pSrcPic->uiTimeStamp = WELS_ROUND (iFrameIdx * (1000 / sSvcParam.fMaxFrameRate));
      this->ServerTimer->GetTime();
      this->encodeStartTime = this->ServerTimer->GetTimeStampInNanoseconds();
      int iEncFrames = this->EncodeSingleFrame(pYUV);
      this->ServerTimer->GetTime();
      this->encodeEndTime = this->ServerTimer->GetTimeStampInNanoseconds();
      iTotal += WelsTime()- iStart;
      ++ iFrameIdx;
      if (sFbi.eFrameType == videoFrameTypeSkip) {
        continue;
      }
      
      if (iEncFrames == cmResultSuccess ) {
        SendCompressedData();
        igtl::Sleep(1);
  #if defined (STICK_STREAM_SIZE)
        if (fTrackStream) {
          fwrite (&iFrameSize, 1, sizeof (int), fTrackStream);
        }
  #endif//STICK_STREAM_SIZE
        iActualFrameEncodedCount ++;
      } else {
        fprintf (stderr, "EncodeFrame(), ret: %d, frame index: %d.\n", iEncFrames, iFrameIdx);
      }
      if (iActualFrameEncodedCount%10 == 0) {
        double dElapsed = iTotal / 1e6;
        printf ("Width:\t\t%d\nHeight:\t\t%d\nFrames:\t\t%d\nencode time:\t%f sec\nFPS:\t\t%f fps\nCompressionRate:\t\t%f\n",
                sSvcParam.iPicWidth, sSvcParam.iPicHeight,
                iActualFrameEncodedCount, dElapsed, (iActualFrameEncodedCount * 1.0) / dElapsed, totalCompressedDataSize/(iActualFrameEncodedCount*3/2.0*sSvcParam.iPicWidth*sSvcParam.iPicHeight));
        
  #if defined (WINDOWS_PHONE)
        g_fFPS = (iActualFrameEncodedCount * 1.0f) / (float) dElapsed;
        g_dEncoderTime = dElapsed;
        g_iEncodedFrame = iActualFrameEncodedCount;
  #endif
      }
    }
  }
  delete[] pYUV;
  pYUV = NULL;
  return NULL;
}



int VideoStreamIGTLinkServer::EncodeSingleFrame(igtl_uint8* picPointer)
{
  int encodeRet = -1;
  if (this->InitializationDone == true)
  {
    int iSourceWidth = pSrcPic->iPicWidth;
    int iSourceHeight = pSrcPic->iPicHeight;
    pSrcPic->pData[0] = picPointer;
    pSrcPic->pData[1] = pSrcPic->pData[0] + (iSourceWidth * iSourceHeight);
    pSrcPic->pData[2] = pSrcPic->pData[1] + (iSourceWidth * iSourceHeight >> 2);
    encodeRet = pSVCEncoder->EncodeFrame(pSrcPic, &sFbi);
    this->videoFrameType = sFbi.eFrameType;
  }
  return encodeRet;
}

