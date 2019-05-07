/*=========================================================================

 Program:   The OpenIGTLink Library
 Language:  C++
 Web page:  http://openigtlink.org/

 Copyright (c) Insight Software Consortium. All rights reserved.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.

 =========================================================================*/

#ifndef __igtlMessageBase_h
#define __igtlMessageBase_h

#include "igtlMacro.h"
#include "igtlMath.h"
#include "igtlMessageHeader.h"
#include "igtlObject.h"
#include "igtlObjectFactory.h"
#include "igtlTimeStamp.h"
#include "igtl_header.h"
#include "igtl_types.h"

#include <string>
#include <map>
#include <vector>

namespace igtl
{

  /// The MessageBase class is the base class of all message type classes
  /// used in the OpenIGTLink Library. The message classes can be used
  /// both for serializing (packing) OpenIGTLink message byte streams.
  /// The class can also deserializing (unpacking) OpenIGTLink.
  /// For the deserialization example, please refer igtlMessageHeader.h.
  ///
  /// The typical packing procedures using sub-classes of
  /// MessageBase look like the followings
  ///
  ///     // Create instance and set Device Name
  ///     igtl::TransformMessage::Pointer transMsg;
  ///     transMsg = igtl::TransformMessage::New();
  ///     transMsg->SetDeviceName("Tracker");
  ///
  ///     // Create matrix and substitute values
  ///     igtl::Matrix4x4 matrix;
  ///     GetRandomTestMatrix(matrix);
  ///
  ///     // Set matrix data, serialize, and send it.
  ///     transMsg->SetMatrix(matrix);
  ///     transMsg->AllocatePack(); // optional
  ///     transMsg->Pack();
  ///     socket->Send(transMsg->GetBufferPointer(), transMsg->GetBufferSize());
  ///
  ///     V1/V2 message structure:
  ///                                    GetBodySize()
  ///               /-------------------------/\----------------------------------------------------------\
  ///                                    GetPackContentSize() (subclassed)
  ///               /-------------------------------------/\----------------------------------------------\
  ///  |____________|_____________________________________________________________________________________|
  ///  m_Header     m_Body
  ///
  ///
  ///     V3 message structure:
  ///                                      GetBodySize()
  ///               /-------------------------/\------------------------------------------------------------\
  ///                                          GetPackContentSize() (subclassed)
  ///                                             /\                (sending after setters are called, receiving after extended header has been parsed)
  ///                                   /--------/  \-----------\
  ///  |____________|___________________|________________________|___________________|_______________________|
  ///  m_Header     m_ExtendedHeader    m_Content (old m_Body)   m_MetaDataHeader    m_MetaData
  ///               m_Body
  ///

  class IGTLCommon_EXPORT MessageBase: public Object
  {
  public:

#if OpenIGTLink_HEADER_VERSION >= 2
    // Types for managing meta data
    typedef std::map<std::string, std::pair<IANA_ENCODING_TYPE, std::string> > MetaDataMap;
#endif

    igtlTypeMacro(MessageBase, Object)
    igtlNewMacro(igtl::MessageBase);

    /// Unpack status. They are returned by the Unpack() function.
    enum
    {
      UNPACK_UNDEF   = 0x0000,
      UNPACK_HEADER  = 0x0001,
      UNPACK_BODY    = 0x0002
    };

  public:
    /// Create a clone of this message, new memory but all internals are preserved
    virtual igtl::MessageBase::Pointer Clone();

    /// Sets the message version number
    void SetHeaderVersion(unsigned short headerVersion);

    /// Gets the message version number
    unsigned short GetHeaderVersion() const;

    /// Sets the device name.
    void SetDeviceName(const char* name);

    /// Sets the device name.
    void SetDeviceName(const std::string& name);

    /// Sets the device (message) type.
    void SetDeviceType(const std::string& type);

    /// Gets the device name.
    const char* GetDeviceName();

    /// Gets the device name.
    std::string GetDeviceName() const;

    /// Gets the device (message) type.
    const char* GetDeviceType();

#if OpenIGTLink_HEADER_VERSION >= 2
    /// Gets the size (length) of the byte array for the meta data.
    /// The size is defined by the length of each meta data elements and the total number of
    /// the meta data element.
    igtlUint32 GetMetaDataSize();

    /// Gets the size (length) of the byte array for the meta data header.
    igtlUint16 GetMetaDataHeaderSize();

    /// Gets the message ID
    igtlUint32 GetMessageID();

    /// Sets the message ID
    void SetMessageID(igtlUint32 idValue);

