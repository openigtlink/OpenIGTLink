/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    git@github.com:openigtlink/OpenIGTLink.git
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <iomanip>
#include <math.h>
#include <cstdlib>
#include <cstring>

#include "igtlSessionManager.h"
#include "igtlMessageHandler.h"
#include "igtlClientSocket.h"
#include "igtlServerSocket.h"

#include "igtl_header.h"

namespace igtl
{
  

SessionManager::SessionManager()
{
  this->m_MessageHandlerList.clear();
  this->m_Mode = MODE_SERVER;

  this->m_Header = igtl::MessageHeader::New();
  this->m_TimeStamp = igtl::TimeStamp::New();

  this->m_CurrentReadIndex = 0;
  this->m_HeaderDeserialized = 0;
}


SessionManager::~SessionManager()
{
}


int SessionManager::AddMessageHandler(MessageHandler* handler)
{
  // Check if there is any handler for the same message type
  std::vector< MessageHandler* >::iterator iter;
  for (iter = this->m_MessageHandlerList.begin(); iter != this->m_MessageHandlerList.end(); iter ++)
    {
    if ( (*iter)->GetMessageType() == handler->GetMessageType() )
      {
      return 0;
      }
    }

  // If not, add the handler to the list.
  this->m_MessageHandlerList.push_back(handler);

  return 1;
}


int SessionManager::RemoveMessageHandler(MessageHandler* handler)
{
  // Check if there is any handler for the same message type
  std::vector< MessageHandler* >::iterator iter;
  for (iter = this->m_MessageHandlerList.begin(); iter != this->m_MessageHandlerList.end(); iter ++)
    {
    if (*iter == handler)
      {
      this->m_MessageHandlerList.erase(iter);
      return 1;
      }
    }
  return 0;
}


int SessionManager::Connect()
{
  
  this->m_Socket = NULL;

  if (this->m_Mode == MODE_CLIENT)
    {
    ClientSocket::Pointer clientSocket;
    clientSocket = ClientSocket::New();
    //this->DebugOff();
    if (this->m_Hostname.length() == 0)
      {
      return 0;
      }
    //this->m_Socket->SetConnectTimeout(1000);
    int r = clientSocket->ConnectToServer(this->m_Hostname.c_str(), this->m_Port);
    if (r == 0) // if connected to server
      {
      //clientSocket->SetReceiveTimeout(0);
      this->m_Socket = clientSocket;
      }
    else
      {
      return 0;
      }
    }
  else
    {
    ServerSocket::Pointer serverSocket;
    serverSocket = ServerSocket::New();
    int r = serverSocket->CreateServer(this->m_Port);
    if (r < 0)
      {
      return 0;
      }

    if (serverSocket.IsNotNull())
      {
      //this->ServerSocket->CreateServer(this->m_Port);
      this->m_Socket = serverSocket->WaitForConnection(10000);
      }
    
    if (this->m_Socket.IsNotNull() && this->m_Socket->GetConnected()) // if client connected
      {
      this->m_Socket->DebugOff();
      }
    else
      {
      return 0;
      }
    }

  this->m_Socket->SetReceiveBlocking(0); // Psuedo non-blocking
  this->m_CurrentReadIndex = 0;
  this->m_HeaderDeserialized = 0;
  return 1;
}


int SessionManager::Disconnect()
{
  if (this->m_Socket.IsNotNull())
    {
    this->m_Socket->CloseSocket();
    }

  return 0;
}

int SessionManager::ProcessMessage()
{
  // The workflow of this function is as follows:
  //
  //  HEADER:
  //   IF the message is (a) a new message:
  //       start reading the header;
  //   ELSE IF the message is a message in progress:
  //       if the process is reading the header:
  //           continue to read the header;
  //   ELSE:
  //       GOTO BODY;
  //   
  //   IF the header has not been received:
  //       GOTO BODY;
  //   ELSE
  //       RETURN;
  //   
  //  BODY:
  //   IF the process has not started reading the body:
  //       check the body type;
  //       find an appropriate handler;
  //       start reading the body
  //   ELSE:
  //       continue to read the body
  //

  //--------------------------------------------------
  // Header
  if (this->m_CurrentReadIndex == 0)
    {
    // Initialize receive buffer
    this->m_Header->InitBuffer();
  
    // Receive generic header from the socket
    int r = this->m_Socket->Receive(this->m_Header->GetBufferPointer(), this->m_Header->GetBufferSize(), 0);
    if (r == 0)
      {
      this->m_CurrentReadIndex = 0;
      this->m_HeaderDeserialized = 0;
      return 0; // Disconnected
      }
    if (r != this->m_Header->GetBufferSize())
      {
      // Only a part of header has arrived.
      if (r < 0) // timeout
        {
        this->m_CurrentReadIndex = 0;
        }
      else
        {
        this->m_CurrentReadIndex = r;
        }
      return -1;
      }
    // The header has been received.
    this->m_CurrentReadIndex = IGTL_HEADER_SIZE;
    }
  else if (this->m_CurrentReadIndex < IGTL_HEADER_SIZE)
    {
    // Message transfer was interrupted in the header
    int r = this->m_Socket->Receive((void*)((char*)this->m_Header->GetBufferPointer()+this->m_CurrentReadIndex),
                                    this->m_Header->GetBufferSize()-this->m_CurrentReadIndex, 0);
    if (r == 0)
      {
      this->m_CurrentReadIndex = 0;
      this->m_HeaderDeserialized = 0;
      return 0; // Disconnected
      }
    if (r != this->m_Header->GetBufferSize()-this->m_CurrentReadIndex)
      {
      // Only a part of header has arrived.
      if (r > 0) // exclude a case of timeout 
        {
        this->m_CurrentReadIndex += r;
        }
      return -1;
      }
    // The header has been received.
    this->m_CurrentReadIndex = IGTL_HEADER_SIZE;
    }
  

  //--------------------------------------------------
  // Body
  if (this->m_HeaderDeserialized == 0)
    {
    // Deserialize the header
    this->m_Header->Unpack();
    
    // Get time stamp
    igtlUint32 sec;
    igtlUint32 nanosec;
   
    this->m_Header->GetTimeStamp(this->m_TimeStamp);
    this->m_TimeStamp->GetTimeStamp(&sec, &nanosec);
    //std::cerr << "Time stamp: "
    //          << sec << "." << std::setw(9) << std::setfill('0') 
    //          << nanosec << std::endl;

    // Look for a message handler that matches to the message type.
    int found = 0;
    std::vector< MessageHandler* >::iterator iter;
    for (iter = this->m_MessageHandlerList.begin(); iter != this->m_MessageHandlerList.end(); iter ++)
      {
#if OpenIGTLink_HEADER_VERSION >= 2
      if ( this->m_Header->GetMessageType() == (*iter)->GetMessageType() )
#else
      if (strcmp(this->m_Header->GetDeviceType(), (*iter)->GetMessageType()) == 0)
#endif
        {
        this->m_CurrentMessageHandler = *iter;
        found = 1;
        break;
        }
      }
      
    // If there is no message handler, skip the message
    if (!found)
      {
#if OpenIGTLink_HEADER_VERSION >= 2
      std::cerr << "Receiving: " << this->m_Header->GetMessageType() << std::endl;
#else
      std::cerr << "Receiving: " << this->m_Header->GetDeviceType() << std::endl;
#endif
      this->m_Socket->Skip(this->m_Header->GetBodySizeToRead(), 0);
      // Reset the index counter to be ready for the next message
      this->m_CurrentReadIndex = 0;
      this->m_HeaderDeserialized = 0;
      return 1;
      }

    this->m_HeaderDeserialized = 1;
    }

  int r = this->m_CurrentMessageHandler->ReceiveMessage(this->m_Socket, this->m_Header,
                                                        this->m_CurrentReadIndex-IGTL_HEADER_SIZE);
  if (r == this->m_Header->GetBodySizeToRead())
    {
    this->m_CurrentReadIndex = 0;
    this->m_HeaderDeserialized = 0;
    }
  else
    {
    this->m_CurrentReadIndex += IGTL_HEADER_SIZE + r;
    }

  return 1;
}  
  

int SessionManager::PushMessage(MessageBase* message)
{
  
  if (message && this->m_Socket.IsNotNull() && this->m_Socket->GetConnected()) // if client connected
    {
    return this->m_Socket->Send(message->GetBufferPointer(), message->GetBufferSize());
    }
  else
    {
    return 0;
    }
}


}


