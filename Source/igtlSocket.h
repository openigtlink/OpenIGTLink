/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlSocket.h $
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
  Module:    $RCSfile: igtlSocket.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

/** \class Socket
 *  \brief BSD socket encapsulation.
 *
 * This abstract class encapsulates a BSD socket. 
 * It provides an API for basic socket operations.
 *
 * This class was largely based on the igtlSocket class
 * from the Visualization Toolkit VTK.
 *
 */

#ifndef __igtlSocket_h
#define __igtlSocket_h

#include "igtlObject.h"
#include "igtlObjectFactory.h"
#include "igtlMacro.h"
#include "igtlWin32Header.h"


#if defined(_WIN32) && !defined(__CYGWIN__)
#else
#include <sys/time.h>
#endif


namespace igtl
{

class SocketCollection;

// class IGTL_EXPORT Socket
class IGTLCommon_EXPORT Socket : public Object
{
 public:
  typedef Socket              Self;
  typedef Object  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::Socket, igtl::Object)
  igtlNewMacro(igtl::Socket);

public:

  // ----- Status API ----
  // Description:
  // Check is the socket is alive.
  int GetConnected() { return (this->m_SocketDescriptor >=0); }

  // Description:
  // Close the socket.
  void CloseSocket() {
    this->CloseSocket(this->m_SocketDescriptor);
    this->m_SocketDescriptor = -1;
  }
 
  // ------ Communication API ---
  // Description:
  // These methods send data over the socket.
  // Returns 1 on success, 0 on error and raises vtkCommand::ErrorEvent.
  int Send(const void* data, int length);

  // Description:
  // Receive data from the socket.
  // This call blocks until some data is read from the socket.
  // When readFully is set, this call will block until all the
  // requested data is read from the socket.
  // 0 on error, else number of bytes read is returned. On error,
  // vtkCommand::ErrorEvent is raised.
  int Receive(void* data, int length, int readFully=1);

  // Description:
  // Set sending/receiving timeout for the existing socket in millisecond.
  // This function should be called after opening the socket.
  int SetTimeout(int timeout);

  // Description:
  // Set reciving timeout for the existing socket in millisecond.
  // This function should be called after opening the socket.
  int SetReceiveTimeout(int timeout);

  // Description:
  // Set sending timeout for the existing socket in millisecond.
  // This function should be called after opening the socket.
  int SetSendTimeout(int timeout);

  // Description:
  // Set  (psuedo) non-blocking mode for recv(). When sw=1, the time out is set to
  // minimum value (1 microsecond in UNIX, 1 millisecond in Windows) for receiving.
  int SetReceiveBlocking(int sw);

  // Description:
  // Set (psuedo) non-blocking mode for recv(). When sw=1, the time out is set to
  // minimum value (1 microsecond in UNIX, 1 millisecond in Windows) for sending.
  int SetSendBlocking(int sw);

  // Description:
  // Get socket address
  int GetSocketAddressAndPort(std::string& address, int & port);

  // Description:
  // Skip reading data from the socket.
  // The Skip() call has been newly introduced to the igtlSocket,
  // after the class is imported from VTK, thus the call is
  // not available in vtkSocket class.
  int Skip(int length, int skipFully=1);

protected:
  Socket();
  ~Socket();

  void PrintSelf(std::ostream& os) const;

  int m_SocketDescriptor;
  igtlGetMacro(SocketDescriptor, int);

  //BTX
  friend class vtkSocketCollection;
  //ETX
 
  // Description:
  // Creates an endpoint for communication and returns the descriptor.
  // -1 indicates error.
  int CreateSocket();

  // Description:
  // Close the socket.
  void CloseSocket(int socketdescriptor);

  // Description:
  // Binds socket to a particular port.
  // Returns 0 on success other -1 is returned.
  int BindSocket(int socketdescriptor, int port);

  // Description:
  // Selects a socket ie. waits for it to change status.
  // Returns 1 on success; 0 on timeout; -1 on error. msec=0 implies
  // no timeout.
  int SelectSocket(int socketdescriptor, unsigned long msec);

  // Description:
  // Accept a connection on a socket. Returns -1 on error. Otherwise
  // the descriptor of the accepted socket.
  int Accept(int socketdescriptor);

  // Description:
  // Listen for connections on a socket. Returns 0 on success. -1 on error.
  int Listen(int socketdescriptor);

  // Description:
  // Connect to a server socket. Returns 0 on success, -1 on error.
  int Connect(int socketdescriptor, const char* hostname, int port);

  // Description:
  // Returns the port to which the socket is connected.
  // 0 on error.
  int GetPort(int socketdescriptor);

  // Description:
  // Selects set of sockets. Returns 0 on timeout, -1 on error.
  // 1 on success. Selected socket's index is returned thru 
  // selected_index
  static int SelectSockets(const int* sockets_to_select, int size,
    unsigned long msec, int* selected_index);

private:
  Socket(const Socket&); // Not implemented.
  void operator=(const Socket&); // Not implemented.

#if defined(_WIN32) && !defined(__CYGWIN__)
  DWORD m_SendTimeout;
  DWORD m_ReceiveTimeout;
  DWORD m_OrigSendTimeout;
  DWORD m_OrigReceiveTimeout;
#else
  struct timeval m_SendTimeout;
  struct timeval m_ReceiveTimeout;
  struct timeval m_OrigSendTimeout;
  struct timeval m_OrigReceiveTimeout;
#endif
  int m_SendTimeoutFlag;
  int m_ReceiveTimeoutFlag;

};

}

#endif
