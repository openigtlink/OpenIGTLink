/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlCommandMessage_h
#define __igtlCommandMessage_h

#include "igtlObject.h"
#include "igtlMacro.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtl_command.h"

namespace igtl
{

class IGTLCommon_EXPORT CommandMessage : public MessageBase
{
public:
  static const int validEncodings[257];

public:
  igtlTypeMacro(igtl::CommandMessage, igtl::MessageBase)
  igtlNewMacro(igtl::CommandMessage);

public:

  /// Sets the command ID
  int        SetCommandId(igtlUint32 aId);

  /// Sets the command name
  int        SetCommandName(const char* aCommandName);

  /// Sets the command name by std::string
  int        SetCommandName(const std::string& aCommandName);

  /// Sets the string by character array.
  int        SetCommandContent(const char* string);

  /// Sets the string by std::string.
  int        SetCommandContent(const std::string & string);

  /// Sets the encoding of the string. For character encoding, please refer IANA Character Sets
  /// (http://www.iana.org/assignments/character-sets).
  /// US-ASCII (ANSI-X3.4-1968; MIBenum = 3) is strongly recommended.
  int        SetContentEncoding(igtlUint16 enc);

  /// Gets the command ID
  igtlUint32  GetCommandId() const;

  /// Get the command name
  std::string GetCommandName() const;

  /// Gets the string.
  std::string GetCommandContent() const;

  /// Gets the length of the command content
  igtlUint32 GetCommandContentLength() const;

  /// Gets the encoding of the string. The returned value is defined in
  /// IANA Character Sets (http://www.iana.org/assignments/character-sets).
  igtlUint16  GetContentEncoding() const;

protected:
  CommandMessage();
  ~CommandMessage();

protected:
  virtual int  CalculateContentBufferSize();
  virtual int  PackContent();
  virtual int  UnpackContent();

  /// The unique ID of the command
  igtlUint32   m_CommandId;

  /// The name of the command
  igtlUint8    m_CommandName[IGTL_COMMAND_NAME_SIZE];

  /// The encoding of the command string.
  /// The value is defined in IANA Character Sets (http://www.iana.org/assignments/character-sets).
  igtlUint16   m_Encoding;

  /// The command stored as an XML encoded string.
  std::string  m_Command;
};

class IGTLCommon_EXPORT RTSCommandMessage : public CommandMessage
{
public:
  igtlTypeMacro(igtl::RTSCommandMessage, igtl::CommandMessage)
  igtlNewMacro(igtl::RTSCommandMessage);

public:

  /// Sets the error string
  int        SetCommandErrorString(const char* aCommandName);

  /// Sets the error string by std::string
  int        SetCommandErrorString(const std::string& aCommandName);

  /// Get the error string
  std::string GetCommandErrorString() const;

protected:
  RTSCommandMessage();
  ~RTSCommandMessage();
};

} // namespace igtl

#endif // _igtlCommandMessage_h