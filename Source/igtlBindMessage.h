/*=========================================================================

  Program:   The OpenIGTLink Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink2_beta/Source/igtlBindMessage.h $
  Language:  C++
  Date:      $Date: 2009-12-16 23:58:02 -0500 (Wed, 16 Dec 2009) $
  Version:   $Revision: 5466 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlBindMessage_h
#define __igtlBindMessage_h

#include <string>

#include "igtlObject.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

namespace igtl
{
//
// Base class for BindMessage, GetBindMessage and StartBindMessage
//
class IGTLCommon_EXPORT BindMessageBase: public MessageBase
{
public:
  typedef BindMessageBase                Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::BindMessageBase, igtl::MessageBase);
  igtlNewMacro(igtl::BindMessageBase);

public:
  
  void        Init();

  int         SetNumberOfChildMessages(unsigned int n);
  int         GetNumberOfChildMessages();

  int         AppendChildMessage(igtl::MessageBase * child);
  int         SetChildMessage(unsigned int i, igtl::MessageBase * child);

  const char* GetChildMessageType(unsigned int i);

protected:

  BindMessageBase();
  ~BindMessageBase();
  
protected:

  typedef struct {
    std::string  type;
    std::string  name;
    igtlUint64   size;
    void *       ptr;
  } ChildMessageInfo;

  std::vector<ChildMessageInfo> m_ChildMessages;
  
};


class IGTLCommon_EXPORT BindMessage: public BindMessageBase
{
public:
  typedef BindMessage                    Self;
  typedef BindMessageBase                Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::BindMessage, igtl::BindMessageBase);
  igtlNewMacro(igtl::BindMessage);

public:
  
  int         GetChildMessage(unsigned int i, igtl::MessageBase * child);

protected:
  BindMessage();
  ~BindMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

};


class IGTLCommon_EXPORT GetBindMessage: public BindMessageBase
{
public:
  typedef GetBindMessage               Self;
  typedef BindMessageBase              Superclass;
  typedef SmartPointer<Self>           Pointer;
  typedef SmartPointer<const Self>     ConstPointer;

  igtlTypeMacro(igtl::GetBindMessage, igtl::BindMessageBase);
  igtlNewMacro(igtl::GetBindMessage);

public:
  
  int          AppendChildMessage(const char * type, const char * name);

protected:
  GetBindMessage();
  ~GetBindMessage();
  
protected:
  
  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

};


class IGTLCommon_EXPORT StartBindMessage: public GetBindMessage
{
public:
  typedef StartBindMessage             Self;
  typedef GetBindMessage               Superclass;
  typedef SmartPointer<Self>           Pointer;
  typedef SmartPointer<const Self>     ConstPointer;

  igtlTypeMacro(igtl::StartBindMessage, igtl::GetBindMessage);
  igtlNewMacro(igtl::StartBindMessage);

public:

  // Set/get time resolution. The time resolution is specified
  // as a 64-bit fixed-point used in OpenIGTLink time stamp.
  void        SetResolution(igtlUint64 res);
  igtlUint64  GetResolution();

protected:
  StartBindMessage();
  ~StartBindMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

  igtlUint64   m_Resolution;

};


class IGTLCommon_EXPORT StopBindMessage: public MessageBase
{
public:
  typedef StopBindMessage                Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::StopBindMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StopBindMessage);

protected:
  StopBindMessage() : MessageBase() { this->m_DefaultBodyType  = "STP_BIND"; };
  ~StopBindMessage() {};

protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };

};


class IGTLCommon_EXPORT RTSBindMessage: public MessageBase
{
public:
  typedef RTSBindMessage         Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  // Status type
  enum {
    STATUS_SUCCESS = 0,
    STATUS_ERROR = 1
  };


  igtlTypeMacro(igtl::RTSBindMessage, igtl::MessageBase);
  igtlNewMacro(igtl::RTSBindMessage);

  void          SetStatus(igtlUint8 status){ this->m_Status = status; }
  igtlUint8     GetStatus()                { return this->m_Status; };

protected:
  RTSBindMessage() : MessageBase(), m_Status(0) { this->m_DefaultBodyType  = "RTS_BIND"; };
  ~RTSBindMessage() {};

  igtlUint8 m_Status;

protected:
  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

};



} // namespace igtl

#endif // _igtlBindMessage_h



