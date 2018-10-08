/*=========================================================================
 
 Program:   VideoStreamIGTLinkReceiver
 Modified based on the OpenH264/codec/console/dec/src/h264dec.cpp
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlVideoStreamIGTLinkReceiver.h"

namespace igtl {

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
  const char *deviceType = "VIDEO";
  Wrapper parentObj = *(static_cast<Wrapper*>(info->UserData));
  const char * videoDeviceName = parentObj.receiver->GetDeviceName().c_str();
  while(1)
    {
    parentObj.wrapper->UnWrapPacketWithTypeAndName(deviceType, videoDeviceName);
    }
}

void WriteTimeInfo(unsigned char * UDPPacket, int totMsgLen, VideoStreamIGTLinkReceiver* receiver)
{
  igtl_uint16 fragmentField;
  igtl_uint32 messageID;
  int extendedHeaderLength = IGTL_EXTENDED_HEADER_SIZE;
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

VideoStreamIGTLinkReceiver::VideoStreamIGTLinkReceiver(char *fileName)
{
  this->deviceName = "";
  this->configFile     = fileName;
  this->rtpWrapper = igtl::MessageRTPWrapper::New();
  strncpy(this->codecType, "H264",4);
  
  this->interval = 10; //10ms
  this->kpOuputFileName = (char*)"decodedOutput.yuv";
  this->videoMessageBuffer=NULL;
  this->decodedFrame=NULL;
  socket = igtl::ClientSocket::New();
  UDPSocket = igtl::UDPClientSocket::New();
  this->Height = 0;
  this->Width = 0;
  this->decodeInstance = NULL;
}

VideoStreamIGTLinkReceiver::~VideoStreamIGTLinkReceiver()
{}

void VideoStreamIGTLinkReceiver::SetDecoder(GenericDecoder* decoder)
{
  this->decodeInstance = decoder;
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
  igtl::StartVideoMessage::Pointer startVideoMsg;
  startVideoMsg = igtl::StartVideoMessage::New();
  startVideoMsg->AllocatePack();
  startVideoMsg->SetHeaderVersion(IGTL_HEADER_VERSION_2);
  startVideoMsg->SetDeviceName("MacCamera5");
  startVideoMsg->SetCodecType(codecType);
  startVideoMsg->SetTimeInterval(interval);
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
      videoMsg->SetMessageHeader(headerMsg);
      videoMsg->AllocateScalars();
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
      int streamLength = videoMsg->GetBitStreamSize();
      if (!(this->videoMessageBuffer==NULL))
        {
        delete[] this->videoMessageBuffer;
        }
      this->videoMessageBuffer = new igtl_uint8[streamLength];
      memcpy(this->videoMessageBuffer, videoMsg->GetPackFragmentPointer(2), streamLength);
      static int frameNum = 0;
      int status = this->ProcessVideoStream(this->videoMessageBuffer,streamLength);
      
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
  UDPSocket->JoinNetwork("127.0.0.1", UDPClientPort);
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
        int streamLength = videoMultiPKTMSG->GetBitStreamSize();
        this->SetWidth(videoMultiPKTMSG->GetWidth());
        this->SetHeight(videoMultiPKTMSG->GetHeight());
        if (!(this->videoMessageBuffer==NULL))
          {
          delete[] this->videoMessageBuffer;
          }
        this->videoMessageBuffer = new igtl_uint8[streamLength];
        memcpy(this->videoMessageBuffer, videoMultiPKTMSG->GetPackFragmentPointer(2), streamLength);
        
        int status = this->ProcessVideoStream(this->videoMessageBuffer,streamLength);
        
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
  if(strcmp(this->configFile.c_str(),"")==0)
    {
    fprintf (stdout, "The udp/tcp selection is upto the application.\n");
    return true;
    }
  else
    {
    cRdCfg.OpenFile(this->configFile.c_str());// to do get the first configFile from this->configFile.
    if (cRdCfg.ExistFile())
      {
      cRdCfg.OpenFile (this->configFile.c_str());// reset the file read pointer to the beginning.
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
  while (!cRdCfg.EndOfFile())
    {
    strTag->clear();
    long iRd = cRdCfg.ReadLine (&strTag[0]);
    if (iRd > 0)
      {
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

void VideoStreamIGTLinkReceiver::InitializeDecodedFrame()
{
  if (!(this->decodedFrame == NULL))
    {
    delete[] this->decodedFrame;
    }
  this->decodedFrame = NULL;
  this->decodedFrame = new unsigned char[this->Width*this->Height*3>>1];
}

int VideoStreamIGTLinkReceiver::ProcessVideoStream(igtl_uint8* bitStream, int streamLength)
{
  //std::cerr << "Receiving Video data type." << std::endl;
  //this->videoMessageBuffer = new igtl_uint8[StreamLength];
  //memcpy(this->videoMessageBuffer, bitStream, StreamLength);// copy slow down the process, however, the videoMsg is a smart pointer, it gets deleted unpredictable.
  if(!decodeInstance)
    {
    return 0;
    }

  this->InitializeDecodedFrame();
  igtl_uint32 dimensions[2] = {static_cast<igtl_uint32>(Width), static_cast<igtl_uint32>(Height)};
  igtl_uint64 bitStreamLength = streamLength;
  int status = decodeInstance->DecodeBitStreamIntoFrame(bitStream, this->decodedFrame, dimensions , bitStreamLength);
  // return 2 for succefully decode one frame
  FILE* pYuvFile    = NULL;
  // Lenght input mode support
  if (kpOuputFileName.c_str())
    {
    pYuvFile = fopen (kpOuputFileName.c_str(), "ab");
    if (pYuvFile == NULL)
      {
      fprintf (stderr, "Can not open yuv file to output result of decoding..\n");
      }
    }
  igtl_uint32 stride[2] = {static_cast<igtl_uint32>(Width), static_cast<igtl_uint32>(Width/2)};
  SourcePicture pic;
  pic.data[0] = this->decodedFrame;
  pic.data[1] = this->decodedFrame+Width*Height;
  pic.data[2] = this->decodedFrame+Width*Height*5/4;
  //pic.data[0] =
  if(pYuvFile)
    {
    decodeInstance->Write2File (pYuvFile, pic.data, dimensions, stride);
    fclose (pYuvFile);
    }
  return status;
}

}// namespace igtl

