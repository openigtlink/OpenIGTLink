/*=========================================================================

  Program:   The OpenIGTLink Library
  Module:
  Language:  C++
  Date:
  Version:

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlCapabilityMessage.h"

#include <igtlTypes.h>

#include <igtl_header.h>
#include <igtl_capability.h>
#include <igtlOSUtil.h>

#include <vector>
#include <string>

// Disable warning C4996 (strncpy() may be unsafe) in Windows.
#define _CRT_SECURE_NO_WARNINGS

#include <string.h>

namespace igtl {

CapabilityMessage::CapabilityMessage():
    MessageBase()
{
  this->m_SendMessageType = "CAPABILITY";
  this->m_TypeNames.clear();
/// CapabilityMessage stay the same as previous versions, set m_Version = 1
/// to make the pack and unpack procedures the same as OpenIGTLink_PROTOCOL_VERSION 1
#if OpenIGTLink_HEADER_VERSION >= 2
  m_HeaderVersion = IGTL_HEADER_VERSION_1;
#endif
}


CapabilityMessage::~CapabilityMessage()
{
  this->m_TypeNames.clear();
}

void CapabilityMessage::SetTypes(std::vector<std::string> types)
{
  m_IsBodyPacked = false;
  this->m_TypeNames.clear();
  this->m_TypeNames = types;
}


int CapabilityMessage::SetType(int id, const char* type)
{
  if (id < (int)this->m_TypeNames.size() && strlen(type) < IGTL_HEADER_TYPE_SIZE)
    {
    m_IsBodyPacked = false;
    this->m_TypeNames[id] = type;
    return 1;
    }
  else
    {
    return 0;
    }
}


const char* CapabilityMessage::GetType(int id)
{
  if (id < (int)this->m_TypeNames.size())
    {
    return this->m_TypeNames[id].c_str();
    }
  else
    {
    return "";
    }
}


igtlUint64 CapabilityMessage::CalculateContentBufferSize()
{
  return (sizeof(char) * IGTL_HEADER_TYPE_SIZE * this->m_TypeNames.size());
}


int CapabilityMessage::PackContent()
{
  AllocateBuffer();

  if (this->m_TypeNames.size() == 0)
  {
    return 0;
  }

  igtl_capability_info info;
  int nTypes = this->m_TypeNames.size();

  igtl_capability_init_info(&info);
  info.ntypes = nTypes;
  igtl_capability_alloc_info(&info, nTypes);

  for(int i = 0; i < nTypes; i++ )
    {
    memcpy(info.typenames[i], this->m_TypeNames[i].c_str(), IGTL_HEADER_TYPE_SIZE);
    }

  igtl_capability_pack(&info, this->m_Content);

  return 1;
}


int CapabilityMessage::UnpackContent()
{

  igtl_capability_info info;

  igtl_capability_init_info(&info);
  bool isUnpacked(true);
  igtl_capability_unpack(this->m_Content, &info, this->CalculateReceiveContentSize(isUnpacked));

  int ntypes = info.ntypes;

  if(ntypes == 0)
    {
    return 0;
    }

  this->m_TypeNames.clear();
  for(int i = 0; i < ntypes; i++)
    {
    std::string buf;
    if (igtl::Strnlen((const char*)info.typenames[i], IGTL_HEADER_TYPE_SIZE) < IGTL_HEADER_TYPE_SIZE)
      {
      buf.append((const char*)info.typenames[i]);
      }
    else
      {
      buf.append((const char*)info.typenames[i], IGTL_HEADER_TYPE_SIZE);
      }
    this->m_TypeNames.push_back(buf);
    }
  

  return 1;
}


} // namespace igtl