    /// Add Meta data element
    bool SetMetaDataElement(const std::string& key, IANA_ENCODING_TYPE encodingScheme, std::string value);
    bool SetMetaDataElement(const std::string& key, igtl_uint8);
    bool SetMetaDataElement(const std::string& key, igtl_int8);
    bool SetMetaDataElement(const std::string& key, igtl_uint16);
    bool SetMetaDataElement(const std::string& key, igtl_int16);
    bool SetMetaDataElement(const std::string& key, igtl_uint32);
    bool SetMetaDataElement(const std::string& key, igtl_int32);
    bool SetMetaDataElement(const std::string& key, igtl_uint64);
    bool SetMetaDataElement(const std::string& key, igtl_int64);
    bool SetMetaDataElement(const std::string& key, float);
    bool SetMetaDataElement(const std::string& key, double);

    /// Get meta data element
    bool GetMetaDataElement(const std::string& key, std::string& value) const;
    bool GetMetaDataElement(const std::string& key, IANA_ENCODING_TYPE& encoding, std::string& value) const;

    /// Get meta data map
    const MetaDataMap& GetMetaData() const;

    /// Pack the extended header
    bool PackExtendedHeader();

    /// Pack the meta data
    bool PackMetaData();

    /// Unpack Extended header
    /// When receiving a message, after the base header is unpacked, the extended header is read
    /// Once read, the m_MetaDataHeader and m_MetaData members are populated as we now know the message structure
    bool UnpackExtendedHeader();

    /// Unpack Extended header and the meta data
    bool UnpackMetaData();
#endif

    /// Sets time of message creation. 'sec' and 'frac' are seconds and fractions of a second respectively.
    int SetTimeStamp(unsigned int sec, unsigned int frac);

    /// Gets time of message creation. 'sec' and 'frac' are seconds and fractions of a second respectively.
    int GetTimeStamp(unsigned int* sec, unsigned int* frac);

    /// Sets time of message creation.
    void SetTimeStamp(igtl::TimeStamp::Pointer& ts);

    /// Gets time of message creation.
    void GetTimeStamp(igtl::TimeStamp::Pointer& ts);

    /// Pack() serializes the header and body based on the member variables.
    /// PackContent() must be implemented in the child class.
    virtual int Pack();

    /// Unpack() deserializes the header and/or body, extracting data from
    /// the byte stream. If the header has already been deserilized, Unpack()
    /// deserializes only the body part. UnpackBody() must be implemented to
    /// deserialize the body part. Unpack() performs 64-bit CRC check,
    /// when crccheck = 1. It returns:
    ///
    ///   UNPACK_UNDEF             : Nothing deserialized
    ///   UNPACK_HEADER            : The header has been deserialized.
    ///   UNPACK_BODY              : The body has been deserialized.
    ///                              If CRC check fails, Unpack() doesn't
    ///                              deserialize the body, thus it doesn't
    ///                              return UNPACK_BODY flag.
    ///   UNPACK_HEADER|UNPACK_BODY: Both the header and body have been
    ///                              deserialized
    int Unpack(int crccheck = 0);

    /// Gets a pointer to the raw byte array for the serialized data including the header and the body.
    void* GetBufferPointer();
    void* GetPackPointer() { return GetBufferPointer(); }

    /// Gets a pointer to the raw byte array for the serialized body data.
    void* GetBufferBodyPointer();
    void* GetPackBodyPointer() { return GetBufferBodyPointer(); }

    /// Gets the size of the serialized message data.
    int GetBufferSize();
    int GetPackSize() { return GetBufferSize(); }

    /// Gets the size of the serialized body data.
    int GetBufferBodySize();
    int GetPackBodySize() { return GetBufferBodySize(); }

    /// Calculate the size of the received content data
    /// Returns -1 if the extended header has not been properly initialized (meta data size, meta data header size, etc...)
    /// Used when receiving data, not sending
    int CalculateReceiveContentSize();

    /// Gets the type of the body.
    const char* GetBodyType();

    /// Gets the message type.
    std::string GetMessageType() const;

    /// AllocatePack() allocates memory for underlying buffer
    /// If m_BodySizeToRead > 0, we are allocating for receiving a message
    void AllocateBuffer();
    void AllocatePack() { AllocateBuffer(); }

    /// Call InitPack() before receive header.
    /// This function simply resets the Unpacked flag for both
    /// the header and body pack
    /// Only allocate the original 58 byte header
    void InitBuffer();
    void InitPack() { InitBuffer(); }

    /// Copy() copies contents from the specified Massage class.
    /// If the type of the specified class is the same as this class,
    /// general header and body are copied.
    int Copy(const MessageBase* mb);

    /// Sets the message header.
    virtual int SetMessageHeader(const MessageHeader* mb);

    /// GetBodySizeToRead() returns the size of the body to be read. This function must be called
    /// after the message header is set.
    int GetBodySizeToRead();

  protected:
    MessageBase();
    ~MessageBase();

  protected:
    /// Gets the size of the serialized content.
    virtual int CalculateContentBufferSize();

