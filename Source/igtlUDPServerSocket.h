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
  Module:    $RCSfile: vtkServerSocket.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME igtlUDPServerSocket - Encapsulate a socket that accepts connections.
// .SECTION Description
//

#ifndef __igtlUDPServerSocket_h
#define __igtlUDPServerSocket_h

//#include <vector>
#include <string.h>

#include "igtlGeneralSocket.h"
#include "igtlUDPClientSocket.h"
#include "igtlWin32Header.h"
#include "igtl_header.h"
#include "igtl_util.h"

namespace igtl
{
class GroupDest
{
public:
  GroupDest(const char* add, igtl_uint16 port, unsigned int groupID)
  {
    this->address = new unsigned char[IP4AddressStrLen];
    strcpy((char*)this->address, add);
    this->portNum = port;
    this->groupID = groupID;
  };
  unsigned char* address;
  igtl_uint16 portNum; // should be in network byte order
  unsigned int groupID;
};
  
class ClientDest
{
public:
  ClientDest(const char* add, igtl_uint16 port, unsigned int clientID)
  {
    this->address = new unsigned char[IP4AddressStrLen];
    strcpy((char*)this->address, add);
    this->portNum = port;
    this->clientID = clientID;
  };
  unsigned char* address;
  igtl_uint16 portNum; // should be in network byte order
  unsigned int clientID;
};
  
class IGTLCommon_EXPORT UDPServerSocket : public GeneralSocket
{
public:
  igtlTypeMacro(igtl::UDPServerSocket, igtl::GeneralSocket);
  igtlNewMacro(igtl::UDPServerSocket);
  
  bool IsMulticastAddreesValid(const char* add);

  
  // Description:
  // Add a client socket with given address at a given port and binds to it.
  // Returns -1 on error. return clientID on success.
  int AddGroup(const char* add, igtl_uint16 port, unsigned int clientID);
  
  // Description:
  // Add a client socket with given address at a given port and binds to it.
  // Returns -1 on error. return clientID on success.
  int AddClient(const char* add, igtl_uint16 port, unsigned int clientID);
  
  
  // Description:
  // Add a client socket with given address at a given port and binds to it.
  // Returns -1 on error. 0 on success.
  int DeleteClient(unsigned int groupID);
  
  // Description:
  // Creates a UDP server socket at a given port and binds to it.
  // Returns -1 on error. 0 on success.
  int CreateUDPServer();
  
  // Desciption:
  // Write the data to all clients
  int WriteSocket(unsigned char* buffer, unsigned bufferSize);

protected:
  UDPServerSocket();
  ~UDPServerSocket();

  std::vector<GroupDest> groups;
  
  std::vector<ClientDest> clients;

  void PrintSelf(std::ostream& os) const;

private:
  UDPServerSocket(const UDPServerSocket&); // Not implemented.
  void operator=(const UDPServerSocket&); // Not implemented.
};

} // end of igtl namespace 


#endif

