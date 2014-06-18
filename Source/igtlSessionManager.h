/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    git@github.com:openigtlink/OpenIGTLink.git
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlSessionManager_h
#define __igtlSessionManager_h

#include "igtlObject.h"
#include "igtlMacro.h"
#include "igtlMessageHandler.h"


#include <vector>

namespace igtl
{

class IGTLCommon_EXPORT SessionManager: public Object
{
 public:

  typedef SessionManager   Self;
  typedef Object                    Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(SessionManager, Object)
  igtlNewMacro(SessionManager);

 public:
  enum {
    MODE_SERVER,
    MODE_CLIENT
  };

  void           SetHostname(const char * str) {this->m_Hostname = str; this->m_ConfigurationUpdated = true; }
  const char *   GetHostname() { return this->m_Hostname.c_str(); }
  void           SetPort(int p) { this->m_Port = p; this->m_ConfigurationUpdated = true; }
  int            GetPort() { return this->m_Port; }

  // Description:
  // Set the role of session manager. Either MODE_SERVER or MODE_CLIENT
  void           SetMode(int m) {this->m_Mode = m; this->m_ConfigurationUpdated = true; }
  int            GetMode() {return this->m_Mode; }

  // Description:
  // Register / Unregister a message handler
  int            AddMessageHandler(MessageHandler*);
  int            RemoveMessageHandler(MessageHandler*);

  // Description:
  // Functions to manage the session
  int            Connect();
  int            Disconnect();
  int            ProcessMessage();
  int            PushMessage(MessageBase*);

 protected:
  SessionManager();
  ~SessionManager();

 protected:
  bool           m_ConfigurationUpdated;
  std::string    m_Hostname;
  int            m_Port;
  int            m_Mode;

  // Description:
  // m_CurrentReadIndex is used to save the current position of the message.
  // The index becomes >0 when message transfer is interrupted and only a part
  // of message has arrived. 
  int            m_CurrentReadIndex;
  int            m_HeaderDeserialized;

  MessageHandler* m_CurrentMessageHandler;

  std::vector< MessageHandler* > m_MessageHandlerList;
  Socket::Pointer  m_Socket;

  igtl::MessageHeader::Pointer m_Header;
  igtl::TimeStamp::Pointer m_TimeStamp;

};

}
#endif // __igtlSessionManager_h
