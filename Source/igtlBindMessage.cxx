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
    ChildMessageInfo info;
    info.type = child->GetDeviceType();
    info.name = child->GetDeviceName();

    // If the class instance is BindMessage.
    if (strncmp(this->m_DefaultBodyType.c_str(), "BIND", 4) == 0)
      {
      info.size = child->GetPackBodySize();
      info.ptr  = child->GetPackBodyPointer();
      }
    this->m_ChildMessages.push_back(info);
    }
  return this->m_ChildMessages.size();
}


int BindMessageBase::SetChildMessage(unsigned int i, igtl::MessageBase * child)
{
  if (i < this->m_ChildMessages.size())
    {
    this->m_ChildMessages[i].type = child->GetDeviceType();
    this->m_ChildMessages[i].name = child->GetDeviceName();
    // If the class instance is BindMessage.
    if (strncmp(this->m_DefaultBodyType.c_str(), "BIND", 4) == 0)
      {
      this->m_ChildMessages[i].size = child->GetPackBodySize();
      this->m_ChildMessages[i].ptr  = child->GetPackBodyPointer();
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
  this->m_DefaultBodyType = "BIND";
}


BindMessage::~BindMessage()
{
}


int BindMessage::GetChildMessage(unsigned int i, igtl::MessageBase * child)
{
  if (i < this->m_ChildMessages.size())
    {
    child->InitPack();
    igtl_header * header = (igtl_header *) child->GetPackPointer();
    header->version = 1;
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

    child->Unpack();
    child->AllocatePack();

    // TODO: Is there any way to avoid this memory copy?
    memcpy(child->GetPackBodyPointer(),
           this->m_ChildMessages[i].ptr, this->m_ChildMessages[i].size);

    return 1;
    }
  else
    {
    return 0;
    }
}


int BindMessage::GetBodyPackSize()
{
  int size;

  size = sizeof(igtlUint16)  // Number of child messages section
    + (IGTL_HEADER_TYPE_SIZE + sizeof(igtlUint64)) * this->m_ChildMessages.size() // BIND header
    + sizeof (igtlUint16);   // Size of name table section

  std::vector<ChildMessageInfo>::iterator iter;
  for (iter = this->m_ChildMessages.begin(); iter != this->m_ChildMessages.end(); iter ++)
    {
    //size += strlen((*iter)->GetDeviceName()); // Device name length
    size += (*iter).name.length();
    size += 1; // NULL separator
    size += (*iter).size + ((*iter).size%2); // child message body + padding (if applicable)
    }

  return size;
}


int BindMessage::PackBody()
{
  // Allocate pack
  AllocatePack();

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
    
    igtl_bind_pack(&bind_info, this->m_Body, IGTL_TYPE_PREFIX_NONE);
    int nc = this->m_ChildMessages.size();
    size_t bind_size = (size_t) igtl_bind_get_size(&bind_info, IGTL_TYPE_PREFIX_NONE);
    char * ptr = (char *)this->m_Body;
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


int BindMessage::UnpackBody()
{

  igtl_bind_info bind_info;

  if (igtl_bind_unpack(IGTL_TYPE_PREFIX_NONE, (void*)this->m_Body, &bind_info, this->GetPackBodySize()) == 0)
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
  this->m_DefaultBodyType = "GET_BIND";
}


GetBindMessage::~GetBindMessage()
{
}


int GetBindMessage::AppendChildMessage(const char * type, const char * name)
{
  if (strlen(type) < IGTL_HEADER_TYPE_SIZE &&
      strlen(name) < IGTL_HEADER_NAME_SIZE)
    {
    BindMessageBase::ChildMessageInfo info;
    info.type = type;
    info.name = name;
    this->m_ChildMessages.push_back(info);
    }
  return this->m_ChildMessages.size();
}


int GetBindMessage::GetBodyPackSize()
{
  int size;

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


int GetBindMessage::PackBody()
{
  // Allocate pack
  AllocatePack();
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
    
    igtl_bind_pack(&bind_info, this->m_Body, IGTL_TYPE_PREFIX_GET);
    igtl_bind_free_info(&bind_info);

    return 1;
    }
  else
    {
    return 0;
    }
}


int GetBindMessage::UnpackBody()
{

  igtl_bind_info bind_info;

  if (igtl_bind_unpack(IGTL_TYPE_PREFIX_NONE, (void*)this->m_Body, &bind_info, this->GetPackBodySize()) == 0)
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
  this->m_DefaultBodyType = "STT_BIND";
}


StartBindMessage::~StartBindMessage()
{
}


void StartBindMessage::SetResolution(igtlUint64 res)
{
  this->m_Resolution = res; 
}


igtlUint64 StartBindMessage::GetResolution()
{
  return this->m_Resolution;
}


int StartBindMessage::GetBodyPackSize()
{
  if (this->m_ChildMessages.size() == 0)
    {
    // Only a time stamp field is in the message
    return sizeof(igtlUint64);
    }

  return Superclass::GetBodyPackSize() + sizeof(igtlUint64);
}


int StartBindMessage::PackBody()
{
  // Allocate pack
  AllocatePack();
  
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

    igtl_bind_pack(&bind_info, this->m_Body, IGTL_TYPE_PREFIX_STT);
    igtl_bind_free_info(&bind_info);

    return 1;
    }
  else
    {
    return 0;
    }
}


int StartBindMessage::UnpackBody()
{

  igtl_bind_info bind_info;

  if (igtl_bind_unpack(IGTL_TYPE_PREFIX_NONE, (void*)this->m_Body, &bind_info, this->GetPackBodySize()) == 0)
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


int  RTSBindMessage::GetBodyPackSize()
{ 
  return sizeof (igtlUint8);
}

int  RTSBindMessage::PackBody()
{
  AllocatePack(); 

  * (igtlUint8 * )this->m_Body = this->m_Status;

  return 1; 
}


int  RTSBindMessage::UnpackBody()
{ 
  this->m_Status = * (igtlUint8 * )this->m_Body;

  return 1; 
}





} // namespace igtl





