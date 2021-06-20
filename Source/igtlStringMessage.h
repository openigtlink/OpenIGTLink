/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlStringMessage_h
#define __igtlStringMessage_h

#include <string>

#include "igtlObject.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

#define IGTL_STRING_MESSAGE_DEFAULT_ENCODING 3 /* Default encoding -- ANSI-X3.5-1968 */

namespace igtl
{

/// THe STRING message type is used for transferring a character string. It supports character strings up to 65535 bytes.
class IGTLCommon_EXPORT StringMessage: public MessageBase
{
public:
  igtlTypeMacro(igtl::StringMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StringMessage);

public:

  /// Sets the string by character array.
  int        SetString(const char* string);

  /// Sets the string by std::string.
  int        SetString(const std::string & string);

  /// Sets the encoding of the string. For character encoding, please refer IANA Character Sets
  /// (http://www.iana.org/assignments/character-sets).
  /// US-ASCII (ANSI-X3.4-1968; MIBenum = 3) is strongly recommended.
  int        SetEncoding(igtlUint16 enc);

  /// Gets the string.
  const char* GetString();

  /// Gets the encoding of the string. The returned value is defined in
  /// IANA Character Sets (http://www.iana.org/assignments/character-sets).
  igtlUint16  GetEncoding();

protected:
  StringMessage();
  ~StringMessage() override;
  
protected:

  igtlUint64 CalculateContentBufferSize() override;
  int  PackContent() override;
  int  UnpackContent() override;
  
  /// The encoding of the string.
  /// The value is defined in IANA Character Sets (http://www.iana.org/assignments/character-sets).
  igtlUint16   m_Encoding;

  /// The string.
  std::string  m_String;

};


} // namespace igtl

#endif // _igtlStringMessage_h



