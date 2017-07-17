/*=========================================================================
 
 Program:   The OpenIGTLink Library
 Language:  C++
 Web page:  http://openigtlink.org/
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlMessageRTPWrapper.h"
#include <string.h>


namespace igtl {
  
  MessageRTPWrapper::MessageRTPWrapper():Object()
  {
    this->SeqNum = 0;
    this->RTPPayloadLength = RTP_PAYLOAD_LENGTH;
    this->AvailabeBytesNum = RTP_PAYLOAD_LENGTH;
    this->numberOfDataFrag = 1;
    this->numberOfDataFragToSent = 1;
    this->packedMsg = NULL;
    this->appSpecificFreq = 100;
    this->status = PacketReady;
    this->curMSGLocation = 0;
    this->curPackedMSGLocation = 0;
    this->fragmentNumber = 0;
    this->MSGHeader= new igtl_uint8[IGTL_HEADER_SIZE + IGTL_EXTENDED_HEADER_SIZE];
    this->glock = igtl::SimpleMutexLock::New();
    this->incommingPackets =  igtl::PacketBuffer();
    this->reorderBuffer = new igtl::ReorderBuffer();
    this->reorderBufferMap = std::map<igtl_uint32, igtl::ReorderBuffer*>();
    this->fragmentNumberList=std::vector<igtl_uint16>();
    this->PacketSendTimeStampList = std::vector<igtl_uint64>();
    this->PacketBeforeSendTimeStampList = std::vector<igtl_uint64>();
    this->PacketTotalLengthList = std::vector<igtl_uint64>();
    this->wrapperTimer = igtl::TimeStamp::New();
    this->FCFS=true;
    this->packetIntervalTime = 1;
    this->SSRC = 0;
    this->RTPPayloadType = 96; // https://tools.ietf.org/html/rfc3551#page-32 ,96 to 127 is for dynamic allocated type.
  }
  
  MessageRTPWrapper::~MessageRTPWrapper()
  {
    glock->Lock();
    std::map<igtl_uint32, igtl::ReorderBuffer*>::iterator itr = this->reorderBufferMap.begin();
    while (itr != this->reorderBufferMap.end())
      {
      // found it - delete it
      itr->second->~ReorderBuffer();
      this->reorderBufferMap.erase(itr);
      if(this->reorderBufferMap.size()==0)
        {
        break;
        }
      itr++;
      }
    std::map<igtl_uint32, igtl::UnWrappedMessage*>::iterator itr2 = this->unWrappedMessages.begin();
    while (itr2 != this->unWrappedMessages.end())
      {
      // found it - delete it
      itr2->second->~UnWrappedMessage();
      this->unWrappedMessages.erase(itr2);
      if(this->unWrappedMessages.size()==0)
        {
        break;
        }
      itr2++;
      }
    glock->Unlock();
    glock = NULL;
  }
  
  
  void MessageRTPWrapper::SetMSGHeader(igtl_uint8* header)
  {
    memcpy(this->MSGHeader, header, IGTL_HEADER_SIZE+IGTL_EXTENDED_HEADER_SIZE);
    ///Left shift the lower byte of the message ID by two bytes. and fill the last two byte with NoFragmentIndicator
    /// only valid if the messageID is less  than 2^16.
    memcpy(this->MSGHeader+IGTL_HEADER_SIZE+IGTL_EXTENDED_HEADER_SIZE-sizeof(messageID), this->MSGHeader+IGTL_HEADER_SIZE+IGTL_EXTENDED_HEADER_SIZE-FragmentIndexBytes,FragmentIndexBytes);
    igtl_uint16 temp = NoFragmentIndicator;
    memcpy(this->MSGHeader+IGTL_HEADER_SIZE+IGTL_EXTENDED_HEADER_SIZE-FragmentIndexBytes, (void*)(&temp), FragmentIndexBytes);
  }
  
  
  void MessageRTPWrapper::SetSSRC(igtl_uint32 identifier)
  {
    SSRC = identifier;
    if(igtl_is_little_endian())
      {
      SSRC = BYTE_SWAP_INT32(SSRC);
      }
  }
  
  void MessageRTPWrapper::SetCSRC(igtl_uint32 identifier)
  {
    CSRC = identifier;
    if(igtl_is_little_endian())
      {
      CSRC = BYTE_SWAP_INT32(CSRC);
      }
  }
  
  void MessageRTPWrapper::SetSeqNum(igtl_uint16 num)
  {
    SeqNum = num;
    if(igtl_is_little_endian())
      {
      SeqNum = BYTE_SWAP_INT16(num);
      }
  }
  
  int MessageRTPWrapper::PushDataIntoPacketBuffer(igtlUint8* UDPPacket, igtlUint16 PacketLen)
  {
    this->glock->Lock();
    if(this->incommingPackets.pPacketLengthInByte.size()>PacketMaximumBufferNum)
      {
      igtlUint16 firstMsgLen = this->incommingPackets.pPacketLengthInByte[0];
      this->incommingPackets.pPacketLengthInByte.erase(this->incommingPackets.pPacketLengthInByte.begin());
      this->incommingPackets.pBsBuf.erase(this->incommingPackets.pBsBuf.begin(), this->incommingPackets.pBsBuf.begin()+firstMsgLen);
      this->incommingPackets.totalLength -= firstMsgLen;
      }
    this->incommingPackets.pPacketLengthInByte.push_back(PacketLen);
    this->incommingPackets.pBsBuf.insert(this->incommingPackets.pBsBuf.end(), UDPPacket, UDPPacket+PacketLen);
    this->incommingPackets.totalLength += PacketLen;
    if (this->incommingPackets.totalLength  == this->incommingPackets.pBsBuf.size())
      {
      this->glock->Unlock();
      return 1;
      
      }
    else
      {
      this->glock->Unlock();
      return 0;
      }
  }
  
  int MessageRTPWrapper::SendBufferedDataWithInterval(igtl::UDPServerSocket::Pointer &socket, int interval) //interval is in nanosecond
  {
    int totalMsgLen = this->outgoingPackets.totalLength;
    int sendMsgLen = 0;
    while (this->outgoingPackets.pPacketLengthInByte.size())
      {
      igtlUint8 * UDPPacket;
      igtlUint16 currentMsgLen;
      this->glock->Lock();
      if(this->FCFS==true)
        {
        currentMsgLen = this->outgoingPackets.pPacketLengthInByte[0];
        UDPPacket = new igtlUint8[currentMsgLen];
        memcpy(UDPPacket, this->outgoingPackets.pBsBuf.data(), currentMsgLen);
        this->outgoingPackets.pPacketLengthInByte.erase(this->outgoingPackets.pPacketLengthInByte.begin());
        this->outgoingPackets.pBsBuf.erase(this->outgoingPackets.pBsBuf.begin(), this->outgoingPackets.pBsBuf.begin()+currentMsgLen);
        this->outgoingPackets.totalLength -= currentMsgLen;
        }
      else
        {
        int packetNum = outgoingPackets.pPacketLengthInByte.size();
        currentMsgLen = this->outgoingPackets.pPacketLengthInByte[packetNum-1];
        UDPPacket = new igtlUint8[currentMsgLen];
        memcpy(UDPPacket, this->outgoingPackets.pBsBuf.data() + this->outgoingPackets.pBsBuf.size()-currentMsgLen, currentMsgLen);
        this->outgoingPackets.pPacketLengthInByte.erase(this->outgoingPackets.pPacketLengthInByte.end()-1);
        this->outgoingPackets.pBsBuf.erase(this->outgoingPackets.pBsBuf.begin() + this->outgoingPackets.pBsBuf.size()-currentMsgLen, this->outgoingPackets.pBsBuf.end());
        this->outgoingPackets.totalLength -= currentMsgLen;
        }
      this->glock->Unlock();
      int numByteSent = socket->WriteSocket(UDPPacket, currentMsgLen);
      sendMsgLen += numByteSent;
      if (numByteSent != currentMsgLen)
        {
        return 0;
        }
      }
    if (sendMsgLen == totalMsgLen)
      {
      return 1;
      }
    else
      {
      return 0;
      }
  }
  
  int MessageRTPWrapper::WrapMessageAndPushToBuffer(igtl_uint8* messagePackPointer, int msgtotalLen)
  {
    igtl_uint8* messageContentPointer = messagePackPointer+IGTL_HEADER_SIZE+IGTL_EXTENDED_HEADER_SIZE;
    this->SetMSGHeader((igtl_uint8*)messagePackPointer);
    int MSGContentLength = msgtotalLen - IGTL_HEADER_SIZE-IGTL_EXTENDED_HEADER_SIZE; // this is the m_content size + meta data size
    int leftMsgLen = MSGContentLength;
    igtl_uint8* leftmessageContent = messageContentPointer;
    this->PacketTotalLengthList.clear();
    do
      {
      status = this->WrapMessage(leftmessageContent, leftMsgLen);
      if (status == igtl::MessageRTPWrapper::ProcessFragment || status == igtl::MessageRTPWrapper::PacketReady)
        {
        this->glock->Lock();
        if(this->outgoingPackets.pPacketLengthInByte.size()>PacketMaximumBufferNum)
          {
          igtlUint16 firstMsgLen = this->outgoingPackets.pPacketLengthInByte[0];
          this->outgoingPackets.pPacketLengthInByte.erase(this->outgoingPackets.pPacketLengthInByte.begin());
          this->outgoingPackets.pBsBuf.erase(this->outgoingPackets.pBsBuf.begin(), this->outgoingPackets.pBsBuf.begin()+firstMsgLen);
          this->outgoingPackets.totalLength -= firstMsgLen;
          }
        this->outgoingPackets.pPacketLengthInByte.push_back(this->GetPackedMSGLocation());
        this->outgoingPackets.pBsBuf.insert(this->outgoingPackets.pBsBuf.end(), this->GetPackPointer(), this->GetPackPointer()+this->GetPackedMSGLocation());
        this->outgoingPackets.totalLength += this->GetPackedMSGLocation();
        this->PacketTotalLengthList.push_back(this->GetPackedMSGLocation());
        if (this->outgoingPackets.totalLength  == this->outgoingPackets.pBsBuf.size())
          {
          this->glock->Unlock();
          }
        else
          {
          this->glock->Unlock();
          return 0;
          }
        }
      leftmessageContent = messageContentPointer + this->GetCurMSGLocation();
      leftMsgLen = MSGContentLength - this->GetCurMSGLocation();
      }while(leftMsgLen>0 && status!=igtl::MessageRTPWrapper::PacketReady); // to do when bodyMsgLen
    return 1;
  }
  
  
  int MessageRTPWrapper::UnWrapPacketWithTypeAndName(const char *deviceType, const char * deviceName)
  {
    if (this->incommingPackets.pPacketLengthInByte.size())
      {
      this->glock->Lock();
      igtlUint8 * UDPPacket;
      igtlUint16 totMsgLen;
      if(this->FCFS==true)
        {
        totMsgLen = this->incommingPackets.pPacketLengthInByte[0];
        UDPPacket = new igtlUint8[totMsgLen];
        memcpy(UDPPacket, this->incommingPackets.pBsBuf.data(), totMsgLen);
        this->incommingPackets.pPacketLengthInByte.erase(this->incommingPackets.pPacketLengthInByte.begin());
        this->incommingPackets.pBsBuf.erase(this->incommingPackets.pBsBuf.begin(), this->incommingPackets.pBsBuf.begin()+totMsgLen);
        this->incommingPackets.totalLength -= totMsgLen;
        }
      else
        {
        int packetNum = incommingPackets.pPacketLengthInByte.size();
        totMsgLen = this->incommingPackets.pPacketLengthInByte[packetNum-1];
        UDPPacket = new igtlUint8[totMsgLen];
        memcpy(UDPPacket, this->incommingPackets.pBsBuf.data() + this->incommingPackets.pBsBuf.size()-totMsgLen, totMsgLen);
        this->incommingPackets.pPacketLengthInByte.erase(this->incommingPackets.pPacketLengthInByte.end()-1);
        this->incommingPackets.pBsBuf.erase(this->incommingPackets.pBsBuf.begin() + this->incommingPackets.pBsBuf.size()-totMsgLen, this->incommingPackets.pBsBuf.end());
        this->incommingPackets.totalLength -= totMsgLen;
        }
      this->glock->Unlock();
      // Set up the RTP header:
      igtl_uint32  rtpProfileBytes, timeIncrement;
      rtpProfileBytes = *((igtl_uint32*)UDPPacket);
      //bool rtpMarkerBit = (rtpHdr&0x00800000) != 0;
      timeIncrement = *(igtl_uint32*)(UDPPacket+sizeof(rtpProfileBytes));
      igtl_uint32 SSRC = *(igtl_uint32*)(UDPPacket+sizeof(rtpProfileBytes)+sizeof(timeIncrement));
      if(igtl_is_little_endian())
        {
        rtpProfileBytes = BYTE_SWAP_INT32(rtpProfileBytes);
        timeIncrement = BYTE_SWAP_INT32(timeIncrement);
        SSRC = BYTE_SWAP_INT32(SSRC);
        }
      int curPackedMSGLocation = RTP_HEADER_LENGTH;
      status = WaitingForAnotherPacket;
      while(curPackedMSGLocation<totMsgLen)
        {
        igtl::MessageHeader::Pointer header = igtl::MessageHeader::New();
        header->AllocatePack();
        memcpy(header->GetPackPointer(), UDPPacket + curPackedMSGLocation, IGTL_HEADER_SIZE);
        igtl_uint16 fragmentField;
        memcpy(&fragmentField, (void*)(UDPPacket + RTP_HEADER_LENGTH+IGTL_HEADER_SIZE+IGTL_EXTENDED_HEADER_SIZE-FragmentIndexBytes),FragmentIndexBytes);
        messageID = 0;
        //fill the lower two bytes of message ID with the upper two byte from packet data.
        memcpy((char*)(&messageID)+FragmentIndexBytes, (void*)(UDPPacket + RTP_HEADER_LENGTH+IGTL_HEADER_SIZE+IGTL_EXTENDED_HEADER_SIZE-sizeof(messageID)), FragmentIndexBytes);
        // Get rid of the fragment field number.
        memcpy((void*)(UDPPacket + RTP_HEADER_LENGTH+IGTL_HEADER_SIZE+IGTL_EXTENDED_HEADER_SIZE-sizeof(messageID)), &messageID, sizeof(messageID));
        if(igtl_is_little_endian())
          {
          fragmentField = BYTE_SWAP_INT16(fragmentField);
          }
        //this->reorderBufferMap.erase(it);
        if (reorderBufferMap.size()>ReorderBufferMaximumSize) // get rid of the first reoderBuffer when waiting for a long time
          {
          if(reorderBufferMap.begin()->first != messageID)
            {
            reorderBufferMap.erase(reorderBufferMap.begin());
            }
          else
            {
            reorderBufferMap.erase(reorderBufferMap.begin()++);
            }
          }
        std::map<igtl_uint32, igtl::ReorderBuffer*>::iterator it = this->reorderBufferMap.find(messageID);
        if (it == this->reorderBufferMap.end())
          {
          this->reorderBufferMap.insert(std::pair<igtl_uint32,igtl::ReorderBuffer*>(messageID,new igtl::ReorderBuffer()));
          it = this->reorderBufferMap.find(messageID);
          
          }
        this->reorderBuffer = it->second;
        header->Unpack();
        if(fragmentField==NoFragmentIndicator) // fragment doesn't exist
          {
          if (strcmp(header->GetDeviceType(),deviceType)==0 && strcmp(header->GetDeviceName(),deviceName)==0)
            {
            memcpy(this->reorderBuffer->firstFragBuffer, UDPPacket + curPackedMSGLocation, header->GetBodySizeToRead()+IGTL_HEADER_SIZE);
            this->reorderBuffer->filledPacketNum = 1;
            reorderBuffer->receivedFirstFrag = true;
            reorderBuffer->receivedLastFrag = true;
            reorderBuffer->firstPacketLen = header->GetBodySizeToRead()+IGTL_HEADER_SIZE;
            igtl::UnWrappedMessage* message = new igtl::UnWrappedMessage();
            message->messageDataLength = reorderBuffer->firstPacketLen;
            memcpy(message->messagePackPointer, reorderBuffer->firstFragBuffer, reorderBuffer->firstPacketLen);
            glock->Lock();
            unWrappedMessages.insert(std::pair<igtl_uint32, igtl::UnWrappedMessage*>(it->first,message));
            glock->Unlock();
            delete it->second;
            it->second = NULL;
            this->reorderBufferMap.erase(it);
            status = MessageReady;
            }
          curPackedMSGLocation += header->GetBodySizeToRead()+IGTL_HEADER_SIZE;
          }
        else
          {
          if (strcmp(header->GetDeviceType(),deviceType)==0 && strcmp(header->GetDeviceName(),deviceName)==0)
            {
            int bodyMsgLength = (RTPPayloadLength-IGTL_HEADER_SIZE-IGTL_EXTENDED_HEADER_SIZE);//this is the length of the body within a full fragment Packet
            if(fragmentField==FragmentBeginIndicator)// To do, fix the issue when later fragment arrives earlier than the beginning fragment
              {
              *(UDPPacket + curPackedMSGLocation + IGTL_HEADER_SIZE+IGTL_EXTENDED_HEADER_SIZE-FragmentIndexBytes) = NoFragmentIndicator; // set the fragment no. to 0000
              memcpy(reorderBuffer->firstFragBuffer, UDPPacket + curPackedMSGLocation, totMsgLen-curPackedMSGLocation);
              reorderBuffer->firstPacketLen = totMsgLen-curPackedMSGLocation;
              curPackedMSGLocation = totMsgLen;
              reorderBuffer->receivedFirstFrag = true;
              status = WaitingForAnotherPacket;
              }
            else if(fragmentField>=FragmentEndIndicator)// this is the last fragment
              {
              reorderBuffer->totFragNumber = fragmentField - FragmentEndIndicator + 1;
              memcpy(reorderBuffer->lastFragBuffer, UDPPacket + RTP_HEADER_LENGTH+IGTL_HEADER_SIZE+IGTL_EXTENDED_HEADER_SIZE, totMsgLen-(RTP_HEADER_LENGTH+IGTL_HEADER_SIZE+IGTL_EXTENDED_HEADER_SIZE));
              reorderBuffer->receivedLastFrag = true;
              reorderBuffer->lastPacketLen = totMsgLen-(RTP_HEADER_LENGTH+IGTL_HEADER_SIZE+IGTL_EXTENDED_HEADER_SIZE);
              curPackedMSGLocation = totMsgLen;
              status = WaitingForAnotherPacket;
              }
            else if(fragmentField>FragmentBeginIndicator && fragmentField<FragmentEndIndicator)
              {
              int curFragNumber = fragmentField - FragmentBeginIndicator;
              memcpy(reorderBuffer->buffer+(curFragNumber-1)*bodyMsgLength, UDPPacket + RTP_HEADER_LENGTH+IGTL_HEADER_SIZE+IGTL_EXTENDED_HEADER_SIZE, totMsgLen-(RTP_HEADER_LENGTH+IGTL_HEADER_SIZE+IGTL_EXTENDED_HEADER_SIZE));
              status = WaitingForAnotherPacket;
              }
            reorderBuffer->filledPacketNum++;
            if(reorderBuffer->receivedFirstFrag == true && reorderBuffer->receivedLastFrag == true && reorderBuffer->filledPacketNum == reorderBuffer->totFragNumber)
              {
              igtl::UnWrappedMessage* message = new igtl::UnWrappedMessage();
              message->messageDataLength = reorderBuffer->lastPacketLen+reorderBuffer->firstPacketLen+ (reorderBuffer->totFragNumber-2)*(RTPPayloadLength-IGTL_HEADER_SIZE-IGTL_EXTENDED_HEADER_SIZE);
              memcpy(message->messagePackPointer, reorderBuffer->firstFragBuffer, reorderBuffer->firstPacketLen);
              memcpy(message->messagePackPointer+reorderBuffer->firstPacketLen, reorderBuffer->buffer, (RTPPayloadLength-IGTL_HEADER_SIZE-IGTL_EXTENDED_HEADER_SIZE)*(reorderBuffer->totFragNumber-2));
              memcpy(message->messagePackPointer+reorderBuffer->firstPacketLen+(RTPPayloadLength-IGTL_HEADER_SIZE-IGTL_EXTENDED_HEADER_SIZE)*(reorderBuffer->totFragNumber-2), reorderBuffer->lastFragBuffer, reorderBuffer->lastPacketLen);
              glock->Lock();
              unWrappedMessages.insert(std::pair<igtl_uint32, igtl::UnWrappedMessage*>(it->first,message));
              glock->Unlock();
              std::map<igtl_uint32, igtl::ReorderBuffer*>::iterator it_forDelete= this->reorderBufferMap.begin();
              while(it_forDelete != this->reorderBufferMap.end())
                {
                delete it_forDelete->second;
                it_forDelete->second = NULL;
                this->reorderBufferMap.erase(it_forDelete);
                if (this->reorderBufferMap.size())
                  {
                  if (it_forDelete->first == it->first)
                    {
                    break;
                    }
                  }
                else
                  {
                  break;
                  }
                it_forDelete = this->reorderBufferMap.begin();
                }
              status = MessageReady;
              }
            }
          else
            {
            curPackedMSGLocation = totMsgLen;
            }
          break;
          }
        }
      delete[] UDPPacket;
      return 1;
      }
    return 0;
  }
  
  int MessageRTPWrapper::WrapMessageAndSend(igtl::UDPServerSocket::Pointer &socket, igtl_uint8* messagePackPointer, int msgtotalLen)
  {
    igtl_uint8* messageContentPointer = messagePackPointer+IGTL_HEADER_SIZE+IGTL_EXTENDED_HEADER_SIZE;
    this->SetMSGHeader((igtl_uint8*)messagePackPointer);
    int MSGContentLength = msgtotalLen- IGTL_HEADER_SIZE-IGTL_EXTENDED_HEADER_SIZE; // this is the m_content size + meta data size
    int leftMsgLen = MSGContentLength;
    igtl_uint8* leftmessageContent = messageContentPointer;
    this->fragmentNumberList.clear();
    this->PacketSendTimeStampList.clear();
    this->PacketBeforeSendTimeStampList.clear();
    this->PacketTotalLengthList.clear();
    do
      {
      status = this->WrapMessage(leftmessageContent, leftMsgLen);
      if (status == igtl::MessageRTPWrapper::ProcessFragment || status == igtl::MessageRTPWrapper::PacketReady)
        {
        this->wrapperTimer->GetTime();
        this->PacketBeforeSendTimeStampList.push_back(this->wrapperTimer->GetTimeStampInNanoseconds());
        this->glock->Lock();
        int numByteSent = socket->WriteSocket(this->GetPackPointer(), this->GetPackedMSGLocation());
        this->glock->Unlock();
        this->fragmentNumberList.push_back(this->fragmentNumber);
        this->wrapperTimer->GetTime();
        this->PacketSendTimeStampList.push_back(this->wrapperTimer->GetTimeStampInNanoseconds());
        this->PacketTotalLengthList.push_back(numByteSent);
        }
      this->SleepInNanoSecond(this->packetIntervalTime);
      leftmessageContent = messageContentPointer + this->GetCurMSGLocation();
      leftMsgLen = MSGContentLength - this->GetCurMSGLocation();
      }while(leftMsgLen>0 && status!=igtl::MessageRTPWrapper::PacketReady); // to do when bodyMsgLen
    return 1;
  }
  
  void MessageRTPWrapper::SleepInNanoSecond(int nanoSecond)
  {
#if defined(_WIN32) && !defined(__CYGWIN__)
  
  // Call Windows Native Sleep() function
  //::Sleep(nanoSecond/1000);
  
#else
  
  struct timespec req;
  req.tv_sec  = (int) nanoSecond/1000000000;
  req.tv_nsec = nanoSecond % 1000000000;
  
  while ((nanosleep(&req, &req) == -1) && (errno == EINTR))
    {
    continue;
    }
  
#endif
  }
  
  // SSRC should be set only once before wraping the message in a transmission session.
  // CSRC should be set when messages from different devices are packed into the same Packet.
  // this special header is here for the compatiblity with standard RTP protocal.
  // we have our own defined openigtlink header, which is 58 byte size.
  // append the openigtlink header as the Extension header after CSRC.
  int MessageRTPWrapper::WrapMessage(igtl_uint8* messageContent, int bodyMsgLen)
  {
    // Set up the RTP header:
    igtl_uint32 rtpProfileBytes = 0x80000000; // RTP version 2;
    rtpProfileBytes |= (RTPPayloadType<<16);
    rtpProfileBytes |= SeqNum; // sequence number, increment the sequence number after sending the data
  #if defined(WIN32) || defined(_WIN32)
    
    //LARGE_INTEGER tick;
    //
    //::QueryPerformanceCounter( &tick );
    //
    //TimeStampType value =
    //    static_cast< TimeStampType >( (__int64)tick.QuadPart ) /
    //    this->m_WinFrequency;
    //
    //value += this->m_WinOrigin;
    //
    //double second = floor(value);
    
    clock_t time1 = clock();
    clock_t winOriginTime = time(NULL);
    clock_t second =  + (time1 - winOriginTime) / CLOCKS_PER_SEC;
    clock_t microsecond = (time1 - winOriginTime) % CLOCKS_PER_SEC * (1e6 / CLOCKS_PER_SEC);
    clock_t timeIncrement = appSpecificFreq*second;
    timeIncrement += igtl_uint32(appSpecificFreq*(microsecond / 1.0e6) + 0.5);
  #else
    struct timeval timeNow;
    gettimeofday(&timeNow, NULL);
    igtl_uint32 timeIncrement = (appSpecificFreq*timeNow.tv_sec); //need to check the frequency of different application
    timeIncrement += igtl_uint32(appSpecificFreq*(timeNow.tv_usec/1.0e6)+ 0.5);
  #endif
    //igtl_uint32 CSRC = 0x00000000; not used currently
    if(igtl_is_little_endian())
      {
      rtpProfileBytes = BYTE_SWAP_INT32(rtpProfileBytes);
      timeIncrement = BYTE_SWAP_INT32(timeIncrement);
      }
    if (status == PacketReady)
      {
      delete packedMsg;
      packedMsg = new unsigned char[RTPPayloadLength + RTP_HEADER_LENGTH];
      AvailabeBytesNum = RTPPayloadLength;
      curMSGLocation = 0;
      curPackedMSGLocation = 0;
      fragmentNumber = -1; // -1 = 0XFFFF to indicate the message has no fragments
      }
    if (status != ProcessFragment)
      {
      this->fragmentTimeIncrement = timeIncrement;
      memcpy(packedMsg, (void *)(&rtpProfileBytes), sizeof(rtpProfileBytes));
      memcpy(packedMsg+sizeof(rtpProfileBytes), (void *)(&timeIncrement), sizeof(timeIncrement));
      memcpy(packedMsg+sizeof(rtpProfileBytes)+sizeof(timeIncrement), (void *)(&SSRC), sizeof(SSRC)); // SSRC needs to set by different devices, collision should be avoided.
      curPackedMSGLocation += RTP_HEADER_LENGTH;
      if (bodyMsgLen <= (AvailabeBytesNum-IGTL_HEADER_SIZE-IGTL_EXTENDED_HEADER_SIZE))
        {
        igtl_uint16 temp = NoFragmentIndicator;
        memcpy(this->MSGHeader+IGTL_HEADER_SIZE+IGTL_EXTENDED_HEADER_SIZE-FragmentIndexBytes, (void*)&temp,FragmentIndexBytes);// no fragmented message here
        memcpy(packedMsg+curPackedMSGLocation, this->MSGHeader, IGTL_HEADER_SIZE+IGTL_EXTENDED_HEADER_SIZE);
        curPackedMSGLocation += (IGTL_HEADER_SIZE + IGTL_EXTENDED_HEADER_SIZE);
        memcpy(packedMsg + curPackedMSGLocation, (void *)(messageContent), bodyMsgLen);
        AvailabeBytesNum -= (bodyMsgLen+IGTL_EXTENDED_HEADER_SIZE+IGTL_HEADER_SIZE);
        curPackedMSGLocation += bodyMsgLen;
        curMSGLocation += bodyMsgLen;
        status = PacketReady;// now we send the Packet immediately, however, it is possible to send after the packt is full.
        SeqNum++;
        }
      else if(bodyMsgLen > (AvailabeBytesNum-IGTL_HEADER_SIZE-IGTL_EXTENDED_HEADER_SIZE))
        {
        igtl_uint16 temp = FragmentBeginIndicator;
        if(igtl_is_little_endian())
          {
          temp = BYTE_SWAP_INT16(temp);
          }
        memcpy(this->MSGHeader+IGTL_HEADER_SIZE+IGTL_EXTENDED_HEADER_SIZE-FragmentIndexBytes, (void*)&temp,FragmentIndexBytes);// no fragmented message here
        // fragmented message exists, first bit indicate the existance, the second bit indicates if the current section is the ending fragment, the other 14 bits indicates the fragements' sequence number.
        memcpy(packedMsg+curPackedMSGLocation, this->MSGHeader, IGTL_HEADER_SIZE+IGTL_EXTENDED_HEADER_SIZE);
        curPackedMSGLocation += (IGTL_HEADER_SIZE + IGTL_EXTENDED_HEADER_SIZE);
        memcpy(packedMsg + curPackedMSGLocation, (void *)(messageContent), AvailabeBytesNum-IGTL_EXTENDED_HEADER_SIZE-IGTL_HEADER_SIZE);
        status = ProcessFragment;
        this->curPackedMSGLocation = RTPPayloadLength+RTP_HEADER_LENGTH;
        this->curMSGLocation = AvailabeBytesNum-IGTL_HEADER_SIZE-IGTL_EXTENDED_HEADER_SIZE;
        AvailabeBytesNum = RTPPayloadLength;
        SeqNum++;
        fragmentNumber++;
        }
      }
    else
      {
      memcpy(packedMsg, (void *)(&rtpProfileBytes), sizeof(rtpProfileBytes));
      memcpy(packedMsg+sizeof(rtpProfileBytes), (void *)(&this->fragmentTimeIncrement), sizeof(this->fragmentTimeIncrement));
      memcpy(packedMsg+sizeof(rtpProfileBytes)+sizeof(this->fragmentTimeIncrement), (void *)(&SSRC), sizeof(SSRC)); // SSRC needs to set by different devices, collision should be avoided.
      curPackedMSGLocation = RTP_HEADER_LENGTH;
      fragmentNumber++;
      if (bodyMsgLen <= (AvailabeBytesNum-IGTL_HEADER_SIZE-IGTL_EXTENDED_HEADER_SIZE))
        {
        igtl_uint16 temp = FragmentEndIndicator+fragmentNumber; //set the seconde bit to be 1, indicates the end of fragmented msg.;
        if(igtl_is_little_endian())
          {
          temp = BYTE_SWAP_INT16(temp);
          }
        memcpy(this->MSGHeader+IGTL_HEADER_SIZE+IGTL_EXTENDED_HEADER_SIZE-FragmentIndexBytes, (void*)&temp,FragmentIndexBytes);
        // fragmented message exists, first bit indicate the existance, the second bit indicates if the current section is the ending fragment, the other 14 bits indicates the fragements' sequence number.
        memcpy(packedMsg+curPackedMSGLocation, this->MSGHeader, IGTL_HEADER_SIZE+IGTL_EXTENDED_HEADER_SIZE);
        curPackedMSGLocation += (IGTL_HEADER_SIZE + IGTL_EXTENDED_HEADER_SIZE);
        memcpy(packedMsg + curPackedMSGLocation, (void *)(messageContent), bodyMsgLen);
        this->curPackedMSGLocation += bodyMsgLen;
        // when it is packing the fragment, we want to sent the data ASAP, otherwize, we will wait for another message
        this->curMSGLocation += bodyMsgLen;
        status = PacketReady;
        }
      else if(bodyMsgLen > (AvailabeBytesNum-IGTL_HEADER_SIZE-IGTL_EXTENDED_HEADER_SIZE))
        {
        igtl_uint16 temp = FragmentBeginIndicator+fragmentNumber;
        if(igtl_is_little_endian())
          {
          temp = BYTE_SWAP_INT16(temp);
          }
        memcpy(this->MSGHeader+IGTL_HEADER_SIZE+IGTL_EXTENDED_HEADER_SIZE-FragmentIndexBytes, (void*)&temp,FragmentIndexBytes);//set the second bit to be 0, indicates more fragmented msg are comming.
        memcpy(packedMsg+curPackedMSGLocation, MSGHeader, IGTL_HEADER_SIZE+IGTL_EXTENDED_HEADER_SIZE);
        curPackedMSGLocation += (IGTL_HEADER_SIZE + IGTL_EXTENDED_HEADER_SIZE);
        memcpy(packedMsg + curPackedMSGLocation, (void *)(messageContent), AvailabeBytesNum-IGTL_EXTENDED_HEADER_SIZE-IGTL_HEADER_SIZE);
        status = ProcessFragment;
        this->curMSGLocation += (AvailabeBytesNum-IGTL_EXTENDED_HEADER_SIZE-IGTL_HEADER_SIZE);
        curPackedMSGLocation += (AvailabeBytesNum-IGTL_EXTENDED_HEADER_SIZE-IGTL_HEADER_SIZE);
        }
      AvailabeBytesNum = RTPPayloadLength;
      SeqNum++;
      }
    return status;
  }
  
  
  igtl::MessageBase::Pointer MessageRTPWrapper::UnWrapMessage(igtl_uint8* messageContent, int bodyMsgLen)
  {
    return NULL;
  }
  
} // namespace igtl