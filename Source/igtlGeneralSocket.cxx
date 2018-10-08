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
 Module:    $RCSfile: igtlSocket.cxx,v $
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#include "igtlGeneralSocket.h"
#include <string.h>
#include <stdio.h>

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

#if defined(_WIN32) && !defined(__CYGWIN__)
#define WSA_VERSION MAKEWORD(1,1)
#define igtlCloseSocketMacro(sock) (closesocket(sock))
#else
#define igtlCloseSocketMacro(sock) (shutdown(sock, 2))
#endif

#if defined (_WIN32)
  #define TTL_TYPE const char
#else
  #define TTL_TYPE igtl_uint8
#endif

namespace igtl
{
  
  //-----------------------------------------------------------------------------
  GeneralSocket::GeneralSocket()
  {
    this->m_SocketDescriptor = -1;
    this->m_SendTimeoutFlag = 0;
    this->m_ReceiveTimeoutFlag = 0;
  }
  
  //-----------------------------------------------------------------------------
  GeneralSocket::~GeneralSocket()
  {
    if (this->m_SocketDescriptor != -1)
      {
      this->CloseSocket(this->m_SocketDescriptor);
      this->m_SocketDescriptor = -1;
      }
  }
  
  //-----------------------------------------------------------------------------
  int GeneralSocket::CreateSocket()
  {
  #if defined(_WIN32) && !defined(__CYGWIN__)
    // Declare variables
    WSADATA wsaData;
    
    //---------------------------------------
    // Initialize Winsock
    int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
    if( iResult != NO_ERROR )
      {
      std::cerr << "Error at WSAStartup" << std::endl;
      return -1;
      }
  #endif
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    // Elimate windows 0.2 second delay sending (buffering) data.
    int on = 1;
    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&on, sizeof(on)))
      {
      return -1;
      }
    return sock;
  }
  
  
  //-----------------------------------------------------------------------------
  int GeneralSocket::CreateUDPServerSocket()
  {
  #if defined(_WIN32) && !defined(__CYGWIN__)
    // Declare variables
    WSADATA wsaData;
    
    //---------------------------------------
    // Initialize Winsock
    int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
    if( iResult != NO_ERROR )
      {
      std::cerr << "Error at WSAStartup" << std::endl;
      return -1;
      }
  #endif
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    int bcast = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char *)&bcast, sizeof(bcast)) < 0)
      {
      CloseSocket(sock);
      return -1;
      }
    return sock;
  }
  
  int GeneralSocket::CreateUDPClientSocket()
  {
  #if defined(_WIN32) && !defined(__CYGWIN__)
    // Declare variables
    WSADATA wsaData;
    
    //---------------------------------------
    // Initialize Winsock
    int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
    if( iResult != NO_ERROR )
      {
      std::cerr << "Error at WSAStartup" << std::endl;
      return -1;
      }
  #endif
    int err;
    struct addrinfo * info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    // maximum port number is less than 65536, 128byte is sufficient. std::to_string needs c++ 11 support
    char portStr[128];
    sprintf(portStr, "%d", this->PortNum);
    if (0 != (err = getaddrinfo(this->IPAddress, portStr, &hints, &info)))
      {
      return -1;
      }
    int sock;
    struct addrinfo * p;
    for (p = info; p != NULL; p = p->ai_next)
      {
      if (0 > (sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)))
        {
        continue;
        }

      if (0 > bind(sock, p->ai_addr, p->ai_addrlen))
        {
        CloseSocket(sock);
        continue;
        }

      break;
      }
    int bcast = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char *)&bcast, sizeof(bcast)) < 0)
      {
      CloseSocket(sock);
      return -1;
      }
    return sock;
  }

  //-----------------------------------------------------------------------------
  int GeneralSocket::BindSocket(int socketdescriptor, int port)
  {
    struct sockaddr_in server;
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);
    // Allow the socket to be bound to an address that is already in use
  #ifdef _WIN32
    int opt=1;
    setsockopt(socketdescriptor, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(int));
  #elif defined(VTK_HAVE_SO_REUSEADDR)
    int opt=1;
    setsockopt(socketdescriptor, SOL_SOCKET, SO_REUSEADDR, (void *) &opt, sizeof(int));
  #endif
    
    if ( bind(socketdescriptor, reinterpret_cast<sockaddr*>(&server), sizeof(server)) )
      {
      return -1;
      }
    return 0;
  }
  
  //-----------------------------------------------------------------------------
  int GeneralSocket::Accept(int socketdescriptor)
  {
    if (socketdescriptor < 0)
      {
      return -1;
      }
    return accept(socketdescriptor, 0, 0);
  }
  
  //-----------------------------------------------------------------------------
  int GeneralSocket::Listen(int socketdescriptor)
  {
    if (socketdescriptor < 0)
      {
      return -1;
      }
    return listen(socketdescriptor, 1);
  }
  
  //-----------------------------------------------------------------------------
  int GeneralSocket::SelectSocket(int socketdescriptor, unsigned long msec)
  {
    if (socketdescriptor < 0 )
      {
      // invalid socket descriptor.
      return -1;
      }
    
    fd_set rset;
    struct timeval tval;
    struct timeval* tvalptr = 0;
    if ( msec > 0 )
      {
      tval.tv_sec = msec / 1000;
      tval.tv_usec = (msec % 1000)*1000;
      tvalptr = &tval;
      }
    FD_ZERO(&rset);
    FD_SET(socketdescriptor, &rset);
    int res = select(socketdescriptor + 1, &rset, 0, 0, tvalptr);
    if(res == 0)
      {
      return 0;//for time limit expire
      }
    
    if ( res < 0 || !(FD_ISSET(socketdescriptor, &rset)) )
      {
      // Some error.
      return -1;
      }
    // The indicated socket has some activity on it.
    return 1;
  }

  //-----------------------------------------------------------------------------
  int GeneralSocket::SelectSockets(const int* sockets_to_select, int size,
                                   unsigned long msec, int* selected_index)
  {
    int i;
    int max_fd = -1;
    *selected_index = -1;
    if (size <  0)
      {
      return -1;
      }
    
    fd_set rset;
    struct timeval tval;
    struct timeval* tvalptr = 0;
    if ( msec > 0 )
      {
      tval.tv_sec = msec / 1000;
      tval.tv_usec = msec % 1000;
      tvalptr = &tval;
      }
    FD_ZERO(&rset);
    for (i=0; i<size; i++)
      {
      FD_SET(sockets_to_select[i],&rset);
      max_fd = (sockets_to_select[i] > max_fd)? sockets_to_select[i] : max_fd;
      }
    
    int res = select(max_fd + 1, &rset, 0, 0, tvalptr);
    if (res == 0)
      {
      return 0; //Timeout
      }
    if (res < 0)
      {
      // SelectSocket error.
      return -1;
      }
    
    //check which socket has some activity.
    for (i=0; i<size; i++)
      {
      if ( FD_ISSET(sockets_to_select[i],&rset) )
        {
        *selected_index = i;
        return 1;
        }
      }
    return -1;
  }
  
  //-----------------------------------------------------------------------------
  int GeneralSocket::Connect(int socketdescriptor, const char* hostName, int port)
  {
    if (socketdescriptor < 0)
      {
      return -1;
      }
    
    struct hostent* hp;
    hp = gethostbyname(hostName);
    if (!hp)
      {
      unsigned long addr = inet_addr(hostName);
      hp = gethostbyaddr((char *)&addr, sizeof(addr), AF_INET);
      }
    
    if (!hp)
      {
      // vtkErrorMacro("Unknown host: " << hostName);
      return -1;
      }
    
    struct sockaddr_in name;
    name.sin_family = AF_INET;
    memcpy(&name.sin_addr, hp->h_addr, hp->h_length);
    name.sin_port = htons(port);
    
    int r = connect(socketdescriptor, reinterpret_cast<sockaddr*>(&name),
                    sizeof(name));
    
    return r;
  }
  
  //-----------------------------------------------------------------------------
  int GeneralSocket::GetPort(int sock)
  {
    struct sockaddr_in sockinfo;
    memset(&sockinfo, 0, sizeof(sockinfo));
  #if defined(OpenIGTLink_HAVE_GETSOCKNAME_WITH_SOCKLEN_T)
    socklen_t sizebuf = sizeof(sockinfo);
  #else
    int sizebuf = sizeof(sockinfo);
  #endif
    //  FIXME: Setup configuration for VTK_HAVE_GETSOCKNAME_WITH_SOCKLEN_T so we can uncomment these lines
    if(getsockname(sock, reinterpret_cast<sockaddr*>(&sockinfo), &sizebuf) != 0)
      {
      return 0;
      }
    return ntohs(sockinfo.sin_port);
  }
  
  //-----------------------------------------------------------------------------
  void GeneralSocket::CloseSocket(int socketdescriptor)
  {
    if (socketdescriptor < 0)
      {
      return;
      }
    igtlCloseSocketMacro(socketdescriptor);
  }
  
  //-----------------------------------------------------------------------------
  int GeneralSocket::Send(const void* data, int length)
  {
    if (!this->GetConnected())
      {
      return 0;
      }
    if (length == 0)
      {
      // nothing to send.
      return 1;
      }
    const char* buffer = reinterpret_cast<const char*>(data);
    int total = 0;
    do
      {
      int flags;
  #if defined(_WIN32) && !defined(__CYGWIN__)
      flags = 0;
  #else
      // On unix boxes if the client disconnects and the server attempts
      // to send data through the socket then the application crashes
      // due to SIGPIPE signal. Disable the signal to prevent crash.
      //#ifndef __sun
      //  flags = MSG_NOSIGNAL;
      //#else
      //  // Signal is not present on SUN systems, the signal has
      //  // to be managed at application level.
      //  flags = 0;
      //#endif
  #if defined(MSG_NOSIGNAL) // For Linux > 2.2
      flags = MSG_NOSIGNAL;
  #else
  #if defined(SO_NOSIGPIPE) // Mac OS X
      int set = 1;
      setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
  #endif
      flags = 0;
  #endif
  #endif
      int n = send(this->m_SocketDescriptor, buffer+total, length-total, flags);
      if(n < 0)
        {
        // FIXME : Use exceptions ?  igtlErrorMacro("Socket Error: Send failed.");
        return 0;
        }
      total += n;
      } while(total < length);
    return 1;
  }
  
  //-----------------------------------------------------------------------------
  int GeneralSocket::Receive(void* data, int length, int readFully/*=1*/)
  {
    if (!this->GetConnected())
      {
      return 0;
      }
    
    char* buffer = reinterpret_cast<char*>(data);
    int total = 0;
    do
      {
  #if defined(_WIN32) && !defined(__CYGWIN__)
      int trys = 0;
  #endif
      
      int n = recv(this->m_SocketDescriptor, buffer+total, length-total, 0);
      
  #if defined(_WIN32) && !defined(__CYGWIN__)
      if(n == 0)
        {
        // On long messages, Windows recv sometimes fails with WSAENOBUFS, but
        // will work if you try again.
        int error = WSAGetLastError();
        if ((error == WSAENOBUFS) && (trys++ < 1000))
          {
          Sleep(1);
          continue;
          }
        // FIXME : Use exceptions ?  igtlErrorMacro("Socket Error: Receive failed.");
        return 0;
        }
      else if (n < 0)
        {
        // TODO: Need to check if this means timeout.
        return -1;
        }
  #else
      if(n == 0) // Disconnected
        {
        // FIXME : Use exceptions ?  igtlErrorMacro("Socket Error: Receive failed.");
        return 0;
        }
      else if (n < 0) // Error (including time out)
        {
        // TODO: If it is time-out, errno == EAGAIN
        return -1;
        }
  #endif
      
      total += n;
      } while(readFully && total < length);
    return total;
  }
  
  int GeneralSocket::SetIPAddress(const char* ip)
  {
    if(strcpy(this->IPAddress,ip))
      {
      return 0;
      }
    return -1;
  }
  
  int GeneralSocket::SetPortNumber(igtl_uint16 port)
  {
  #if defined(_WIN32) && !defined(__CYGWIN__)
    this->PortNum = port;
  #else
    this->PortNum = (in_port_t) port;
  #endif
    return 0;
  }
  
  //-----------------------------------------------------------------------------
  int GeneralSocket::SendUDP(const void* data, int length)
  {
    if (!this->GetConnected())
      {
      return 0;
      }
    if (length == 0)
      {
      // nothing to send.
      return 1;
      }
    int flags;
  #if defined(_WIN32) && !defined(__CYGWIN__)
    flags = 0;
  #else
    // On unix boxes if the client disconnects and the server attempts
    // to send data through the socket then the application crashes
    // due to SIGPIPE signal. Disable the signal to prevent crash.
    //#ifndef __sun
    //  flags = MSG_NOSIGNAL;
    //#else
    //  // Signal is not present on SUN systems, the signal has
    //  // to be managed at application level.
    //  flags = 0;
    //#endif
  #if defined(MSG_NOSIGNAL) // For Linux > 2.2
    flags = MSG_NOSIGNAL;
  #else
  #if defined(SO_NOSIGPIPE) // Mac OS X
    int set = 1;
    setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
  #endif
    flags = 0;
  #endif
  #endif
    igtl_uint8 ttlArg = 1; // 1 is the default value , valid value from 0 to 255
    TTL_TYPE ttl = (TTL_TYPE)ttlArg;
    if (setsockopt(this->m_SocketDescriptor, IPPROTO_IP, IP_MULTICAST_TTL,
                   (const char*)&ttl, sizeof ttl) < 0) {
      return 0;
    }
    
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    
    // store this IP address in dest:
    dest.sin_addr.s_addr = inet_addr(this->IPAddress);
    dest.sin_port = htons(this->PortNum);
    
    int n = sendto(this->m_SocketDescriptor, (char*)data, length, 0, (struct sockaddr*)&dest, sizeof dest);
    if(n < 0)
      {
      // FIXME : Use exceptions ?  igtlErrorMacro("Socket Error: Send failed.");
      return 0;
      }
    return n;
  }
  
  //-----------------------------------------------------------------------------
  int GeneralSocket::ReceiveUDP(void* data, int length)
  {
    if (!this->GetConnected())
      {
      return 0;
      }
    
    char* buffer = reinterpret_cast<char*>(data);
  #if defined(_WIN32) && !defined(__CYGWIN__)
    int trys = 0;
  #endif
    struct sockaddr_storage from;
  #if defined(OpenIGTLink_HAVE_GETSOCKNAME_WITH_SOCKLEN_T)
    // store this IP address in dest:
    socklen_t addressLen = sizeof(from);
  #else
    int addressLen = sizeof(from);
  #endif
    
    //dest.sin_port = htons(this->PortNum);
  #if defined(_WIN32) && !defined(__CYGWIN__)
    int n = recvfrom(this->m_SocketDescriptor, buffer, length, 0, (struct sockaddr*)&from, &addressLen);
  #else
    int n = recvfrom(this->m_SocketDescriptor, (void*)buffer, length, 0, (struct sockaddr*)&from, &addressLen);
  #endif
  #if defined(_WIN32) && !defined(__CYGWIN__)
    if(n == 0)
      {
      // On long messages, Windows recv sometimes fails with WSAENOBUFS, but
      // will work if you try again.
      int error = WSAGetLastError();
      if ((error == WSAENOBUFS) && (trys++ < 1000))
        {
        Sleep(1);
        }
      // FIXME : Use exceptions ?  igtlErrorMacro("Socket Error: Receive failed.");
      return 0;
      }
    else if (n < 0)
      {
      // TODO: Need to check if this means timeout.
      return -1;
      }
  #else
    if(n == 0) // Disconnected
      {
      // FIXME : Use exceptions ?  igtlErrorMacro("Socket Error: Receive failed.");
      return 0;
      }
    else if (n < 0) // Error (including time out)
      {
      // TODO: If it is time-out, errno == EAGAIN
      return -1;
      }
  #endif
    return n;
  }

  
  //-----------------------------------------------------------------------------
  int GeneralSocket::SetTimeout(int timeout)
  {
    if (SetReceiveTimeout(timeout) && SetSendTimeout(timeout))
      {
      return 1;
      }
    else
      {
      return 0;
      }
  }
  
  
  //-----------------------------------------------------------------------------
  int GeneralSocket::SetReceiveTimeout(int timeout)
  {
    if (!this->GetConnected())
      {
      return 0;
      }
    
  #if defined(_WIN32) && !defined(__CYGWIN__)
    this->m_ReceiveTimeout = timeout;
    int len;
  #else
    this->m_ReceiveTimeout.tv_sec  = timeout/1000;          /* second */
    this->m_ReceiveTimeout.tv_usec = (timeout%1000) * 1000; /* microsecond */
    socklen_t len;
  #endif
    if ( timeout > 0 )
      {
      getsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_RCVTIMEO,
                 (char*)&(this->m_OrigReceiveTimeout), &len);
      setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_RCVTIMEO,
                 (char*)&(this->m_ReceiveTimeout), sizeof(this->m_ReceiveTimeout));
      this->m_ReceiveTimeoutFlag = 1;
      }
    else if (this->m_ReceiveTimeoutFlag)
      {
      setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_RCVTIMEO,
                 (char*)&(this->m_OrigReceiveTimeout), sizeof(this->m_OrigReceiveTimeout));
      this->m_ReceiveTimeoutFlag = 0;
      }
    
    return timeout;
  }
  
  
  //-----------------------------------------------------------------------------
  int GeneralSocket::SetSendTimeout(int timeout)
  {
    if (!this->GetConnected())
      {
      return 0;
      }
    
  #if defined(_WIN32) && !defined(__CYGWIN__)
    this->m_SendTimeout = timeout;
    int len;
  #else
    this->m_SendTimeout.tv_sec  = timeout/1000;          /* second */
    this->m_SendTimeout.tv_usec = (timeout%1000) * 1000; /* microsecond */
    socklen_t len;
  #endif
    if ( timeout > 0 )
      {
      getsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_SNDTIMEO,
                 (char*)&(this->m_OrigSendTimeout), &len);
      setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_SNDTIMEO,
                 (char*)&(this->m_SendTimeout), sizeof(this->m_SendTimeout));
      this->m_SendTimeoutFlag = 1;
      }
    else if (this->m_SendTimeoutFlag)
      {
      setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_SNDTIMEO,
                 (char*)&(this->m_OrigSendTimeout), sizeof(this->m_OrigSendTimeout));
      this->m_SendTimeoutFlag = 0;
      }
    
    return timeout;
  }
  
  
  //-----------------------------------------------------------------------------
  int GeneralSocket::SetReceiveBlocking(int sw)
  {
    if (!this->GetConnected())
      {
      return 0;
      }
    
    // If sw == 1, timeout is set to 0 (wait until it receives message)
  #if defined(_WIN32) && !defined(__CYGWIN__)
    if (sw==0)
      {
      this->m_ReceiveTimeout = 1;
      }
    else
      {
      this->m_ReceiveTimeout = 0;
      }
    int len;
  #else
    if (sw==0)
      {
      this->m_ReceiveTimeout.tv_sec  = 0;          /* second */
      this->m_ReceiveTimeout.tv_usec = 1;          /* nanosecond */
      }
    else
      {
      this->m_ReceiveTimeout.tv_sec  = 0;          /* second */
      this->m_ReceiveTimeout.tv_usec = 0;          /* nanosecond */
      }
    socklen_t len;
  #endif
    if (sw==0)
      {
      getsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_RCVTIMEO,
                 (char*)&(this->m_OrigReceiveTimeout), &len);
      setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_RCVTIMEO,
                 (char*)&(this->m_ReceiveTimeout), sizeof(this->m_ReceiveTimeout));
      this->m_ReceiveTimeoutFlag = 1;
      }
    else if (this->m_ReceiveTimeoutFlag)
      {
      setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_RCVTIMEO,
                 (char*)&(this->m_OrigReceiveTimeout), sizeof(this->m_OrigReceiveTimeout));
      this->m_ReceiveTimeoutFlag = 0;
      }
    
    return sw;
  }
  
  
  //-----------------------------------------------------------------------------
  int GeneralSocket::SetSendBlocking(int sw)
  {
    if (!this->GetConnected())
      {
      return 0;
      }
    
    // If sw == 1, timeout is set to 0 (wait until it receives message)
  #if defined(_WIN32) && !defined(__CYGWIN__)
    if (sw==0)
      {
      this->m_SendTimeout = 1;
      }
    else
      {
      this->m_SendTimeout = 0;
      }
    int len;
  #else
    if (sw==0)
      {
      this->m_SendTimeout.tv_sec  = 0;          /* second */
      this->m_SendTimeout.tv_usec = 1;          /* nanosecond */
      }
    else
      {
      this->m_SendTimeout.tv_sec  = 0;          /* second */
      this->m_SendTimeout.tv_usec = 0;          /* nanosecond */
      }
    socklen_t len;
  #endif
    if (sw==0)
      {
      getsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_SNDTIMEO,
                 (char*)&(this->m_OrigSendTimeout), &len);
      setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_SNDTIMEO,
                 (char*)&(this->m_SendTimeout), sizeof(this->m_SendTimeout));
      this->m_SendTimeoutFlag = 1;
      }
    else if (this->m_SendTimeoutFlag)
      {
      setsockopt(this->m_SocketDescriptor, SOL_SOCKET, SO_SNDTIMEO,
                 (char*)&(this->m_OrigSendTimeout), sizeof(this->m_OrigSendTimeout));
      this->m_SendTimeoutFlag = 0;
      }
    
    return sw;
  }
  
  
  //-----------------------------------------------------------------------------
  int GeneralSocket::GetSocketAddressAndPort(std::string& address, int& port)
  {
    struct sockaddr_in sockinfo;
    
    memset(&sockinfo, 0, sizeof(sockinfo));
    
  #if defined(OpenIGTLink_HAVE_GETSOCKNAME_WITH_SOCKLEN_T)
    socklen_t sizebuf = sizeof(sockinfo);
  #else
    int sizebuf = sizeof(sockinfo);
  #endif
    
    if( getsockname(this->m_SocketDescriptor, reinterpret_cast<sockaddr*>(&sockinfo), &sizebuf) != 0)
      {
      return -1;
      }
    const char* a = inet_ntoa(sockinfo.sin_addr);
    if ( a == NULL )
      {
      return -1;
      }
    address = a;
    port = ntohs(sockinfo.sin_port);
    
    return 0;
  }
  
  
  //-----------------------------------------------------------------------------
  int GeneralSocket::Skip(int length, int skipFully/*=1*/)
  {
    if (length == 0)
      {
      return 0;
      }
    
    unsigned char dummy[256];
    int block  = 256;
    int n      = 0;
    int remain = length;
    
    do
      {
      if (remain < block)
        {
        block = remain;
        }
      
      n = this->Receive(dummy, block, skipFully);
      if (!skipFully && n <= 0)
        {
        break;
        }
      remain -= n;
      }
    while (remain > 0 || (skipFully && n < block));
    
    return (length - remain);
  
  }
  
  //-----------------------------------------------------------------------------
  void GeneralSocket::PrintSelf(std::ostream& os) const
  {
    this->Superclass::PrintSelf(os);
  }
  
  
} // end of igtl namespace
