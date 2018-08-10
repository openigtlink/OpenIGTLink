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

/// class IGTL_EXPORT Socket
class IGTLCommon_EXPORT Socket : public Object
{
 public:
  igtlTypeMacro(igtl::Socket, igtl::Object)
  igtlNewMacro(igtl::Socket);

public:

  /// Check is the socket is alive.
  bool GetConnected() { return (this->m_SocketDescriptor >=0); }
  
  /// Close the socket.
  void CloseSocket() {
    this->CloseSocket(this->m_SocketDescriptor);
    this->m_SocketDescriptor = -1;
  }
 
  /// These methods send data over the socket.
  /// Returns 1 on success, 0 on error and raises vtkCommand::ErrorEvent.
  /// SIGPIPE or other signal may be raised on systems (e.g., Sun Solaris) where
  /// MSG_NOSIGNAL flag is not supported for the socket send method.
  int Send(const void* data, int length);

  /// Receive data from the socket.
  /// This call blocks until some data is read from the socket, unless timeout is set
  /// by SetTimeout() or SetReceiveTimeout().
  /// When the readFully flag is set, this call will block until all the requested data is
  /// read from the socket. The readFully flag will be ignored if the timeout is active.
  /// 0 on error, -1 on timeout, else number of bytes read is returned.
  int Receive(void* data, int length, int readFully=1);

  /// Set sending/receiving timeout for the existing socket in millisecond.
  /// This function should be called after opening the socket.
  int SetTimeout(int timeout);

  /// Set reciving timeout for the existing socket in millisecond.
  /// This function should be called after opening the socket.
  int SetReceiveTimeout(int timeout);

  /// Set sending timeout for the existing socket in millisecond.
  /// This function should be called after opening the socket.
  int SetSendTimeout(int timeout);

  /// Set  (psuedo) non-blocking mode for recv(). When sw=1, the time out is set to
  /// minimum value (1 microsecond in UNIX, 1 millisecond in Windows) for receiving.
  int SetReceiveBlocking(int sw);

  /// Set (psuedo) non-blocking mode for recv(). When sw=1, the time out is set to
  /// minimum value (1 microsecond in UNIX, 1 millisecond in Windows) for sending.
  int SetSendBlocking(int sw);

  /// Get socket address
  int GetSocketAddressAndPort(std::string& address, int & port);

  /// Skip reading data from the socket.
  /// The Skip() call has been newly introduced to the igtlSocket,
  /// after the class is imported from VTK, thus the call is
  /// not available in vtkSocket class.
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
 
  /// Creates an endpoint for communication and returns the descriptor.
  /// -1 indicates error.
  int CreateSocket();

  /// Close the socket.
  void CloseSocket(int socketdescriptor);

  /// Binds socket to a particular port.
  /// Returns 0 on success other -1 is returned.
  int BindSocket(int socketdescriptor, int port);

  /// Selects a socket ie. waits for it to change status.
  /// Returns 1 on success; 0 on timeout; -1 on error. msec=0 implies
  /// no timeout.
  int SelectSocket(int socketdescriptor, unsigned long msec);

  /// Accept a connection on a socket. Returns -1 on error. Otherwise
  /// the descriptor of the accepted socket.
  int Accept(int socketdescriptor);

  /// Listen for connections on a socket. Returns 0 on success. -1 on error.
  int Listen(int socketdescriptor);

  /// Connect to a server socket. Returns 0 on success, -1 on error.
  int Connect(int socketdescriptor, const char* hostname, int port);

  /// Returns the port to which the socket is connected.
  /// 0 on error.
  int GetPort(int socketdescriptor);

  /// Selects set of sockets. Returns 0 on timeout, -1 on error.
  /// 1 on success. Selected socket's index is returned thru 
  /// selected_index
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
