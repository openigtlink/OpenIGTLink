/*=========================================================================
 
 Program:   VideoStreamIGTLinkReceiver 
 Modified based on the OpenH264/codec/console/dec/src/h264dec.cpp
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "VideoStreamIGTLinkReceiver.h"

struct ReadSocketAndPush
{
  igtl::MessageRTPWrapper::Pointer wrapper;
  igtl::UDPClientSocket::Pointer clientSocket;
  VideoStreamIGTLinkReceiver* receiver;
};

struct Wrapper
{
  igtl::MessageRTPWrapper::Pointer wrapper;
  VideoStreamIGTLinkReceiver* receiver;
};


void* ThreadFunctionUnWrap(void* ptr)
{
  // Get thread information
  igtl::MultiThreader::ThreadInfo* info =
  static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);
  const char *deviceType = "Video";
  Wrapper parentObj = *(static_cast<Wrapper*>(info->UserData));
  const char * videoDeviceName = parentObj.receiver->deviceName.c_str();
  while(1)
  {
    parentObj.wrapper->UnWrapPacketWithTypeAndName(deviceType, videoDeviceName);
  }
}

void WriteTimeInfo(unsigned char * UDPPacket, int totMsgLen, VideoStreamIGTLinkReceiver* receiver)
{
  igtl_uint16 fragmentField;
  igtl_uint32 messageID;
  int extendedHeaderLength = sizeof(igtl_extended_header);
  memcpy(&fragmentField, (void*)(UDPPacket + RTP_HEADER_LENGTH+IGTL_HEADER_SIZE+extendedHeaderLength-2),2);
  memcpy(&messageID, (void*)(UDPPacket + RTP_HEADER_LENGTH+IGTL_HEADER_SIZE+extendedHeaderLength-6),4);
  if(igtl_is_little_endian())
  {
    fragmentField = BYTE_SWAP_INT16(fragmentField);
    messageID = BYTE_SWAP_INT32(messageID);
  }
  
}


void* ThreadFunctionReadSocket(void* ptr)
{
  // Get thread information
  igtl::MultiThreader::ThreadInfo* info =
  static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);
  
  ReadSocketAndPush parentObj = *(static_cast<ReadSocketAndPush*>(info->UserData));
  unsigned char UDPPacket[RTP_PAYLOAD_LENGTH+RTP_HEADER_LENGTH];
  while(1)
  {
    int totMsgLen = parentObj.clientSocket->ReadSocket(UDPPacket, RTP_PAYLOAD_LENGTH+RTP_HEADER_LENGTH);
    
    WriteTimeInfo(UDPPacket, totMsgLen, parentObj.receiver);
    if (totMsgLen>0)
    {
      parentObj.wrapper->PushDataIntoPacketBuffer(UDPPacket, totMsgLen);
    }
  }
}

VideoStreamIGTLinkReceiver::VideoStreamIGTLinkReceiver(char *argv[])
{
  this->deviceName = "";
  this->augments     = argv[1];
  this->rtpWrapper = igtl::MessageRTPWrapper::New();
  strncpy(this->codecType, "H264",4);
  this->useCompress      = true;
  
  this->interval = 10; //10ms
  
  WelsCreateDecoder (&this->pSVCDecoder);
  memset (&this->decParam, 0, sizeof (SDecodingParam));
  this->decParam.uiTargetDqLayer = UCHAR_MAX;
  this->decParam.eEcActiveIdc = ERROR_CON_SLICE_COPY;
  this->decParam.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_DEFAULT;
  this->pSVCDecoder->Initialize (&decParam);
  this->kpOuputFileName = (char*)"decodedOutput.yuv";
  this->videoMessageBuffer=NULL;
  this->decodedNal=NULL;
  socket = igtl::ClientSocket::New();
  UDPSocket = igtl::UDPClientSocket::New();
  this->Height = 0;
  this->Width = 0;
  this->flipAtX = true;
  this->H264DecodeInstance = new H264Decode();
}

int VideoStreamIGTLinkReceiver::RunOnTCPSocket()
{
  //------------------------------------------------------------
  // Establish Connection
  int r = socket->ConnectToServer(TCPServerIPAddress, TCPServerPort);
  
  if (r != 0)
  {
    std::cerr << "Cannot connect to the server." << std::endl;
    exit(0);
  }
  
  //------------------------------------------------------------
  // Ask the server to start pushing tracking data
  std::cerr << "Sending STT_VIDEO message....." << std::endl;
  igtl::StartVideoDataMessage::Pointer startVideoMsg;
  startVideoMsg = igtl::StartVideoDataMessage::New();
  startVideoMsg->AllocatePack();
  startVideoMsg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
  startVideoMsg->SetDeviceName("MacCamera5");
  startVideoMsg->SetCodecType(codecType);
  startVideoMsg->SetTimeInterval(interval);
  startVideoMsg->SetUseCompress(useCompress);
  startVideoMsg->Pack();
  socket->Send(startVideoMsg->GetPackPointer(), startVideoMsg->GetPackSize());
  while (1)
  {
    //------------------------------------------------------------
    // Wait for a reply
    igtl::MessageHeader::Pointer headerMsg;
    headerMsg = igtl::MessageHeader::New();
    headerMsg->InitPack();
    int rs = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
    if (rs == 0)
    {
      std::cerr << "Connection closed." << std::endl;
      socket->CloseSocket();
      exit(0);
    }
    if (rs != headerMsg->GetPackSize())
    {
      std::cerr << rs <<"Message size information and actual data size don't match." <<headerMsg->GetPackSize()<< std::endl;
      continue;
    }
    
    headerMsg->Unpack();
    if (strcmp(headerMsg->GetDeviceName(), this->deviceName.c_str()) == 0)
    {
      //------------------------------------------------------------
      // Allocate Video Message Class
      
      igtl::VideoMessage::Pointer videoMsg;
      videoMsg = igtl::VideoMessage::New();
      videoMsg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
      videoMsg->SetMessageHeader(headerMsg);
      videoMsg->AllocateBuffer();
      // Receive body from the socket
      socket->Receive(videoMsg->GetPackBodyPointer(), videoMsg->GetPackBodySize());
      // Deserialize the transform data
      // If you want to skip CRC check, call Unpack() without argument.
      int c = videoMsg->Unpack(1);
      
      if (c==0) // if CRC check fails
      {
        // TODO: error handling
        return 0;
      }
      this->SetWidth(videoMsg->GetWidth());
      this->SetHeight(videoMsg->GetHeight());
      int streamLength = videoMsg->GetPackSize()-IGTL_VIDEO_HEADER_SIZE-IGTL_HEADER_SIZE;
      this->SetStreamLength(streamLength);
      if (!(this->videoMessageBuffer==NULL))
      {
        delete[] this->videoMessageBuffer;
      }
      this->videoMessageBuffer = new igtl_uint8[streamLength];
      memcpy(this->videoMessageBuffer, videoMsg->GetPackFragmentPointer(2), streamLength);
      static int frameNum = 0;
      int status = this->ProcessVideoStream(this->videoMessageBuffer);
      
      if (status == 0)
      {
        this->SendStopMessage();
        break;
      }
      else if(status == 2)
      {
        frameNum ++;
      }
    }
    else
    {
      std::cerr << "Receiving : " << headerMsg->GetDeviceType() << std::endl;
      socket->Skip(headerMsg->GetBodySizeToRead(), 0);
    }
    igtl::Sleep(20);
  }
  WelsDestroyDecoder(this->pSVCDecoder);
  return 1;
}

void VideoStreamIGTLinkReceiver::SendStopMessage()
{
  //------------------------------------------------------------
  // Ask the server to stop pushing tracking data
  std::cerr << "Sending STP_VIDEO message....." << std::endl;
  igtl::StopVideoMessage::Pointer stopVideoMsg;
  stopVideoMsg = igtl::StopVideoMessage::New();
  stopVideoMsg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
  stopVideoMsg->SetDeviceName("VIDEO");
  stopVideoMsg->Pack();
  socket->Send(stopVideoMsg->GetPackPointer(), stopVideoMsg->GetPackSize());
}

int VideoStreamIGTLinkReceiver::RunOnUDPSocket()
{
  igtl::ConditionVariable::Pointer conditionVar = igtl::ConditionVariable::New();
  igtl::SimpleMutexLock* glock = igtl::SimpleMutexLock::New();
  UDPSocket->JoinNetwork("127.0.0.1", UDPClientPort, 0);
  ReadSocketAndPush info;
  info.wrapper = rtpWrapper;
  info.clientSocket = UDPSocket;
  info.receiver = this;
  
  Wrapper infoWrapper;
  infoWrapper.wrapper = rtpWrapper;
  infoWrapper.receiver = this;
  
  igtl::MultiThreader::Pointer threader = igtl::MultiThreader::New();
  threader->SpawnThread((igtl::ThreadFunctionType)&ThreadFunctionReadSocket, &info);
  threader->SpawnThread((igtl::ThreadFunctionType)&ThreadFunctionUnWrap, &infoWrapper);
  while(1)
  {
    glock->Lock();
    unsigned int messageNum = rtpWrapper->unWrappedMessages.size();
    glock->Unlock();
    if(messageNum)// to do: glock this session
    {
      igtl::VideoMessage::Pointer videoMultiPKTMSG = igtl::VideoMessage::New();
      glock->Lock();
      std::map<igtl_uint32, igtl::UnWrappedMessage*>::iterator it = rtpWrapper->unWrappedMessages.begin();
      igtlUint8 * message = new igtlUint8[it->second->messageDataLength];
      static int frameNum = 0;
      int MSGLength = it->second->messageDataLength;
      memcpy(message, it->second->messagePackPointer, it->second->messageDataLength);
      delete it->second;
      it->second = NULL;
      rtpWrapper->unWrappedMessages.erase(it);
      glock->Unlock();
      igtl::MessageHeader::Pointer header = igtl::MessageHeader::New();
      header->InitPack();
      memcpy(header->GetPackPointer(), message, IGTL_HEADER_SIZE);
      header->Unpack();
      videoMultiPKTMSG->SetMessageHeader(header);
      videoMultiPKTMSG->AllocateBuffer();
      if (MSGLength == videoMultiPKTMSG->GetPackSize())
      {
        memcpy(videoMultiPKTMSG->GetPackPointer(), message, MSGLength);
        videoMultiPKTMSG->Unpack(0);
        this->SetWidth(videoMultiPKTMSG->GetWidth());
        this->SetHeight(videoMultiPKTMSG->GetHeight());
        int streamLength = videoMultiPKTMSG->GetBitStreamSize();
        this->SetStreamLength(streamLength);
        if (!(this->videoMessageBuffer==NULL))
        {
          delete[] this->videoMessageBuffer;
        }
        this->videoMessageBuffer = new igtl_uint8[streamLength];
        memcpy(this->videoMessageBuffer, videoMultiPKTMSG->GetPackFragmentPointer(2), streamLength);
        
        int status = this->ProcessVideoStream(this->videoMessageBuffer);
        
        if (status == 0)
        {
          break;
        }
        else if(status == 2)
        {
          frameNum ++;
        }
      }
      delete [] message;
    }
  }
  return 0;
}

bool VideoStreamIGTLinkReceiver::InitializeClient()
{
  // if configure file exit, reading configure file firstly
  if(strcmp(this->augments.c_str(),"")==0)
  {
    fprintf (stdout, "The udp/tcp selection is upto the application.\n");
    return true;
  }
  else
  {
    cRdCfg.Openf (this->augments.c_str());// to do get the first augments from this->augments.
    if (cRdCfg.ExistFile())
    {
      cRdCfg.Openf (this->augments.c_str());// reset the file read pointer to the beginning.
      int iRet = ParseConfigForClient();
      if (iRet == -1) {
        fprintf (stderr, "parse client parameter config file failed.\n");
        return false;
      }
      return true;
    }
    else
    {
      fprintf (stderr, "Specified file: %s not exist, maybe invalid path or parameter settting.\n",
               cRdCfg.GetFileName().c_str());
      return false;
    }
  }
}

int VideoStreamIGTLinkReceiver::ParseConfigForClient()
{
  std::string strTag[4];
  while (!cRdCfg.EndOfFile()) {
    strTag->clear();
    long iRd = cRdCfg.ReadLine (&strTag[0]);
    if (iRd > 0) {
      if (strTag[0].empty())
        continue;
      if (strTag[0].compare ("TCPServerPortNumber") == 0) {
        this->TCPServerPort = atoi (strTag[1].c_str());
        if(this->TCPServerPort<0 || this->TCPServerPort>65535)
        {
          fprintf (stderr, "Invalid parameter for server port number should between 0 and 65525.");
          return 1;
        }
      }
      if (strTag[0].compare ("TCPServerIPAddress") == 0) {
        this->TCPServerIPAddress = new char[IP4AddressStrLen];
        memcpy(this->TCPServerIPAddress, strTag[1].c_str(), IP4AddressStrLen);
        if(!inet_addr(this->TCPServerIPAddress))
        {
          fprintf (stderr, "Invalid parameter for IP address");
          return 1;
        }
      }
      if (strTag[0].compare ("UDPClientPortNumber") == 0) {
        this->UDPClientPort = atoi (strTag[1].c_str());
        if(this->UDPClientPort<0 || this->UDPClientPort>65535)
        {
          fprintf (stderr, "Invalid parameter for server port number should between 0 and 65525.");
          return 1;
        }
      }
      if (strTag[0].compare ("DeviceName") == 0)
      {
        this->deviceName =strTag[1].c_str();
      }
      if (strTag[0].compare ("TransportMethod") == 0)
      {
        this->transportMethod = atoi(strTag[1].c_str());
      }
      if (strTag[0].compare ("UseCompress") == 0)
      {
        this->useCompress = atoi(strTag[1].c_str());
      }
      
      if (strTag[0].compare ("DecodedFile") == 0)
      {
        this->kpOuputFileName = std::string(strTag[1].c_str());
      }
    }
  }
  return 0;
}

void VideoStreamIGTLinkReceiver::SetWidth(int iWidth)
{
  this->Width = iWidth;
}

void VideoStreamIGTLinkReceiver::SetHeight(int iHeight)
{
  this->Height = iHeight;
}

void VideoStreamIGTLinkReceiver::SetStreamLength(int iStreamLength)
{
  this->StreamLength = iStreamLength;
}

void VideoStreamIGTLinkReceiver::SetDecodedNal()
{
  if (!(this->decodedNal == NULL))
  {
    delete[] this->decodedNal;
  }
  this->decodedNal = NULL;
  this->decodedNal = new unsigned char[this->Width*this->Height*3>>1];
}

int VideoStreamIGTLinkReceiver::ProcessVideoStream(igtl_uint8* bitStream)
{
  //std::cerr << "Receiving Video data type." << std::endl;
  //this->videoMessageBuffer = new igtl_uint8[StreamLength];
  //memcpy(this->videoMessageBuffer, bitStream, StreamLength);// copy slow down the process, however, the videoMsg is a smart pointer, it gets deleted unpredictable.
  
  if (useCompress)
  {
    this->SetDecodedNal();
    int status = H264DecodeInstance->DecodeSingleNal(this->pSVCDecoder, bitStream, this->decodedNal, kpOuputFileName.c_str(), Width, Height, StreamLength);
    // return 2 for succefully decode one frame
    return status;
  }
  else
  {
    //std::cerr << "No using compression, data size in byte is: " << Width*Height*3/2  <<std::endl;
    FILE* pYuvFile    = NULL;
    pYuvFile = fopen (kpOuputFileName.c_str(), "ab");
    unsigned char* pData[3];
    int iStride[2] = {Width, Width/2};
    pData[0] = bitStream;
    pData[1] = pData[0] + Width * Height;
    pData[2] = pData[1] + Width * Height/4;
    H264DecodeInstance->Write2File (pYuvFile, pData, iStride, Width, Height);
    fclose (pYuvFile);
    pYuvFile = NULL;
    return 2;
  }
  return 0;
}


int VideoStreamIGTLinkReceiver::YUV420ToRGBConversion(igtl_uint8 *RGBFrame, igtl_uint8 * YUV420Frame, int iHeight, int iWidth)
{
  int componentLength = iHeight*iWidth;
  const igtl_uint8 *srcY = YUV420Frame;
  const igtl_uint8 *srcU = YUV420Frame+componentLength;
  const igtl_uint8 *srcV = YUV420Frame+componentLength*5/4;
  igtl_uint8 * YUV444 = new igtl_uint8[componentLength * 3];
  igtl_uint8 *dstY = YUV444;
  igtl_uint8 *dstU = dstY + componentLength;
  igtl_uint8 *dstV = dstU + componentLength;
  
  memcpy(dstY, srcY, componentLength);
  const int halfHeight = iHeight/2;
  const int halfWidth = iWidth/2;
  
#pragma omp parallel for default(none) shared(dstV,dstU,srcV,srcU,iWidth)
  for (int y = 0; y < halfHeight; y++) {
    for (int x = 0; x < halfWidth; x++) {
      dstU[2 * x + 2 * y*iWidth] = dstU[2 * x + 1 + 2 * y*iWidth] = srcU[x + y*iWidth/2];
      dstV[2 * x + 2 * y*iWidth] = dstV[2 * x + 1 + 2 * y*iWidth] = srcV[x + y*iWidth/2];
    }
    memcpy(&dstU[(2 * y + 1)*iWidth], &dstU[(2 * y)*iWidth], iWidth);
    memcpy(&dstV[(2 * y + 1)*iWidth], &dstV[(2 * y)*iWidth], iWidth);
  }
  
  
  const int yOffset = 16;
  const int cZero = 128;
  int yMult, rvMult, guMult, gvMult, buMult;
  yMult =   76309;
  rvMult = 117489;
  guMult = -13975;
  gvMult = -34925;
  buMult = 138438;
  
  static unsigned char clp_buf[384+256+384];
  static unsigned char *clip_buf = clp_buf+384;
  
  // initialize clipping table
  memset(clp_buf, 0, 384);
  
  for (int i = 0; i < 256; i++) {
    clp_buf[384+i] = i;
  }
  memset(clp_buf+384+256, 255, 384);
  
  
#pragma omp parallel for default(none) shared(dstY,dstU,dstV,RGBFrame,yMult,rvMult,guMult,gvMult,buMult,clip_buf,componentLength)// num_threads(2)
  for (int i = 0; i < componentLength; ++i) {
    const int Y_tmp = ((int)dstY[i] - yOffset) * yMult;
    const int U_tmp = (int)dstU[i] - cZero;
    const int V_tmp = (int)dstV[i] - cZero;
    
    const int R_tmp = (Y_tmp                  + V_tmp * rvMult ) >> 16;//32 to 16 bit conversion by left shifting
    const int G_tmp = (Y_tmp + U_tmp * guMult + V_tmp * gvMult ) >> 16;
    const int B_tmp = (Y_tmp + U_tmp * buMult                  ) >> 16;
    
    if (flipAtX)
    {
      int pos = componentLength-i-Width+(i%Width)*2;
      RGBFrame[3*pos] = clip_buf[R_tmp];
      RGBFrame[3*pos+1] = clip_buf[G_tmp];
      RGBFrame[3*pos+2] = clip_buf[B_tmp];
    }
    else
    {
      RGBFrame[3*i]   = clip_buf[R_tmp];
      RGBFrame[3*i+1] = clip_buf[G_tmp];
      RGBFrame[3*i+2] = clip_buf[B_tmp];
    }
  }
  delete [] YUV444;
  YUV444 = NULL;
  dstY = NULL;
  dstU = NULL;
  dstV = NULL;
  return 1;
}


int VideoStreamIGTLinkReceiver::YUV420ToGrayImageConversion(igtl_uint8 *GrayFrame, igtl_uint8 * YUV420Frame, int iHeight, int iWidth)
{
  int componentLength = iHeight*iWidth;
  for (int i = 0,j= 0; i < 3*componentLength; i=i+3,j++) {
    if (flipAtX)
    {
      int pos = componentLength-j-Width+(i%Width)*2;
      GrayFrame[i] = YUV420Frame[pos];
      GrayFrame[i+1] = YUV420Frame[pos];
      GrayFrame[i+2] = YUV420Frame[pos];
    }
    else
    {
      GrayFrame[i] = YUV420Frame[j];
      GrayFrame[i+1] = YUV420Frame[j];
      GrayFrame[i+2] = YUV420Frame[j];
    }
    
  }
  return 1;
}

