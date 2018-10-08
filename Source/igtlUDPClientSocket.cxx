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
  Module:    $RCSfile: vtkClientSocket.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "igtlUDPClientSocket.h"
#include <string.h>

namespace igtl
{

//-----------------------------------------------------------------------------
UDPClientSocket::UDPClientSocket()
{
}

//-----------------------------------------------------------------------------
UDPClientSocket::~UDPClientSocket()
{
}
  
int UDPClientSocket::JoinNetwork(const char* groupIPAddr, int portNum)
{
  this->SetIPAddress(groupIPAddr);
  this->SetPortNumber(portNum);  
  this->m_SocketDescriptor = this->CreateUDPClientSocket();
  return this->m_SocketDescriptor;
}

//-----------------------------------------------------------------------------
int UDPClientSocket::ReadSocket(unsigned char* buffer, unsigned bufferSize) {
  if (!this->m_SocketDescriptor)
  {
    igtlErrorMacro("Failed to create socket.");
    return -1;
  }
  int bytesRead = ReceiveUDP(buffer, bufferSize);
  return bytesRead;
}
//-----------------------------------------------------------------------------
void UDPClientSocket::PrintSelf(std::ostream& os) const
{
  this->Superclass::PrintSelf(os);
}

} // end of igtl namespace