    /// Packs (serialize) the content. Must be implemented in all child classes.
    virtual int PackContent();
    /// Unpacks (deserialize) the content. Must be implemented in all child classes.
    virtual int UnpackContent();

    /// Allocates memory specifying the content size.
    /// Implicitly allocates extended header, metadata header and metadata in addition to content
    virtual void AllocateBuffer(int contentSize);

    /// Allocates memory specifying the unpack content size
    /// Size of body to allocate is determined from v1 message header field 'body_size'
    virtual void AllocateUnpack(int bodySizeToRead);

    /// Copies the serialized body data
    int CopyBody(const MessageBase* mb);

    /// Copies a header from given message
    int CopyHeader(const MessageBase* mb);

    /// Unpack the first 58 bytes
    void UnpackHeader(int& r);

    /// Unpack the body
    /// If it's a v3 message, body is ext header + content + metadataheader + metadata<optional>
    void UnpackBody(int crccheck, int& r);

  protected:
    int            m_MessageSize;

    /// A pointer to the byte array for the serialized header. To prevent large
    /// copy of the byte array in the Pack() function, header byte array is
    /// concatenated to the byte array for the body.
    unsigned char* m_Header;

    /// A pointer to the byte array for the serialized body. To prevent large
    /// copy of the byte array in the Pack() function, header byte array is
    /// concatenated to the byte array for the header.
    unsigned char* m_Body;

    /// A pointer to the underlying content of a message
    unsigned char* m_Content;

    /// The size of the body to be read. This function must be called
    /// after the message header is set.
    int            m_BodySizeToRead;

    /// A character string for the send device type (message type).
    std::string    m_SendMessageType;

    /// A character string for the device type (message type). This will be used when the header
    /// is deserialized from a byte stream received from the network.
    std::string    m_ReceiveMessageType;

    /// An unsigned short for the message format version
    unsigned short m_HeaderVersion;

    /// A character string for the device name (message name).
    mutable std::string m_DeviceName;

    /// A time stamp (second) for message creation. It consists of fields for seconds
    /// (m_TimeStampSec)and fractions of a second (m_TimeStampSecFraction).
    unsigned int   m_TimeStampSec;

    /// A time stamp (second) for message creation. It consists of fields for seconds
    /// (m_TimeStampSec)and fractions of a second (m_TimeStampSecFraction).
    unsigned int   m_TimeStampSecFraction;

    /// Unpacking (deserialization) status for the header
    bool           m_IsHeaderUnpacked;

    /// Unpacking (deserialization) status for the body
    bool           m_IsBodyUnpacked;

    /// Packing (serialization) status for the body
    bool           m_IsBodyPacked;

#if OpenIGTLink_HEADER_VERSION >= 2
  protected:
    /// A pointer to the serialized extended header.
    unsigned char*                                                            m_ExtendedHeader;

    /// A flag to record the unpacked state of the extended header
    bool                                                                      m_IsExtendedHeaderUnpacked;

    /// A pointer to the meta data.
    unsigned char*                                                            m_MetaData;

    /// A pointer to the meta data header.
    unsigned char*                                                            m_MetaDataHeader;

    /// Message ID
    igtlUint32                                                                m_MessageId;

    /// Map storing the key value pairs
    MetaDataMap                                                               m_MetaDataMap;

#endif // if OpenIGTLink_HEADER_VERSION >= 2

  };

  /// A class for header-only message types, which are used for querying.
  class IGTLCommon_EXPORT HeaderOnlyMessageBase: public MessageBase
  {
  public:
    igtlTypeMacro(igtl::HeaderOnlyMessageBase, igtl::MessageBase);
    igtlNewMacro(igtl::HeaderOnlyMessageBase);

  protected:
    HeaderOnlyMessageBase()
    {
      this->m_SendMessageType  = "";
    };
    ~HeaderOnlyMessageBase() {};

  protected:
    virtual int  CalculateContentBufferSize()
    {
      return 0;
    };
    virtual int  PackContent()
    {
      AllocateBuffer();
      return 1;
    };
    virtual int  UnpackContent()
    {
      return 1;
    };
  };

  /// A macro to help defining a class for query message types
  /// that do not have message bodies.
  //  TODO: Need test.
#define igtlCreateDefaultQueryMessageClass(name, msgtype) \
class IGTLCommon_EXPORT name : public  HeaderOnlyMessageBase\
{ \
public: \
igtlTypeMacro(igtl::name, igtl::HeaderOnlyMessageBase); \
igtlNewMacro(igtl::name); \
\
protected: \
name() : HeaderOnlyMessageBase() { this->m_SendMessageType  = msgtype; }; \
~name() {}; \
};

} // namespace igtl

#endif // _igtlMessageBase_h


