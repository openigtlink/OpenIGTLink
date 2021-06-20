/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlQueryMessage_h
#define __igtlQueryMessage_h

#include <string>

#include "igtlObject.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"
#include "igtl_query.h"

namespace igtl
{

/// THe STRING message type is used for transferring a character string. It supports character strings up to 65535 bytes.
class IGTLCommon_EXPORT QueryMessage: public MessageBase
{
public:
  igtlTypeMacro(igtl::QueryMessage, igtl::MessageBase);
  igtlNewMacro(igtl::QueryMessage);

public:

  /// Sets the string by character array.
  int        SetDeviceUID(const char* string);

  /// Sets the string by std::string.
  int        SetDeviceUID(const std::string & string);
  
  /// Sets the string by character array.
  int        SetDataType(const char* string);
  
  /// Sets the string by std::string.
  int        SetDataType(const std::string & string);

  /// Gets the device uid.
  std::string GetDeviceUID();
  
  /// Gets the data type.
  std::string GetDataType();
  
protected:
  QueryMessage();
  ~QueryMessage();
  
protected:

  igtlUint64 CalculateContentBufferSize() override;
  int  PackContent() override;
  int  UnpackContent() override;

  /// The string.
  std::string  m_DeviceUID;
  
  igtlUint32   m_QueryID;

  /// Query Data Type
  igtlUint8  m_DataType[IGTL_QUERY_DATE_TYPE_SIZE ];
};


} // namespace igtl

#endif // _igtlQueryMessage_h



