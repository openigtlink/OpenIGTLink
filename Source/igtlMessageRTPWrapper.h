/*=========================================================================
 
 Program:   The OpenIGTLink Library
 Language:  C++
 Web page:  http://openigtlink.org/
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#ifndef __igtlMessageRTPWrapper_h
#define __igtlMessageRTPWrapper_h

#include <string>

#include "igtlObject.h"
#include "igtlMacro.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlMessageFactory.h"
#include "igtlUDPServerSocket.h"
#include "igtlUDPClientSocket.h"
#include "igtlMutexLock.h"
#include "igtl_header.h"
#include "igtl_util.h"
#include "igtlTimeStamp.h"
#include "igtlOSUtil.h"

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#else
#include <sys/time.h>
#endif

namespace igtl
{
  
  class PacketBuffer {
  public:
    PacketBuffer(){iPacketCount = 0; pPacketLengthInByte.reserve(100); totalLength= 0; pBsBuf.reserve(100);};
    ~PacketBuffer(){pPacketLengthInByte.clear();pBsBuf.clear();};
    int   iPacketCount;              ///< count number of NAL coded already
    std::vector<int>  pPacketLengthInByte;       ///< length of NAL size in byte from 0 to iNalCount-1
    std::vector<unsigned char> pBsBuf;       ///< buffer of Packet contained
    int totalLength;
  };
  
  
  class ReorderBuffer
  {
  public:
    ReorderBuffer(){firstPacketLen=0;lastPacketLen=0;filledPacketNum=0; totFragNumber = 0;receivedLastFrag=false;receivedFirstFrag=false;};
    ReorderBuffer(ReorderBuffer const &anotherBuffer){firstPacketLen=anotherBuffer.firstPacketLen;lastPacketLen=anotherBuffer.lastPacketLen;filledPacketNum=anotherBuffer.filledPacketNum;receivedLastFrag=anotherBuffer.receivedLastFrag;receivedFirstFrag=anotherBuffer.receivedFirstFrag;};
    ~ReorderBuffer(){};
    unsigned char buffer[RTP_PAYLOAD_LENGTH*(16384-2)];  // we use 14 bits for fragment number, 2^14 = 16384. maximum
    unsigned char firstFragBuffer[RTP_PAYLOAD_LENGTH];
    unsigned char lastFragBuffer[RTP_PAYLOAD_LENGTH];
    igtl_uint32 firstPacketLen;
    igtl_uint32 lastPacketLen;
    igtl_uint32 filledPacketNum;
    igtl_uint32 totFragNumber;
    bool receivedLastFrag;
    bool receivedFirstFrag;
  };
  
  class UnWrappedMessage
  {
  public:
    UnWrappedMessage(){messageDataLength = 0; messagePackPointer = new unsigned char[RTP_PAYLOAD_LENGTH*(16384-2)];};
    UnWrappedMessage(UnWrappedMessage const &anotherMessage){};
    ~UnWrappedMessage(){
      if(messagePackPointer)
      {
        delete[] messagePackPointer;
        messagePackPointer = NULL;
      }
    };
    unsigned char* messagePackPointer;  // we use 14 bits for fragment number, 2^14 = 16384. maximum
    igtl_uint32 messageDataLength;
  };
  
  class IGTLCommon_EXPORT MessageRTPWrapper: public Object
  {
  public:
    typedef MessageRTPWrapper              Self;
    typedef Object               Superclass;
    typedef SmartPointer<Self>        Pointer;
    typedef SmartPointer<const Self>  ConstPointer;
    
    igtlTypeMacro(igtl::MessageRTPWrapper, Object)
    igtlNewMacro(igtl::MessageRTPWrapper);
    
  public:
    enum PacketStatus
    {
      PacketReady,
      WaitingForAnotherMSG,
      WaitingForFragment,
      MessageReady,
      ToUnpackAnotherMSG,
      WaitingForAnotherPacket
    };
    
    igtl_uint32 messageID;
    igtl_uint16 fragmentNumber;
    
    
    /// The message get fragmented and sent in different packets. The packets sending should have some interval in the function
    /// WrapMessageAndSend(), otherwize the network demanding would be too high to cause packet loss
    /// This variable need to be set according to the network bandwidth and the RTPPayload size
    int packetIntervalTime;
    
    std::vector<igtl_uint64> PacketSendTimeStampList;
    
    std::vector<igtl_uint64> PacketTotalLengthList;
    
    std::vector<igtl_uint64> PacketBeforeSendTimeStampList;
    
    std::vector<igtl_uint16> fragmentNumberList;
    //virtual void  AllocateScalars();
    
    /// Gets a pointer to the scalar data.
    //virtual void* GetScalarPointer();
    
    /// Sets the pointer to the scalar data (for fragmented pack support).
    //virtual void  SetScalarPointer(unsigned char * p);
    
    /// Gets the number of fragments for the packed (serialized) data. Returns numberOfDataFrag
    int GetNumberODataFragments() { return numberOfDataFrag;  /* the data for transmission is too big for UDP transmission, so the data will be transmitted by multiple packets*/ };
    
    /// Gets the number of fragments to be sent for the packed (serialized) data. Returns numberOfDataFragToSent
    int GetNumberODataFragToSent() { return numberOfDataFragToSent;  /* the data for transmission is too big for UDP transmission, so the data will be transmitted by multiple packets*/ };
    
    void SetFCFS(bool isFCFS){FCFS = isFCFS;};
    
    int WrapMessageAndPushToBuffer(igtl_uint8* messagePackPointer, int msgtotalLen);
    
    int SendBufferedDataWithInterval(igtl::UDPServerSocket::Pointer &socket, int interval);
    
    int WrapMessageAndSend(igtl::UDPServerSocket::Pointer &socket, igtl_uint8* messagePackPointer, int msgtotalLen);
    
    int PushDataIntoPacketBuffer(igtlUint8* UDPPacket, igtlUint16 PacketLen);
    
    int UnWrapPacketWithTypeAndName(const char *deviceType, const char * deviceName);
    
    igtl::MessageBase::Pointer UnWrapMessage(igtl_uint8* messageContent, int totMsgLen);
    
    void SetPayLoadType(unsigned char type){RTPPayLoadType = type;};
    
    ///Set the synchronization source identifier, different session has different SSRC
    void SetSSRC(igtl_uint32 identifier);
    
    ///Set the Contributing source identifier. different device has different CSRC
    void SetCSRC(igtl_uint32 identifier);
    
    ///Set the sequencen number at the rtp header
    void SetSeqNum(igtl_uint16 num);
    
    ///Set the current msg header
    void SetMSGHeader(igtl_uint8* header);
    
    int GetCurMSGLocation(){return this->curMSGLocation;};
    
    int GetPackedMSGLocation(){return this->curPackedMSGLocation;};
    
    int GetRTPWrapperStatus(){return status;};
    
    std::map<igtl_uint32, igtl::UnWrappedMessage*> unWrappedMessages;
    
    igtl::SimpleMutexLock* glock;
    
    
  protected:
    MessageRTPWrapper();
    ~MessageRTPWrapper();
    
    /// Gets a pointer to the scalar data (for fragmented pack support).
    igtl_uint8* GetPackPointer(){return packedMsg;};
    
    int WrapMessage(igtl_uint8* messageContent, int bodyMsgLen);
    
    
  private:
    igtl_uint16 extendedHeaderSize;
    igtl_uint8* packedMsg;
    igtl_uint8* MSGHeader;
    unsigned int curMSGLocation;
    unsigned int curPackedMSGLocation;
    unsigned int numberOfDataFrag;
    unsigned int numberOfDataFragToSent;
    unsigned char RTPPayLoadType;
    igtl_uint16 AvailabeBytesNum;
    igtl_uint16 SeqNum;
    int status;
    igtl_uint32 appSpecificFreq;
    igtl_uint32 SSRC;
    igtl_uint32 CSRC;
    igtl_uint32 fragmentTimeIncrement;
    igtl::ReorderBuffer* reorderBuffer;
    std::map<igtl_uint32, igtl::ReorderBuffer*> reorderBufferMap;
    PacketBuffer incommingPackets;
    PacketBuffer outgoingPackets;
    igtl::TimeStamp::Pointer wrapperTimer;
    bool FCFS; //first come first serve
    void SleepInNanoSecond(int nanoSecond);
  };
  
  
} // namespace igtl

#endif // _igtlMessageRTPWrapper_h