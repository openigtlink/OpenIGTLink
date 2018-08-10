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


/// This number defines the maximum number for UDP packet buffering, to avoid overflow of the buffer, the first buffered packet will be
#define PacketMaximumBufferNum 1000
#define ReorderBufferMaximumSize 200
#define FragmentIndexBytes 2
#define FragmentBeginIndicator 0X8000
#define FragmentEndIndicator 0XE000
#define NoFragmentIndicator 0X0000

namespace igtl
{
  /// The MessageRTPWrapper class is the class to fragment/assemble messages from all type classes
  /// into UDP packets, as The message classes can be used
  /// for wrapping V3 OpenIGTLink message byte streams into UDPPackets.
  /// The class can also unwrapping the RTPPackets into OpenIGTLink messages.
  ///
  /// The typical wrapping or unwrapping procedures is demonstrated in the VideoStreamIGTLinkServer,  VieoStreamIGTLinkReceiver and igtlMessageRTPWrapperTest
  ///
  ///  Here, an V3 image message is used as an example.
  ///  Assume the length of image is 25000 with 57 byte of meta information, which is longer than the RTP Packet payload size 8900,
  ///  so the message is fragmented into several RTPPackets.
  ///  0            58                  70              142                           25142               25160         25199
  ///
  ///  |____________|___________________|_______________|_____________________________|___________________|_____________|
  ///  m_Header     m_ExtendedHeader    m_ImageHeader   m_Image                       m_MetaDataHeader    m_MetaData
  ///               m_Body              m_Content (old m_Body)
  ///
  ///  All the fragments will be attached with a 12 bytes RTP Header, the RTP Header has the format:
  ///  0-----------4------------8------------------------16-------------------------31 bit number
  ///  |Version|P|X|     CC     |M|     Payload Type      |     Sequence Number      |
  ///   -----------------------------------------------------------------------------
  ///  |                              Time Stamp                                     |
  ///   -----------------------------------------------------------------------------
  ///  |               Source Synchronization Identifier(SSRC)                       |
  ///   -----------------------------------------------------------------------------
  ///
  ///  Fragment 1, total length 8912:
  ///  0          12           70                  82              154                                      8912
  ///  |__________|____________|___________________|_______________|________________________________________|
  ///  RTPHeader  m_Header     m_ExtendedHeader1    m_ImageHeader   m_Image1 (section 1)
  ///
  ///
  ///
  ///  Fragment 2, total length 8912:
  ///  0          12           70                  82              154                                      8912
  ///  |__________|____________|___________________|_______________|________________________________________|
  ///  RTPHeader  m_Header     m_ExtendedHeader2   m_ImageHeader   m_Image2 (section 2)
  ///
  ///
  ///
  ///  Fragment 3, total length 7695, for the simplicity, M_Meta* here includs the m_MetaDataHeader and m_MetaData:
  ///  0          12           70                  82              154                       7636      7695
  ///  |__________|____________|___________________|_______________|_________________________|_________|
  ///  RTPHeader  m_Header     m_ExtendedHeader3   m_ImageHeader   m_Image3 (section 3)      M_Meta*
  ///
  ///
  ///  The m_image is fragmented into three parts:
  ///  0                        8758                    17516             25000
  ///  |________________________|_______________________|_________________|
  ///  m_Image1                 m_Image2                 m_Image3
  ///
  ///
  ///  The last two bytes are used to reorder the packets if they arrive at different time points.
  ///  0x8000 indicates the first RTP packet
  ///  m_ExtendedHeader1
  ///  | 0xXXXX 0xXXXX.....      0xXXXX 0xXXXX |0x8000|
  ///  First 10 Bytes from  m_ExtendedHeader
  ///
  ///  The following RTP packet increments the field 0x8000
  ///  m_ExtendedHeader2
  ///  | 0xXXXX 0xXXXX.....      0xXXXX 0xXXXX |0x8000|
  ///  First 10 Bytes from  m_ExtendedHeader
  ///
  ///  0xE000 indicates the last RTP packet
  ///  m_ExtendedHeader3
  ///  | 0xXXXX 0xXXXX.....      0xXXXX 0xXXXX |0x8000|
  ///  First 10 Bytes from  m_ExtendedHeader
  

