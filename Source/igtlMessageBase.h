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

#include "igtlObject.h"
#include "igtlObjectFactory.h"
//#include "igtlMacros.h"
#include "igtlMacro.h"
#include "igtlMath.h"
#include "igtlTimeStamp.h"

#include "igtlMessageHeader.h"

#include <string>

namespace igtl
{

/// The MessageBase class is the base class of all message type classes
/// used in the Open IGT Link Library. The message classes can be used
/// both for serializing (packing) Open IGT Link message byte streams.
/// The class can also deserializing (unpacking) Open IGT Link.
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
///     transMsg->Pack();
///     socket->Send(transMsg->GetPackPointer(), transMsg->GetPackSize());
///
class IGTLCommon_EXPORT MessageBase: public Object
{
public:

  typedef MessageBase               Self;
  typedef Object                    Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::MessageBase, igtl::Object)
  igtlNewMacro(igtl::MessageBase);

  /// Unpack status. They are returned by the Unpack() function. 
  enum {
    UNPACK_UNDEF   = 0x0000,
    UNPACK_HEADER  = 0x0001,
    UNPACK_BODY    = 0x0002
  };

public:

  /// Sets the device (message) name.
  void  SetDeviceName(const char* name);

  /// Gets the device (message) name.
  const char* GetDeviceName();

  /// Gets the device (message) type.
  const char* GetDeviceType();

  /// Sets time of message creation. 'sec' and 'frac' are seconds and fractions of a second respectively.
  int   SetTimeStamp(unsigned int sec, unsigned int frac);

  /// Gets time of message creation. 'sec' and 'frac' are seconds and fractions of a second respectively.
  int   GetTimeStamp(unsigned int* sec, unsigned int* frac);

  /// Sets time of message creation.
  void  SetTimeStamp(igtl::TimeStamp::Pointer& ts);

  /// Gets time of message creation.
  void  GetTimeStamp(igtl::TimeStamp::Pointer& ts);

  /// Pack() serializes the header and body based on the member variables.
  /// PackBody() must be implemented in the child class.
  virtual int   Pack();

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
  int   Unpack(int crccheck = 0);

  /// Gets a pointer to the raw byte array for the serialized data including the header and the body.
  void* GetPackPointer();

  /// Gets a pointer to the raw byte array for the serialized body data.
  void* GetPackBodyPointer();

  /// Gets the size of the serialized data.
  int   GetPackSize();

  /// Gets the size of the serialized body data.
  int   GetPackBodySize();

  /// GetBodyType() gets the type of the body. 
  const char* GetBodyType() { return this->m_BodyType.c_str(); };

  /// AllocatePack() allocates memory for packing / receiving buffer
  void AllocatePack();
  
  /// Call InitPack() before receive header. 
  /// This function simply resets the Unpacked flag for both
  /// the header and body pack.
  void InitPack();

  /// Copy() copies contents from the specified Massage class.
  /// If the type of the specified class is the same as this class,
  /// both general header and body are copied. Otherwise, only
  /// general header is copied.
  int Copy(const MessageBase* mb);

  /// Sets the message header.
  virtual int SetMessageHeader(const MessageHeader* mb) { return Copy(mb); };

  /// GetBodySizeToRead() returns the size of the body to be read. This function must be called
  /// after the message header is set.
  int GetBodySizeToRead()                       { return m_BodySizeToRead; };
  
protected:
  MessageBase();
  ~MessageBase();

protected:

  /// Gets the size of the serialized body.
  virtual int  GetBodyPackSize() { return 0; };

  /// Packs (serialize) the body. Must be implemented in a child class. 
  virtual int  PackBody()        { return 0; };
  /// Unpacks (deserialize) the body. Must be implemented in a child class.
  virtual int  UnpackBody()      { return 0; };

  /// Allocates memory specifying the body size. This function is used when
  /// creating a brank package to receive data)
  virtual void AllocatePack(int bodySize);

  /// Copies a header from 
  int CopyHeader(const MessageBase *mb);

  /// Copies the serialized body data
  int CopyBody(const MessageBase *mb);

  int            m_PackSize;

  /// A pointer to the byte array for the serialized header. To prevent large
  /// copy of the byte array in the Pack() function, header byte array is
  /// concatinated to the byte array for the body. 
  unsigned char* m_Header;

  /// A pointer to the byte array for the serialized body. To prevent large
  /// copy of the byte array in the Pack() function, header byte array is
  /// concatinated to the byte array for the header. 
  unsigned char* m_Body;

  /// The size of the body to be read. This function must be called
  /// after the message header is set.
  int            m_BodySizeToRead;

  /// A character string for the default device type (message type).
  std::string    m_DefaultBodyType;

  /// A character string for the device type (message type). This will be used when the header
  /// is desrialized from a byte stream received from the network.
  std::string    m_BodyType;

  /// A character string for the device name (message name).
  std::string    m_DeviceName;

  /// A time stamp (second) for message creation. It consists of fields for seconds
  /// (m_TimeStampSec)and franctions of a second (m_TimeStampSecFraction).
  unsigned int   m_TimeStampSec;

  /// A time stamp (second) for message creation. It consists of fields for seconds
  /// (m_TimeStampSec)and franctions of a second (m_TimeStampSecFraction).
  unsigned int   m_TimeStampSecFraction;

  /// Unpacking (desrialization) status for the header (0: --   1: unpacked).
  int            m_IsHeaderUnpacked;

  /// Unpacking (desrialization) status for the body (0: --   1: unpacked).
  int            m_IsBodyUnpacked;

};


/// A class for header-only message types, which are used for quearying.
class IGTLCommon_EXPORT HeaderOnlyMessageBase: public MessageBase
{
public:
  typedef HeaderOnlyMessageBase        Self;
  typedef MessageBase                  Superclass;
  typedef SmartPointer<Self>           Pointer;
  typedef SmartPointer<const Self>     ConstPointer;

  igtlTypeMacro(igtl::HeaderOnlyMessageBase, igtl::MessageBase);
  igtlNewMacro(igtl::HeaderOnlyMessageBase);

protected:
  HeaderOnlyMessageBase()  { this->m_DefaultBodyType  = ""; };
  ~HeaderOnlyMessageBase() {};
  
protected:

  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };

};


/// A macro to help defining a class for query message types
/// that do not have message bodies. 
//  TODO: Need test.
#define igtlCreateDefaultQueryMessageClass(name, msgtype) \
class IGTLCommon_EXPORT name : public  HeaderOnlyMessageBase\
{ \
public: \
  typedef name                           Self; \
  typedef HeaderOnlyMessageBase          Superclass; \
  typedef SmartPointer<Self>             Pointer; \
  typedef SmartPointer<const Self>       ConstPointer; \
  \
  igtlTypeMacro(igtl::name, igtl::HeaderOnlyMessageBase); \
  igtlNewMacro(igtl::name); \
  \
protected: \
  name() : HeaderOnlyMessageBase() { this->m_DefaultBodyType  = msgtype; }; \
  ~name() {}; \
}; 

} // namespace igtl

#endif // _igtlMessageBase_h


