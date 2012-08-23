/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    git@github.com:openigtlink/OpenIGTLink.git
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlMessageHandler_h
#define __igtlMessageHandler_h

#include "igtlObject.h"
#include "igtlMacro.h"
#include "igtlSocket.h"
#include "igtlMessageBase.h"

namespace igtl
{

class IGTLCommon_EXPORT MessageHandlerInterface: public Object
{
 public:

  typedef MessageHandlerInterface   Self;
  typedef Object                    Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::MessageHandlerInterface, igtl::Object)
  igtlNewMacro(igtl::MessageHandlerInterface);

 public:
  virtual const char* GetMessageType() { return ""; }
  virtual int ReceiveMessage(Socket*, MessageBase*) {};

 protected:
  MessageHandlerInterface();
  ~MessageHandlerInterface();

 protected:
};


template<typename MessageType>
class IGTLCommon_EXPORT MessageHandler: public MessageHandlerInterface
{
 public:

  typedef MessageHandler            Self;
  typedef MessageHandlerInterface   Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(MessageHandler, Object)
  igtlNewMacro(MessageHandler);

 public:
  virtual const char* GetMessageType();
  virtual int  ReceiveMessage(Socket*, MessageBase*);
  virtual int  Process(MessageType* message) {};
  virtual void CheckCRC(int i);

 protected:
  MessageHandler();
  ~MessageHandler();

 protected:
  int         m_CheckCRC;
  MessageType m_Message;

};


} // namespace igtl

#include "igtlMessageHandler.txx"

#endif // _igtlMessageHandler_h





