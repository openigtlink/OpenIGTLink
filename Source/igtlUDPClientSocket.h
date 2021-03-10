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
  Module:    $RCSfile: vtkClientSocket.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.UDPClientSocket
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME igtlUDPClientSocket - Encapsulates a client UDPsocket.

#ifndef __igtlUDPClientSocket_h
#define __igtlUDPClientSocket_h

#include <errno.h>
#include "igtlGeneralSocket.h"
#include "igtlWin32Header.h"

namespace igtl
{

class UDPServerSocket;
  
class IGTLCommon_EXPORT UDPClientSocket : public GeneralSocket
{
public:
  igtlTypeMacro(igtl::UDPClientSocket, igtl::GeneralSocket)
  igtlNewMacro(igtl::UDPClientSocket);

  int JoinNetwork(const char* groupIPAddr, int portNum);

  int ReadSocket(unsigned char* buffer, unsigned bufferSize);
  
protected:
  UDPClientSocket();
  ~UDPClientSocket();

  void PrintSelf(std::ostream& os) const override;

  friend class UDPServerSocket;

private:
  UDPClientSocket(const UDPClientSocket&); // Not implemented.
  void operator=(const UDPClientSocket&); // Not implemented.
};

}

#endif

