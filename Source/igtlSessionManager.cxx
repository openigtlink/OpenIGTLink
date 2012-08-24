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

namespace igtl
{
  

SessionManager::SessionManager()
{
  this->m_MessageHandlerList.clear();
  this->m_Mode = MODE_SERVER;

  this->m_Header = igtl::MessageHeader::New();
  this->m_TimeStamp = igtl::TimeStamp::New();
}


SessionManager::~SessionManager()
{
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
      clientSocket->SetReceiveTimeout(0);
      this->m_Socket = clientSocket;
      return 1;
      }
    //igtl::Sleep(1000);
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

    //std::cerr << this->GetClassName() << ": WaitForConnection(): Port number # = " << this->m_Port << std::endl;
    if (serverSocket.IsNotNull())
      {
      //this->ServerSocket->CreateServer(this->m_Port);
      this->m_Socket = serverSocket->WaitForConnection(10000);
      }
    
    if (this->m_Socket.IsNotNull() && this->m_Socket->GetConnected()) // if client connected
      {
      this->m_Socket->DebugOff();
      return 1;
      }
    return 0;
    }

  return 0;

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
  // Initialize receive buffer
  this->m_Header->InitPack();
  
  // Receive generic header from the socket
  int r = this->m_Socket->Receive(this->m_Header->GetPackPointer(), this->m_Header->GetPackSize());
  if (r == 0)
    {
    return 0; // Disconnected
    }
  if (r != this->m_Header->GetPackSize())
    {
    return -1;
    }
  
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
    if (strcmp(this->m_Header->GetDeviceType(), (*iter)->GetMessageType()) == 0)
      {
      (*iter)->ReceiveMessage(this->m_Socket, this->m_Header);
      //iter = this->m_MessageHandlerList.end();
      found = 1;
      break;
      }
    }

  // If there is no message handler, skip the message
  if (!found)
    {
    std::cerr << "Receiving : " << this->m_Header->GetDeviceType() << std::endl;
    this->m_Socket->Skip(this->m_Header->GetBodySizeToRead(), 0);
    }

  return 1;

}  
  

int SessionManager::AddMessageHandler(MessageHandler* handler)
{
  // Check if there is any handler for the same message type
  std::vector< MessageHandler* >::iterator iter;
  for (iter = this->m_MessageHandlerList.begin(); iter != this->m_MessageHandlerList.end(); iter ++)
    {
    if (strcmp((*iter)->GetMessageType(), handler->GetMessageType()) == 0)
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


}


