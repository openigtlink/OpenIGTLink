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

#define igtlMessageHandlerClassMacro(messagetype, classname)      \
  template <typename messagetype>                                 \
  class classname : public MessageHandler<messagetype>            \
  {                                                               \
    typedef classname                      Self;                  \
    typedef MessageHandler<messagetype>    Superclass;            \
    typedef SmartPointer<Self>             Pointer;               \
    typedef SmartPointer<const Self>       ConstPointer;          \
    igtlTypeMacro(classname, MessageHandler<messagetype>);        \
    igtlNewMacro(classname);                                      \
  public:                                                         \
    virtual void Process(messagetype);                            \
  };                                                              \

#endif // __igtlMessageHandlerMacro_h
