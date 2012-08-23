/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    git@github.com:openigtlink/OpenIGTLink.git
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlMessageHandler_txx
#define __igtlMessageHandler_txx

#include "igtlMessageHandler.h"

namespace igtl
{

template <class MessageType>
MessageHandler<MessageType>::MessageHandler()
{
  this->m_Message  = MessageType::New();
  this->m_CheckCRC = 1;
}

template <class MessageType>
MessageHandler<MessageType>::~MessageHandler()
{
}

template <class MessageType>
const char* MessageHandler<MessageType>::GetMessageType()
{
  return this->m_Message->GetBodyType();
}
  
template <class MessageType>
int MessageHandler<MessageType>::ReceiveMessage(Socket* socket, MessageBase* header)
{
  this->m_Message->SetMessageHeader(header);
  this->m_Message->AllocatePack();
  socket->Receive(this->m_Message->GetPackBodyPointer(), this->m_Message->GetPackBodySize());
  int r = this->m_Message->Unpack(this->m_CheckCRC);
  if (r)
    {
    Process(this->m_Message);
    return 1;
    }
  else
    {
    return 0;
    }
}
  
template <class MessageType>
void MessageHandler<MessageType>::CheckCRC(int i)
{
  if (i == 0)
    {
    this->m_CheckCRC = 0;
    }
  else
    {
    this->m_CheckCRC = 1;
    }
}

}



#endif //__igtlMessageHandler_txx
