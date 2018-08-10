/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlBindMessage_h
#define __igtlBindMessage_h

#include <string>

#include "igtlObject.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

namespace igtl
{
/// The BindMessageBase class is a base class for the BindMessage, GetBindMessage and
/// StartBindMessage classes. The class is used to manage a list of child messages that
/// will be bundled into a BIND message. 
class IGTLCommon_EXPORT BindMessageBase: public MessageBase
{
public:
  igtlTypeMacro(igtl::BindMessageBase, igtl::MessageBase);
  igtlNewMacro(igtl::BindMessageBase);

public:

  /// Initializes the BindMessageBase class.
  void        Init();

  /// Sets the number of child messages bundled in the message and returns the total number of
  /// child messages. The returned value should be same as 'n', if the number is updated successfully.
  int         SetNumberOfChildMessages(unsigned int n);

  /// Gets the number of child messages bundled in the message.
  int         GetNumberOfChildMessages();

  /// Appends a new child message to the end of the list of child messages.
  /// The AppendChildMessage() function will increment the number of child messages.
  /// Returns the number of child messages after appending the specified child message.
  int         AppendChildMessage(igtl::MessageBase * child);

  /// Sets or replaces a child message specified by the index 'i'. The SetChildMessage() does not
  /// increment the number of child messages. Returns non-zero value, if success. 
  int         SetChildMessage(unsigned int i, igtl::MessageBase * child);

  /// Gets the name of a child message specified by the index 'i'.
  const char* GetChildMessageType(unsigned int i);

protected:

  BindMessageBase();
  ~BindMessageBase();
  
protected:

  /// A structure to manage properties of a child message, including message type, message name,
  /// size and pointer to the class instance of the child message. A ChildMessageInfo structure is
  /// allocated per child message and managed by a vector m_ChildMessages.
  typedef struct {
    std::string  type;
    std::string  name;
    igtlUint64   size;
    void *       ptr;
  } ChildMessageInfo;

  /// A vector to manage a list of ChildMessageInfo structures. 
  std::vector<ChildMessageInfo> m_ChildMessages;
  
};


/// A class for the BIND message type
class IGTLCommon_EXPORT BindMessage: public BindMessageBase
{
public:
  igtlTypeMacro(igtl::BindMessage, igtl::BindMessageBase);
  igtlNewMacro(igtl::BindMessage);

public:
  /// Gets a child message specified by the index 'i'. A pointer to the instance of
  /// the specified child message is substituted to the message base specified by 'child'.
  /// Returns non-zero value if success. 
  int         GetChildMessage(unsigned int i, igtl::MessageBase * child);

protected:
  BindMessage();
  ~BindMessage();
  
protected:

  virtual int  CalculateContentBufferSize();
  virtual int  PackContent();
  virtual int  UnpackContent();

};


/// A class for the GET_BIND message type. The GET_BIND message contains a list of child messages
/// to request for specific messages. If no child message is specified, the receiver must return
/// the all available messages.
class IGTLCommon_EXPORT GetBindMessage: public BindMessageBase
{
public:
  igtlTypeMacro(igtl::GetBindMessage, igtl::BindMessageBase);
  igtlNewMacro(igtl::GetBindMessage);

public:
  /// Appends the type and name of a new child message to the end of the list of child messages.
  /// The AppendChildMessage() function will increment the number of child messages.
  /// Returns the number of child messages.
  int          AppendChildMessage(const char * type, const char * name);

protected:
  GetBindMessage();
  ~GetBindMessage();
  
protected:
  
  virtual int  CalculateContentBufferSize();
  virtual int  PackContent();
  virtual int  UnpackContent();

};


/// A class for the STT_BIND message type. Like the GET_BIND message type, it contains a list of
/// child messages to request for specific messages. If no child message is specified,
/// the receiver must return the all available messages.
class IGTLCommon_EXPORT StartBindMessage: public GetBindMessage
{
public:
  igtlTypeMacro(igtl::StartBindMessage, igtl::GetBindMessage);
  igtlNewMacro(igtl::StartBindMessage);

public:

  /// Sets time resolution. The time resolution is specified
  /// as a 64-bit fixed-point used in OpenIGTLink time stamp.
  void        SetResolution(igtlUint64 res);

  /// Gets time resolution. The time resolution is specified
  /// as a 64-bit fixed-point used in OpenIGTLink time stamp.
  igtlUint64  GetResolution();

protected:
  StartBindMessage();
  ~StartBindMessage();
  
protected:

  virtual int  CalculateContentBufferSize();
  virtual int  PackContent();
  virtual int  UnpackContent();

  igtlUint64   m_Resolution;

};


/// A class for the STP_BIND message type. The STP_BIND message is sent to the sender
/// of BIND message stream to stop it.
class IGTLCommon_EXPORT StopBindMessage: public MessageBase
{
public:
  igtlTypeMacro(igtl::StopBindMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StopBindMessage);

protected:
  StopBindMessage() : MessageBase() { this->m_SendMessageType  = "STP_BIND"; };
  ~StopBindMessage() {};

protected:
  virtual int  CalculateContentBufferSize() { return 0; };
  virtual int  PackContent()        { AllocateBuffer(); return 1; };
  virtual int  UnpackContent()      { return 1; };

};


/// A class for the RTS_BIND message type. The RTS_BIND message has to be returned 
/// to acknowledge STT_BIND and STP_BIND.
class IGTLCommon_EXPORT RTSBindMessage: public MessageBase
{
public:
  // Status type
  enum {
    STATUS_SUCCESS = 0,
    STATUS_ERROR = 1
  };

  igtlTypeMacro(igtl::RTSBindMessage, igtl::MessageBase);
  igtlNewMacro(igtl::RTSBindMessage);

  /// Sets the status for the start/stop request. 'status' must be either RTSBindMessage::STATUS_SUCCESS or RTSBindMessage::STATUS_ERROR.
  void          SetStatus(igtlUint8 status){ this->m_Status = status; }

  /// Gets the status for the start/stop request. 'status' must be either RTSBindMessage::STATUS_SUCCESS or RTSBindMessage::STATUS_ERROR.
  igtlUint8     GetStatus()                { return this->m_Status; };

protected:
  RTSBindMessage() : MessageBase(), m_Status(0) { this->m_SendMessageType  = "RTS_BIND"; };
  ~RTSBindMessage() {};

  /// Stores the status for the start/stop request.
  igtlUint8 m_Status;

protected:
  virtual int  CalculateContentBufferSize();
  virtual int  PackContent();
  virtual int  UnpackContent();

};



} // namespace igtl

#endif // _igtlBindMessage_h