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
#include "igtlUDPServerSocket.h"

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>
#endif

namespace igtl
{

//-----------------------------------------------------------------------------
UDPServerSocket::UDPServerSocket()
{
}

//-----------------------------------------------------------------------------
UDPServerSocket::~UDPServerSocket()
{
}

//-----------------------------------------------------------------------------
bool UDPServerSocket::IsMulticastAddreesValid(const char* add)
{
  //224.0.0.0 through 224.0.0.255, are not routable
  igtl_uint32 address = inet_addr(add); // to do: make sure the endian is correct
  if(igtl_is_little_endian())
  {
    address = BYTE_SWAP_INT32(address);
  }
  return address >  0xE00000FF &&
  address <= 0xEFFFFFFF;
}

//-----------------------------------------------------------------------------
int UDPServerSocket::AddGroup(const char* add, igtl_uint16 port, unsigned int groupID)
{
  if (IsMulticastAddreesValid(add))
  {
    for(std::vector<GroupDest>::size_type i = 0; i < this->groups.size(); i++)
    {
      if (this->groups[i].groupID == groupID &&
          (strcmp((const char *)this->groups[i].address ,add)==0)
          && this->groups[i].portNum ==  port )
      {
        
        return -1;
      }
    }
    this->groups.push_back(GroupDest(add, port, groupID));
  }
  return 0;
}

//-----------------------------------------------------------------------------
int UDPServerSocket::AddClient(const char* add, igtl_uint16 port, unsigned int clientID)
{
  for(std::vector<ClientDest>::size_type i = 0; i < this->clients.size(); i++)
  {
    if (this->clients[i].clientID == clientID &&
        (strcmp((const char *)this->clients[i].address ,add)==0)
        && this->clients[i].portNum ==  port )
    {
      
      return -1;
    }
  }
  this->clients.push_back(ClientDest(add, port, clientID));
  return 0;
}

//-----------------------------------------------------------------------------
int UDPServerSocket::WriteSocket(unsigned char* buffer, unsigned bufferSize)
{
  int numByteSend = 0;
  for(std::vector<GroupDest>::size_type i = 0; i < this->groups.size(); i++)
  {
    this->SetIPAddress((const char*)this->groups[i].address);
    this->SetPortNumber(this->groups[i].portNum);
    numByteSend = SendUDP((char*)buffer, bufferSize);
  }
  for(std::vector<ClientDest>::size_type i = 0; i < this->clients.size(); i++)
  {
    this->SetIPAddress((const char*)this->clients[i].address);
    this->SetPortNumber(this->clients[i].portNum);
    numByteSend = SendUDP((char*)buffer, bufferSize);
  }
  return numByteSend;
}

//-----------------------------------------------------------------------------
int UDPServerSocket::CreateUDPServer()
{
  if (this->m_SocketDescriptor != -1)
  {
    igtlWarningMacro("Server Socket already exists. Closing old socket.");
    this->CloseSocket(this->m_SocketDescriptor);
    this->m_SocketDescriptor = -1;
  }
  this->m_SocketDescriptor = this->CreateUDPServerSocket();
  if (this->m_SocketDescriptor < 0)
  {
    return -1;
  }
  return 0;
}

//-----------------------------------------------------------------------------
int UDPServerSocket::DeleteClient(unsigned int groupID)
{
  for(std::vector<GroupDest>::size_type i = 0; this->groups.size(); i++)
  {
    if (this->groups[i].groupID == groupID)
    {
      this->groups.erase(this->groups.begin()+i);
      return 0;
    }
  }
  return -1;
}

//-----------------------------------------------------------------------------
void UDPServerSocket::PrintSelf(std::ostream& os) const
{
  this->Superclass::PrintSelf(os);
}

} // end of igtl namespace