  class PacketBuffer {
  public:
    PacketBuffer(){pPacketLengthInByte.reserve(PacketMaximumBufferNum); totalLength= 0; pBsBuf.reserve(PacketMaximumBufferNum*(RTP_PAYLOAD_LENGTH+RTP_HEADER_LENGTH));};
    ~PacketBuffer(){pPacketLengthInByte.clear();pBsBuf.clear();};
    std::vector<int>  pPacketLengthInByte;       ///< length of udp packet size in byte from 0 to number of packet - 1
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
    igtlTypeMacro(igtl::MessageRTPWrapper, Object)
    igtlNewMacro(igtl::MessageRTPWrapper);
    
  public:
    enum PacketStatus
    {
      PacketReady,
      WaitingForAnotherMSG,
      ProcessFragment,
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
    
    /// Gets the number of fragments for the packed (serialized) data. Returns numberOfDataFrag
    int GetNumberODataFragments() { return numberOfDataFrag;  /* the data for transmission is too big for UDP transmission, so the data will be transmitted by multiple packets*/ };
    
    /// Gets the number of fragments to be sent for the packed (serialized) data. Returns numberOfDataFragToSent
    int GetNumberODataFragToSent() { return numberOfDataFragToSent;  /* the data for transmission is too big for UDP transmission, so the data will be transmitted by multiple packets*/ };
    
    void SetFCFS(bool isFCFS){FCFS = isFCFS;};
    
    void SetRTPPayloadType(igtl_uint8 payLoadType){this->RTPPayloadType = payLoadType;};
    
    igtl_uint8 GetRTPPayLoadType(){return this->RTPPayloadType;};
    
    int WrapMessageAndPushToBuffer(igtl_uint8* messagePackPointer, int msgtotalLen);
    
    int SendBufferedDataWithInterval(igtl::UDPServerSocket::Pointer &socket, int interval);
    
    int WrapMessageAndSend(igtl::UDPServerSocket::Pointer &socket, igtl_uint8* messagePackPointer, int msgtotalLen);
    
    int PushDataIntoPacketBuffer(igtlUint8* UDPPacket, igtlUint16 PacketLen);
    
    int UnWrapPacketWithTypeAndName(const char *deviceType, const char * deviceName);
    
    igtl::MessageBase::Pointer UnWrapMessage(igtl_uint8* messageContent, int totMsgLen);
    
    ///Set the synchronization source identifier, different session has different SSRC
    void SetSSRC(igtl_uint32 identifier);
    
    ///Set the Contributing source identifier. different device has different CSRC
    void SetCSRC(igtl_uint32 identifier);
    
    ///Set the sequencen number at the rtp header
    void SetSeqNum(igtl_uint16 num);
    
    ///Set the current msg header
    void SetMSGHeader(igtl_uint8* header);
    
    ///Get the wrapped outgoing UDP packet
    PacketBuffer GetOutGoingPackets(){return outgoingPackets;};
    
    ///Get the incomming UDP packet
    PacketBuffer GetInCommingPackets(){return incommingPackets;};
    
    int GetCurMSGLocation(){return this->curMSGLocation;};
    
    int GetPackedMSGLocation(){return this->curPackedMSGLocation;};
    
    int GetRTPWrapperStatus(){return status;};
    
    void SetRTPPayloadLength(unsigned int payloadLength){this->RTPPayloadLength = payloadLength;};
    
    unsigned int GetRTPPayloadLength(){return this->RTPPayloadLength;};
    
    std::map<igtl_uint32, igtl::UnWrappedMessage*> unWrappedMessages;
    
    igtl::SimpleMutexLock* glock;
    
    
  protected:
    MessageRTPWrapper();
    ~MessageRTPWrapper();
    
    /// Gets a pointer to the scalar data (for fragmented pack support).
    igtl_uint8* GetPackPointer(){return packedMsg;};
    
    int WrapMessage(igtl_uint8* messageContent, int bodyMsgLen);
    
    
  private:
    unsigned int RTPPayloadLength;
    igtl_uint8* packedMsg;
    igtl_uint8* MSGHeader;
    unsigned int curMSGLocation;
    unsigned int curPackedMSGLocation;
    unsigned int numberOfDataFrag;
    unsigned int numberOfDataFragToSent;
    igtl_uint8  RTPPayloadType;
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