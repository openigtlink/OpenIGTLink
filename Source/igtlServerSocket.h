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
// .NAME igtlServerSocket - Encapsulate a socket that accepts connections.
// .SECTION Description
//

#ifndef __igtlServerSocket_h
#define __igtlServerSocket_h

#include "igtlSocket.h"
#include "igtlClientSocket.h"
#include "igtlWin32Header.h"

namespace igtl
{

class IGTLCommon_EXPORT ServerSocket : public Socket
{
public:
  igtlTypeMacro(igtl::ServerSocket, igtl::Socket);
  igtlNewMacro(igtl::ServerSocket);

  // Description:
  // Creates a server socket at a given port and binds to it.
  // Returns -1 on error. 0 on success.
  int CreateServer(int port);

  // Description:
  // Waits for a connection. When a connection is received
  // a new ClientSocket object is created and returned.
  // Returns NULL on timeout. 
  //ClientSocket* WaitForConnection(unsigned long msec=0);
  ClientSocket::Pointer WaitForConnection(unsigned long msec=0);

  // Description:
  // Returns the port on which the server is running.
  int GetServerPort();
protected:
  ServerSocket();
  ~ServerSocket();

  void PrintSelf(std::ostream& os) const;

private:
  ServerSocket(const ServerSocket&); // Not implemented.
  void operator=(const ServerSocket&); // Not implemented.
};

} // end of igtl namespace 


#endif

