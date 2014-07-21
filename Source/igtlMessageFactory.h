/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igtlMessageFactory_h
#define __igtlMessageFactory_h

#include "igtlObject.h"
#include "igtlMacro.h"
#include "igtlMessageBase.h"
#include "igtlMessageHeader.h"

#include <set>

namespace igtl
{

class IGTLCommon_EXPORT MessageFactory: public Object
{

public:

  typedef MessageFactory            Self;
  typedef Object                    Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(MessageFactory, Object)
  igtlNewMacro(MessageFactory);

  /// Checks that headerMsg is not null and the headerMsg->GetDeviceType() refers to a valid type, returning true if valid, and false otherwise.
  bool IsValid(igtl::MessageHeader::Pointer headerMsg);

  /// Constructs a message from the given header.
  /// Throws invalid_argument if headerMsg is NULL.
  /// Throws invalid_argument if this->IsValid(headerMsg) returns false.
  /// Creates message, sets header onto message and calls AllocatePack() on the message.
  igtl::MessageBase::Pointer GetMessage(igtl::MessageHeader::Pointer headerMsg);

protected:
  MessageFactory();
  ~MessageFactory();

private:

  std::set<std::string> m_DeviceTypes;

}; // end class

} // end namespace

#endif // __igtlMessageFactory_h
