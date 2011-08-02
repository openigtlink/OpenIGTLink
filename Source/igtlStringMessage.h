/*=========================================================================

  Program:   The OpenIGTLink Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink2_beta/Source/igtlStringMessage.h $
  Language:  C++
  Date:      $Date: 2009-12-16 23:58:02 -0500 (Wed, 16 Dec 2009) $
  Version:   $Revision: 5466 $

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

class IGTLCommon_EXPORT StringMessage: public MessageBase
{
public:
  typedef StringMessage                  Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::StringMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StringMessage);

public:

  int        SetString(const char* string);
  int        SetString(std::string & string);
  int        SetEncoding(igtlUint16 enc);

  const char* GetString();
  igtlUint16  GetEncoding();

protected:
  StringMessage();
  ~StringMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  igtlUint16   m_Encoding;
  
  //BTX
  std::string  m_String;
  //ETX

};


} // namespace igtl

#endif // _igtlStringMessage_h



