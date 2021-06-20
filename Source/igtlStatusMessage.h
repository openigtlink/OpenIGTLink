/*=========================================================================

  Program:   The OpenIGTLinkLibrary
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlStatusMessage_h
#define __igtlStatusMessage_h

#include "igtlObject.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

namespace igtl
{


/// A class for the GET_STATUS message type.
class IGTLCommon_EXPORT GetStatusMessage: public HeaderOnlyMessageBase
{
public:
  igtlTypeMacro(igtl::GetStatusMessage, igtl::HeaderOnlyMessageBase);
  igtlNewMacro(igtl::GetStatusMessage);

protected:
  GetStatusMessage() : HeaderOnlyMessageBase() { this->m_SendMessageType  = "GET_STATUS"; };
  ~GetStatusMessage() {};
};


/// The STATUS data type is used to notify the receiver about the current status of the sender.
/// The data consist of status code in a 16-bit unsigned integer, sub code in a 64-bit integer,
/// error name in a 20-byte-length character string, and a status message. The length of
/// the status message is determined by the size information in the general header.
/// The status code is defined as a part of the OpenIGTLink protocol specification listed
/// bellow. The sub code is device specific and is defined by developers. In addition,
/// developers can build their own error name/code into the status message and additional
/// optional description in the following data field.
class IGTLCommon_EXPORT StatusMessage: public MessageBase
{
public:
  igtlTypeMacro(igtl::StatusMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StatusMessage);

public:

  /// Status codes -- see igtl_status.h
  enum {
    STATUS_INVALID             = 0,
    STATUS_OK                  = 1,
    STATUS_UNKNOWN_ERROR       = 2,
    STATUS_PANICK_MODE         = 3,  /* emergency */
    STATUS_NOT_FOUND           = 4,  /* file, configuration, device etc */
    STATUS_ACCESS_DENIED       = 5,
    STATUS_BUSY                = 6,
    STATUS_TIME_OUT            = 7,  /* Time out / Connection lost */
    STATUS_OVERFLOW            = 8,  /* Overflow / Can't be reached */
    STATUS_CHECKSUM_ERROR      = 9,  /* Checksum error */
    STATUS_CONFIG_ERROR        = 10, /* Configuration error */
    STATUS_RESOURCE_ERROR      = 11, /* Not enough resource (memory, storage etc) */
    STATUS_UNKNOWN_INSTRUCTION = 12, /* Illegal/Unknown instruction */
    STATUS_NOT_READY           = 13, /* Device not ready (starting up)*/
    STATUS_MANUAL_MODE         = 14, /* Manual mode (device does not accept commands) */
    STATUS_DISABLED            = 15, /* Device disabled */
    STATUS_NOT_PRESENT         = 16, /* Device not present */
    STATUS_UNKNOWN_VERSION     = 17, /* Device version not known */
    STATUS_HARDWARE_FAILURE    = 18, /* Hardware failure */
    STATUS_SHUT_DOWN           = 19, /* Exiting / shut down in progress */
    STATUS_NUM_TYPES           = 20
  };

public:

  /// Sets the status code.
  void        SetCode(int code);

  /// Gets the status code.
  int         GetCode();

  /// Sets the sub code. 
  void        SetSubCode(igtlInt64 subcode);

  /// Gets the sub code.
  igtlInt64   GetSubCode();

  /// Sets the error name. The error name can be defined by a developer.
  void        SetErrorName(const char* name);

  /// Gets the error name. 
  const char* GetErrorName();

  /// Sets the status string.
  void        SetStatusString(const char* str);

  /// Gets the status string.
  const char* GetStatusString();

protected:
  StatusMessage();
  ~StatusMessage();
  
protected:

  igtlUint64 CalculateContentBufferSize() override;
  int  PackContent() override;
  int  UnpackContent() override;

  /// The error code.
  igtlUint16   m_Code;

  /// The sub code.
  igtlInt64    m_SubCode;

  /// The error name.
  char         m_ErrorName[20];
  
  /// The status message string.
  std::string  m_StatusMessageString;

  /// A pointer to the byte array of the status header.
  unsigned char*  m_StatusHeader;

  /// A pointer to the byte array of the status message.
  char*           m_StatusMessage;

};


} // namespace igtl

#endif // _igtlStatusMessage_h