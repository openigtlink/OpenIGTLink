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

#include <igtlObject.h>
#include <igtlMath.h>
#include <igtlMessageBase.h>
#include <igtlTypes.h>

#include <vector>
#include <string>

namespace igtl
{

class IGTLCommon_EXPORT GetCapabilityMessage: public MessageBase
{
public:
  typedef GetCapabilityMessage            Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::GetCapabilityMessage, igtl::MessageBase);
  igtlNewMacro(igtl::GetCapabilityMessage);

protected:
  GetCapabilityMessage() : MessageBase() { this->m_DefaultBodyType  = "GET_CAPABIL"; };
  ~GetCapabilityMessage() {};
protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };
};


class IGTLCommon_EXPORT CapabilityMessage: public MessageBase
{

public:

  typedef CapabilityMessage          Self;
  typedef MessageBase                Superclass;
  typedef SmartPointer<Self>         Pointer;
  typedef SmartPointer<const Self>   ConstPointer;

  igtlTypeMacro(igtl::CapabilityMessage, igtl::MessageBase);
  igtlNewMacro(igtl::CapabilityMessage);

public:

  //void    SetTypes(int ntypes, const char names[][IGTL_HEADER_TYPE_SIZE]);
  void    SetTypes(std::vector<std::string> types);
  int     SetType(int id, const char* name);
  const char* GetType(int id);
  
  void     SetNumberOfTypes(int n) { m_TypeNames.resize(n); }
  int     GetNumberOfTypes() { return m_TypeNames.size(); }
  //char**  GetTypeNames() { return m_TypeNames; }
  std::vector<std::string> GetTypes() { return m_TypeNames; }

protected:
  CapabilityMessage();
  ~CapabilityMessage();

protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

  std::vector<std::string>  m_TypeNames;

};


} // namespace igtl

#endif // __igtlCapabilityMessage_h
