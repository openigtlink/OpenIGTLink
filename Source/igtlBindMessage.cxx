/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlBindMessage.h"

#include "igtl_header.h"
#include "igtl_bind.h"

#include <string.h>

namespace igtl {



BindMessageBase::BindMessageBase():
  MessageBase()
{
  Init();
}


BindMessageBase::~BindMessageBase()
{
}

void BindMessageBase::Init()
{
  this->m_ChildMessages.clear();
}


int BindMessageBase::SetNumberOfChildMessages(unsigned int n)
{
  m_IsBodyPacked = false;
  this->m_ChildMessages.resize(n);
  return this->m_ChildMessages.size();
}


int BindMessageBase::GetNumberOfChildMessages()
{
  return this->m_ChildMessages.size();
}


int BindMessageBase::AppendChildMessage(igtl::MessageBase * child)
{
  if (this->m_ChildMessages.size() < 0xFFFF)
    {
    m_IsBodyPacked = false;
    ChildMessageInfo info;
#if OpenIGTLink_HEADER_VERSION >= 2
    info.type = child->GetMessageType();
#else
    info.type = child->GetDeviceType();
#endif
    info.name = child->GetDeviceName();

    // If the class instance is BindMessage.
    if (strncmp(this->m_SendMessageType.c_str(), "BIND", 4) == 0)
      {
      info.size = child->GetBufferBodySize();
      info.ptr  = child->GetBufferBodyPointer();
      }
    this->m_ChildMessages.push_back(info);
    }
  return this->m_ChildMessages.size();
}


int BindMessageBase::SetChildMessage(unsigned int i, igtl::MessageBase * child)
{
  if (i < this->m_ChildMessages.size())
    {
    m_IsBodyPacked = false;
#if OpenIGTLink_HEADER_VERSION >= 2
    this->m_ChildMessages[i].type = child->GetMessageType();
#else
    this->m_ChildMessages[i].type = child->GetDeviceType();
#endif
    this->m_ChildMessages[i].name = child->GetDeviceName();
    // If the class instance is BindMessage.
    if (strncmp(this->m_SendMessageType.c_str(), "BIND", 4) == 0)
      {
      this->m_ChildMessages[i].size = child->GetBufferBodySize();
      this->m_ChildMessages[i].ptr  = child->GetBufferBodyPointer();
      }
    return 1;
    }
  else
    {
    return 0;
    }
}


const char* BindMessageBase::GetChildMessageType(unsigned int i)
{
  if (i < this->m_ChildMessages.size())
    {
    return this->m_ChildMessages[i].type.c_str();
    }
  else
    {
    return NULL;
    }
}



BindMessage::BindMessage():
  BindMessageBase()
{
  this->m_SendMessageType = "BIND";
}


BindMessage::~BindMessage()
{
}


int BindMessage::GetChildMessage(unsigned int i, igtl::MessageBase * child)
{
  if (i < this->m_ChildMessages.size())
    {
    child->InitBuffer();
    igtl_header * header = (igtl_header *) child->GetBufferPointer();
    header->header_version = 1;
    strncpy( header->name, this->m_ChildMessages[i].type.c_str(),  IGTL_HEADER_TYPE_SIZE);
    strncpy( header->device_name, this->m_ChildMessages[i].name.c_str(), IGTL_HEADER_NAME_SIZE);

    // Time stamp -- same as the bind message
    igtl_uint64 ts  =  m_TimeStampSec & 0xFFFFFFFF;
    ts = (ts << 32) | (m_TimeStampSecFraction & 0xFFFFFFFF);
    header->timestamp = ts;
    header->body_size = this->m_ChildMessages[i].size;
    header->crc = 0;

    // Convert to network byte order
    igtl_header_convert_byte_order(header);
    igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
    headerMsg->AllocatePack();
    memcpy(headerMsg->GetPackPointer(), header, IGTL_HEADER_SIZE);
    headerMsg->Unpack();
    child->SetMessageHeader(headerMsg);
    child->AllocateBuffer();
    // TODO: Is there any way to avoid this memory copy?
    memcpy(child->GetBufferBodyPointer(),
           this->m_ChildMessages[i].ptr, this->m_ChildMessages[i].size);
  
    child->Unpack();
    return 1;
    }
  else
    {
    return 0;
    }
}


igtlUint64 BindMessage::CalculateContentBufferSize()
{
  igtlUint64 size;
  igtlUint64 nameTableSectionSize = 0; // Size of name table section
  igtlUint64 dataSectionSize = 0; // Size of data section

  size = sizeof(igtlUint16)  // Number of child messages section
    + (IGTL_HEADER_TYPE_SIZE + sizeof(igtlUint64)) * this->m_ChildMessages.size() // BIND header
    + sizeof (igtlUint16);   // Size of name table section

  std::vector<ChildMessageInfo>::iterator iter;
  for (iter = this->m_ChildMessages.begin(); iter != this->m_ChildMessages.end(); iter ++)
    {
    nameTableSectionSize += (*iter).name.length();
    nameTableSectionSize += 1; // NULL separator
    dataSectionSize += (*iter).size + ((*iter).size%2); // child message body + padding (if applicable)
    }

  // Add padding for the whole name table section
  if (nameTableSectionSize % 2 > 0)
    {
    nameTableSectionSize++;
    }
  size += nameTableSectionSize;
  size += dataSectionSize;

  return size;
}


int BindMessage::PackContent()
{
  // Allocate buffer
  AllocateBuffer();

  igtl_bind_info bind_info;
  igtl_bind_init_info(&bind_info);
  
  if (igtl_bind_alloc_info(&bind_info, this->m_ChildMessages.size()))
    {
    // TODO: using c library causes additional data copy (C++ member variable to c-structure,
    // then to pack byte array). Probably, it's good idea to implement PackBody() without
    // using c APIs.
    int i = 0;
    std::vector<ChildMessageInfo>::iterator iter;
    for (iter = this->m_ChildMessages.begin(); iter != this->m_ChildMessages.end(); iter ++)
      {
      strncpy(bind_info.child_info_array[i].type, (*iter).type.c_str(), IGTL_HEADER_TYPE_SIZE);
      strncpy(bind_info.child_info_array[i].name, (*iter).name.c_str(), IGTL_HEADER_NAME_SIZE);
      bind_info.child_info_array[i].size = (*iter).size;
      bind_info.child_info_array[i].ptr = (*iter).ptr;

      i ++;
      }
    
    igtl_bind_pack(&bind_info, this->m_Content, IGTL_TYPE_PREFIX_NONE);
    int nc = this->m_ChildMessages.size();
    size_t bind_size = (size_t) igtl_bind_get_size(&bind_info, IGTL_TYPE_PREFIX_NONE);
    char * ptr = (char *)this->m_Content;
    ptr = ptr + bind_size;
    for (int i = 0; i < nc; i ++)
      {
      memcpy((void*)ptr, bind_info.child_info_array[i].ptr, bind_info.child_info_array[i].size);
      ptr += bind_info.child_info_array[i].size;
      /* Note: a padding byte is added, if the size of the child message body
         is odd. */
      if (bind_info.child_info_array[i].size % 2)
        {
        *ptr = '\0';
        ptr ++;
        }
      }
    
    igtl_bind_free_info(&bind_info);   // TODO: calling igtl_bind_free_info() after igtl_bind_pack() causes 
                                       // this causes segmentation fault on Linux... why?
    return 1;
    }
  else
    {
    return 0;
    }
}


int BindMessage::UnpackContent()
{

  igtl_bind_info bind_info;

  if (igtl_bind_unpack(IGTL_TYPE_PREFIX_NONE, (void*)this->m_Content, &bind_info, this->GetBufferBodySize()) == 0)
    {
    return 0;
    }

  int n = bind_info.ncmessages;

  Init();

  for (int i = 0; i < n; i ++)
    {
    ChildMessageInfo info;

    info.type = bind_info.child_info_array[i].type;
    info.name = bind_info.child_info_array[i].name;
    info.size = bind_info.child_info_array[i].size;
    info.ptr  = bind_info.child_info_array[i].ptr;

    this->m_ChildMessages.push_back(info);

    }

  return 1;
}



GetBindMessage::GetBindMessage():
  BindMessageBase()
{
  this->m_SendMessageType = "GET_BIND";
}


GetBindMessage::~GetBindMessage()
{
}


int GetBindMessage::AppendChildMessage(const char * type, const char * name)
{
  if (strlen(type) < IGTL_HEADER_TYPE_SIZE &&
      strlen(name) < IGTL_HEADER_NAME_SIZE)
    {
    m_IsBodyPacked = false;
    BindMessageBase::ChildMessageInfo info;
    info.type = type;
    info.name = name;
    this->m_ChildMessages.push_back(info);
    }
  return this->m_ChildMessages.size();
}


igtlUint64 GetBindMessage::CalculateContentBufferSize()
{
  igtlUint64 size;

  size = sizeof(igtlUint16)  // Number of child messages section
    + IGTL_HEADER_TYPE_SIZE * this->m_ChildMessages.size() // BIND header
    + sizeof (igtlUint16);   // Size of name table section

  std::vector<ChildMessageInfo>::iterator iter;
  for (iter = this->m_ChildMessages.begin(); iter != this->m_ChildMessages.end(); iter ++)
    {
    size += (*iter).name.length();
    size += 1; // NULL separator
    }

  return size;
}


int GetBindMessage::PackContent()
{
  // Allocate buffer
  AllocateBuffer();
  igtl_bind_info bind_info;
  igtl_bind_init_info(&bind_info);
  
  if (igtl_bind_alloc_info(&bind_info, this->m_ChildMessages.size()))
    {
    // TODO: using c library causes additional data copy (C++ member variable to c-structure,
    // then to pack byte array). Probably, it's good idea to implement PackBody() without
    // using c APIs.
    int i = 0;
    std::vector<ChildMessageInfo>::iterator iter;
    for (iter = this->m_ChildMessages.begin(); iter != this->m_ChildMessages.end(); iter ++)
      {
      strncpy(bind_info.child_info_array[i].type, (*iter).type.c_str(), IGTL_HEADER_TYPE_SIZE);
      strncpy(bind_info.child_info_array[i].name, (*iter).name.c_str(), IGTL_HEADER_NAME_SIZE);
      bind_info.child_info_array[i].size = 0;
      bind_info.child_info_array[i].ptr = NULL;
      i ++;
      }
    
    igtl_bind_pack(&bind_info, this->m_Content, IGTL_TYPE_PREFIX_GET);
    igtl_bind_free_info(&bind_info);

    return 1;
    }
  else
    {
    return 0;
    }
}


int GetBindMessage::UnpackContent()
{

  igtl_bind_info bind_info;

  if (igtl_bind_unpack(IGTL_TYPE_PREFIX_NONE, (void*)this->m_Content, &bind_info, this->GetBufferBodySize()) == 0)
    {
    return 0;
    }

  int n = bind_info.ncmessages;

  Init();

  for (int i = 0; i < n; i ++)
    {
    ChildMessageInfo info;

    info.type = bind_info.child_info_array[i].type;
    info.name = bind_info.child_info_array[i].name;
    info.size = 0;
    info.ptr  = NULL;

    this->m_ChildMessages.push_back(info);

    }

  return 1;
}


StartBindMessage::StartBindMessage():
  GetBindMessage()
{
  this->m_SendMessageType = "STT_BIND";
}


StartBindMessage::~StartBindMessage()
{
}


void StartBindMessage::SetResolution(igtlUint64 res)
{
  m_IsBodyPacked = false;
  this->m_Resolution = res; 
}


igtlUint64 StartBindMessage::GetResolution()
{
  return this->m_Resolution;
}


igtlUint64 StartBindMessage::CalculateContentBufferSize()
{
  if (this->m_ChildMessages.size() == 0)
    {
    // Only a time stamp field is in the message
    return sizeof(igtlUint64);
    }

  return Superclass::CalculateContentBufferSize() + sizeof(igtlUint64);
}


int StartBindMessage::PackContent()
{
  // Allocate buffer
  AllocateBuffer();
  
  igtl_bind_info bind_info;
  igtl_bind_init_info(&bind_info);

  // If there is no child message information in the class instance,
  // the message request any available messages
  if (this->m_ChildMessages.size() == 0)
    {
    bind_info.request_all = 1;
    }
  
  if (igtl_bind_alloc_info(&bind_info, this->m_ChildMessages.size()))
    {
    // TODO: using c library causes additional data copy (C++ member variable to c-structure,
    // then to pack byte array). Probably, it's good idea to implement PackBody() without
    // using c APIs.
    int i = 0;
    std::vector<ChildMessageInfo>::iterator iter;
    for (iter = this->m_ChildMessages.begin(); iter != this->m_ChildMessages.end(); iter ++)
      {
      strncpy(bind_info.child_info_array[i].type, (*iter).type.c_str(), IGTL_HEADER_TYPE_SIZE);
      strncpy(bind_info.child_info_array[i].name, (*iter).name.c_str(), IGTL_HEADER_NAME_SIZE);
      bind_info.child_info_array[i].size = 0;
      bind_info.child_info_array[i].ptr = NULL;
      i ++;
      }
    
    bind_info.resol = this->m_Resolution;

    igtl_bind_pack(&bind_info, this->m_Content, IGTL_TYPE_PREFIX_STT);
    igtl_bind_free_info(&bind_info);

    return 1;
    }
  else
    {
    return 0;
    }
}


int StartBindMessage::UnpackContent()
{

  igtl_bind_info bind_info;

  if (igtl_bind_unpack(IGTL_TYPE_PREFIX_NONE, (void*)this->m_Content, &bind_info, this->GetBufferBodySize()) == 0)
    {
    return 0;
    }

  int n = bind_info.ncmessages;

  this->m_Resolution = bind_info.resol;

  Init();

  for (int i = 0; i < n; i ++)
    {
    ChildMessageInfo info;

    info.type = bind_info.child_info_array[i].type;
    info.name = bind_info.child_info_array[i].name;
    info.size = 0;
    info.ptr  = NULL;

    this->m_ChildMessages.push_back(info);

    }

  return 1;
}


igtlUint64  RTSBindMessage::CalculateContentBufferSize()
{ 
  return sizeof (igtlUint8);
}

int  RTSBindMessage::PackContent()
{
  AllocateBuffer(); 

  * (igtlUint8 * )this->m_Content = this->m_Status;

  return 1; 
}


int  RTSBindMessage::UnpackContent()
{ 
  this->m_Status = * (igtlUint8 * )this->m_Content;

  return 1; 
}





} // namespace igtl
