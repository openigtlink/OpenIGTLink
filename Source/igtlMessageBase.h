/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlMessageBase.h $
  Language:  C++
  Date:      $Date: 2009-01-13 14:11:16 -0500 (Tue, 13 Jan 2009) $
  Version:   $Revision: 3544 $

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

//-------------------------------------------------------------------------
// The MessageBase class is the base class of all message type classes
// used in the Open IGT Link Library. The message classes can be used
// both for serializing (packing) Open IGT Link message byte streams.
// The class can also deserializing (unpacking) Open IGT Link.
// For the deserialization example, please refer igtlMessageHeader.h.
// 
// The typical packing procedures using sub-classes of
// MessageBase look like the followings
//
//     // Create instance and set Device Name
//     igtl::TransformMessage::Pointer transMsg;
//     transMsg = igtl::TransformMessage::New();
//     transMsg->SetDeviceName("Tracker");
//
//     // Create matrix and substitute values
//     igtl::Matrix4x4 matrix;
//     GetRandomTestMatrix(matrix);
//
//     // Set matrix data, serialize, and send it.
//     transMsg->SetMatrix(matrix);
//     transMsg->Pack();
//     socket->Send(transMsg->GetPackPointer(), transMsg->GetPackSize());
//
//-------------------------------------------------------------------------

namespace igtl
{

class IGTLCommon_EXPORT MessageBase: public Object
{
public:

  typedef MessageBase               Self;
  typedef Object                    Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::MessageBase, igtl::Object)
  igtlNewMacro(igtl::MessageBase);

  enum {
    UNPACK_UNDEF   = 0x0000,
    UNPACK_HEADER  = 0x0001,
    UNPACK_BODY    = 0x0002
  };

public:

  void  SetDeviceName(const char* name);
  const char* GetDeviceName();
  const char* GetDeviceType();
  
  int   SetTimeStamp(unsigned int sec, unsigned int frac);
  int   GetTimeStamp(unsigned int* sec, unsigned int* frac);

  void  SetTimeStamp(igtl::TimeStamp::Pointer& ts);
  void  GetTimeStamp(igtl::TimeStamp::Pointer& ts);

  // Pack() serializes the header and body based on the member variables.
  // PackBody() must be implemented in the child class.
  int   Pack();

  // Unpack() deserializes the header and/or body, extracting data from
  // the byte stream. If the header has already been deserilized, Unpack()
  // deserializes only the body part. UnpackBody() must be implemented to
  // deserialize the body part. Unpack() performs 64-bit CRC check,
  // when crccheck = 1. It returns:
  //      UNPACK_UNDEF             : Nothing deserialized
  //      UNPACK_HEADER            : The header has been deserialized.
  //      UNPACK_BODY              : The body has been deserialized.
  //                                 If CRC check fails, Unpack() doesn't
  //                                 deserialize the body, thus it doesn't
  //                                 return UNPACK_BODY flag.
  //      UNPACK_HEADER|UNPACK_BODY: Both the header and body have been
  //                                 deserialized
  int   Unpack(int crccheck = 0);

  void* GetPackPointer();
  void* GetPackBodyPointer();
  int   GetPackSize();
  int   GetPackBodySize();

  const char* GetBodyType() { return this->m_BodyType.c_str(); };

  // Allocate memory for packing / receiving buffer
  void AllocatePack();

  // Call InitPack() before receive header. 
  // This function simply resets the Unpacked flag for both
  // the header and body pack.
  void InitPack();

  // Copy contents from the specified Massage class.
  // If the type of the specified class is the same as this class,
  // both general header and body are copied. Otherwise, only
  // general header is copied.
  int Copy(const MessageBase* mb);

  virtual int SetMessageHeader(const MessageHeader* mb) { return Copy(mb); };
  int GetBodySizeToRead()                       { return m_BodySizeToRead; };
  
protected:
  MessageBase();
  ~MessageBase();

protected:

  // Pack body (must be implemented in a child class)
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { return 0; };
  virtual int  UnpackBody()      { return 0; };

  // Allocate memory specifying the body size
  // (used when create a brank package to receive data)
  void AllocatePack(int bodySize);

  // Copy data 
  int CopyHeader(const MessageBase *mb);
  int CopyBody(const MessageBase *mb);

  // Pointers to header and image
  //  To prevent large copy of byte array in Pack() function,
  //  header byte array is concatinated to that of image.
  //  Consequently,
  //    body = header + sizeof (igtl_header)
  //  after these areas are allocated.
  //
  int            m_PackSize;
  unsigned char* m_Header;
  unsigned char* m_Body;

  int            m_BodySizeToRead;

  //BTX
  std::string    m_DefaultBodyType;
  std::string    m_BodyType;
  std::string    m_DeviceName;
  //ETX
  unsigned int   m_TimeStampSec;
  unsigned int   m_TimeStampSecFraction;

  // Unpacking status (0: --   1: unpacked)
  int            m_IsHeaderUnpacked;
  int            m_IsBodyUnpacked;

};


} // namespace igtl

#endif // _igtlMessageBase_h


