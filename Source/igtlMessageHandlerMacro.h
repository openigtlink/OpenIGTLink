/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    git@github.com:openigtlink/OpenIGTLink.git
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlMessageHandlerMacro_h
#define __igtlMessageHandlerMacro_h

#include "igtlMessageHandler.h"

// Description:
// The igtlMessageHandlerClassMacro() macro is to help developers to
// define message handler class. It generates a chlid class of igtl::MessageHandler. 
// The developer only needs to implement ProcessMessage() after calling this macro.
// The following code shows how to define a handler that processes IMAGE message:
//
//   igtlMessageHandlerClassMacro(igtl::ImageMessage, TestImageMessageHandler);
//   void TestImageMessageHandler::Process(igtl::ImageMessage * message)
//   {
//     // do something
//   }

//#define igtlMessageHandlerClassMacro(messagetype, classname)      \
//  template <typename messagetype>                                 \
//  class classname : public MessageHandler<messagetype>            \
//  {                                                               \
//    typedef classname                      Self;                  \
//    typedef MessageHandler<messagetype>    Superclass;            \
//    typedef SmartPointer<Self>             Pointer;               \
//    typedef SmartPointer<const Self>       ConstPointer;          \
//    igtlTypeMacro(classname, MessageHandler<messagetype>);        \
//    igtlNewMacro(classname);                                      \
//  public:                                                         \
//    virtual void Process(messagetype*);                           \
//  }; 

#define igtlMessageHandlerClassMacro(messagetype, classname)      \
  class classname : public ::igtl::MessageHandler                 \
  {                                                               \
  public:                                                         \
    typedef classname                      Self;                  \
    typedef ::igtl::MessageHandler    Superclass;                 \
    typedef igtl::SmartPointer<Self>             Pointer;         \
    typedef igtl::SmartPointer<const Self>       ConstPointer;    \
    igtlTypeMacro(classname, ::igtl::MessageHandler);             \
    igtlNewMacro(classname);                                      \
  public:                                                         \
    virtual const char* GetMessageType()                          \
    {                                                             \
      return this->m_Message->GetDeviceType();                    \
    }                                                             \
    virtual int Process(messagetype*);                            \
    int ReceiveMessage(::igtl::Socket* socket, ::igtl::MessageBase* header) \
    {                                                             \
      this->m_Message->SetMessageHeader(header);                  \
      this->m_Message->AllocatePack();                            \
      socket->Receive(this->m_Message->GetPackBodyPointer(),      \
                      this->m_Message->GetPackBodySize());        \
      int r = this->m_Message->Unpack(this->m_CheckCRC);          \
      if (r)                                                      \
        {                                                         \
        Process(this->m_Message);                                 \
        return 1;                                                 \
        }                                                         \
      else                                                        \
        {                                                         \
        return 0;                                                 \
        }                                                         \
    }                                                             \
    virtual void CheckCRC(int i)                                  \
    {                                                             \
      if (i == 0)                                                 \
        {                                                         \
        this->m_CheckCRC = 0;                                     \
        }                                                         \
      else                                                        \
        {                                                         \
        this->m_CheckCRC = 1;                                     \
        }                                                         \
    }                                                             \
  protected:                                                      \
    classname()                                                   \
    {                                                             \
      this->m_Message  = messagetype::New();                      \
      this->m_CheckCRC = 1;                                       \
    }                                                             \
    ~classname() {}                                               \
  protected:                                                      \
    int         m_CheckCRC;                                       \
    messagetype::Pointer m_Message;                               \
  }; 

#endif // __igtlMessageHandlerMacro_h
