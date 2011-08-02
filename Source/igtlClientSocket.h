/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlClientSocket.h $
  Language:  C++
  Date:      $Date: 2010-06-09 16:16:36 -0400 (Wed, 09 Jun 2010) $
  Version:   $Revision: 6525 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkClientSocket.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME igtlClientSocket - Encapsulates a client socket.

#ifndef __igtlClientSocket_h
#define __igtlClientSocket_h

#include "igtlSocket.h"
#include "igtlWin32Header.h"

namespace igtl
{

class ServerSocket;

class IGTLCommon_EXPORT ClientSocket : public Socket
{
public:
  typedef ClientSocket              Self;
  typedef Socket                    Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::ClientSocket, igtl::Socket)
  igtlNewMacro(igtl::ClientSocket);

  // Description:
  // Connects to host. Returns 0 on success, -1 on error.
  int ConnectToServer(const char* hostname, int port); 
  
protected:
  ClientSocket();
  ~ClientSocket();

  void PrintSelf(std::ostream& os) const;

//BTX
  friend class ServerSocket;
//ETX
private:
  ClientSocket(const ClientSocket&); // Not implemented.
  void operator=(const ClientSocket&); // Not implemented.
};

}

#endif

