/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkServerSocket.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "igtlServerSocket.h"
#include "igtlClientSocket.h"

namespace igtl
{

//-----------------------------------------------------------------------------
ServerSocket::ServerSocket()
{
}

//-----------------------------------------------------------------------------
ServerSocket::~ServerSocket()
{
}

//-----------------------------------------------------------------------------
int ServerSocket::GetServerPort()
{
  if (!this->GetConnected())
    {
    return 0;
    }
  return this->GetPort(this->m_SocketDescriptor);
}

//-----------------------------------------------------------------------------
int ServerSocket::CreateServer(int port)
{
  if (this->m_SocketDescriptor != -1)
    {
    igtlWarningMacro("Server Socket already exists. Closing old socket.");
    this->CloseSocket(this->m_SocketDescriptor);
    this->m_SocketDescriptor = -1;
    }
  this->m_SocketDescriptor = this->CreateSocket();
  if (this->m_SocketDescriptor < 0)
    {
    return -1;
    }
  if ( this->BindSocket(this->m_SocketDescriptor, port) != 0|| 
    this->Listen(this->m_SocketDescriptor) != 0)
    {
    // failed to bind or listen.
    this->CloseSocket(this->m_SocketDescriptor);
    this->m_SocketDescriptor = -1;
    return -1;
    }
  // Success.
  return 0;
}

//-----------------------------------------------------------------------------
//ClientSocket* ServerSocket::WaitForConnection(unsigned long msec /*=0*/)
ClientSocket::Pointer ServerSocket::WaitForConnection(unsigned long msec /*=0*/)
{
  if (this->m_SocketDescriptor < 0)
    {
    igtlErrorMacro("Server Socket not created yet!");
    return NULL;
    }
   
  int ret = this->SelectSocket(this->m_SocketDescriptor, msec);
  if (ret == 0)
    {
    // Timed out.
    return NULL;
    }
  if (ret == -1)
    {
    igtlErrorMacro("Error selecting socket.");
    return NULL;
    }
  int clientsock = this->Accept(this->m_SocketDescriptor);
  if (clientsock == -1)
    {
    igtlErrorMacro("Failed to accept the socket.");
    return NULL;
    }
  // Create a new ClientSocket and return it.
  ClientSocket::Pointer cs = ClientSocket::New();
  cs->m_SocketDescriptor = clientsock;
  return cs;
}

//-----------------------------------------------------------------------------
void ServerSocket::PrintSelf(std::ostream& os) const
{
  this->Superclass::PrintSelf(os);
}


} // end of igtl namespace
