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

#define igtlMessageHandlerClassMacro(messagetype, classname, datatype)     \
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
    virtual int Process(messagetype*, datatype*);                 \
    int ReceiveMessage(::igtl::Socket* socket, ::igtl::MessageBase* header, int pos) \
    {                                                                   \
      if (pos == 0) /* New body */                                      \
        {                                                               \
        this->m_Message->SetMessageHeader(header);                      \
        this->m_Message->AllocatePack();                                \
        }                                                               \
      int s = socket->Receive((void*)((char*)this->m_Message->GetPackBodyPointer()+pos), \
                              this->m_Message->GetPackBodySize()-pos);  \
      if (s < 0) /* Time out */                                         \
        {                                                               \
        return pos;                                                     \
        }                                                               \
      if (s+pos >= this->m_Message->GetPackBodySize())                  \
        {                                                               \
        int r = this->m_Message->Unpack(this->m_CheckCRC);              \
        if (r)                                                          \
          {                                                             \
          Process(this->m_Message, this->m_Data);                       \
          }                                                             \
        else                                                            \
          {                                                             \
          return -1;                                                    \
          }                                                             \
        }                                                               \
      return s + pos;  /* return current position in the body */        \
    }                                                                   \
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
    void SetData(datatype* p)                                     \
    {                                                             \
      this->m_Data = p;                                           \
    }                                                             \
    datatype* GetData()                                           \
    {                                                             \
      return this->m_Data;                                        \
    }                                                             \
  protected:                                                      \
    classname()                                                   \
    {                                                             \
      this->m_Message  = messagetype::New();                      \
      this->m_CheckCRC = 1;                                       \
      this->m_Data = NULL;                                        \
    }                                                             \
    ~classname() {}                                               \
  protected:                                                      \
    int         m_CheckCRC;                                       \
    messagetype::Pointer m_Message;                               \
    datatype*   m_Data;                                           \
  }; 

#endif // __igtlMessageHandlerMacro_h
