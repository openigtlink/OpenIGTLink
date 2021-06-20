/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlCapabilityMessage_h
#define __igtlCapabilityMessage_h

#include "igtlObject.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

#include <vector>
#include <string>

namespace igtl
{

class IGTLCommon_EXPORT CapabilityMessage: public MessageBase
{

public:
  igtlTypeMacro(igtl::CapabilityMessage, igtl::MessageBase);
  igtlNewMacro(igtl::CapabilityMessage);

public:
  void                      SetTypes(std::vector<std::string> types);
  int                       SetType(int id, const char* name);
  const char*               GetType(int id);
  
  void                      SetNumberOfTypes(int n) { m_TypeNames.resize(n); }
  int                       GetNumberOfTypes() { return m_TypeNames.size(); }
  std::vector<std::string>  GetTypes() { return m_TypeNames; }

protected:
  CapabilityMessage();
  ~CapabilityMessage();

protected:

  igtlUint64 CalculateContentBufferSize() override;
  int  PackContent() override;
  int  UnpackContent() override;

  std::vector<std::string>  m_TypeNames;

};


} // namespace igtl

#endif // __igtlCapabilityMessage_h
