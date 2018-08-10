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

class IGTLCommon_EXPORT MessageHandler: public Object
{
 public:
  igtlTypeMacro(igtl::MessageHandler, igtl::Object)
  igtlNewMacro(igtl::MessageHandler);

 public:
  virtual const char* GetMessageType() { return ""; }
#if OpenIGTLink_HEADER_VERSION >= 2
  virtual std::string GetMessageType() const { return std::string(""); }
#endif
  virtual int ReceiveMessage(Socket*, MessageBase*, int) { return 0; };

  void SetMessageBuffer(MessageBase* buffer) { this->m_Buffer = buffer; }
  MessageBase * GetMessageBuffer() { return this->m_Buffer; }

 protected:
  MessageHandler() {}
  ~MessageHandler() {}

  

 protected:
  MessageBase * m_Buffer;

};

} // namespace igtl

#endif // _igtlMessageHandler_h





