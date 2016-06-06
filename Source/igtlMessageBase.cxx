/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <string.h>
#include "igtlMessageBase.h"
#include "igtl_util.h"
#include "igtl_header.h"

#include <string>

namespace igtl {

MessageBase::MessageBase():
  Object()
{
  m_Version        = IGTL_HEADER_VERSION_1;
  m_PackSize       = 0;
  m_Header         = NULL;
  m_Body           = NULL;

  m_BodySizeToRead = 0;
  m_DeviceName     = "";

  m_IsHeaderUnpacked = 0;
  m_IsBodyUnpacked   = 0;

  m_BodyType         = "";
  m_DefaultBodyType  = "";
#if OpenIGTLink_PROTOCOL_VERSION >= 3
  m_Version = IGTL_HEADER_VERSION_3;
  metaDataTotalSize = 2;
  indexCount= 0;
  m_ExtendedHeader = NULL;
  m_MetaData = NULL;
  keySize.clear();
  valueEncoding.clear();
  valueSize.clear();
  keys.clear();
  values.clear();
#endif
}

MessageBase::~MessageBase()
{
  if (this->m_PackSize > 0 && this->m_Header != NULL)
  {
    delete [] m_Header;
    m_PackSize = 0;
    m_Header = NULL;
    m_Body   = NULL;
  }
}

void MessageBase::SetVersion(unsigned short version)
{
  m_Version = version;
}

unsigned short MessageBase::GetVersion() const
{
  return m_Version;
}

void MessageBase::SetDeviceName(const char* name)
{
  m_DeviceName = std::string(name);
}

void MessageBase::SetDeviceName(const std::string& name)
{
  m_DeviceName = name;
}

void MessageBase::SetDeviceType(const std::string& type)
{
  this->m_BodyType = type;
}

std::string MessageBase::GetDeviceName() const
{
  return m_DeviceName;
}

const char* MessageBase::GetDeviceName()
{
  return m_DeviceName.c_str();
}

const char* MessageBase::GetDeviceType()
{
  if (m_DefaultBodyType.length() > 0)
    {
    return m_DefaultBodyType.c_str();
    }
  else
    {
    return m_BodyType.c_str();
    }
}

#if OpenIGTLink_PROTOCOL_VERSION >= 3
std::string MessageBase::GetMessageType() const
{
  if (m_DefaultBodyType.length() > 0)
  {
    return m_DefaultBodyType;
  }
  else
  {
    return m_BodyType;
  }
}

  
int MessageBase::AddMetaDataElement(std::string key, std::string value)
{
  igtlUint16 keyLength = key.length();
  this->keySize.push_back(keyLength);
  this->valueEncoding.push_back(0);
  igtlUint16 valueLength = value.length();
  this->valueSize.push_back(valueLength);
  this->keys.push_back(key);
  this->values.push_back(value);
  this->indexCount = this->keySize.size();
  metaDataTotalSize += keyLength + valueLength + 8;
  return 1;
}

void MessageBase::PackMetaData()
{
  // For the Get_, Start_, Stop_ .... command messages, the m_Version could be IGTL_HEADER_VERSION_3 for
  // communication purpose, however the meta data is none. so use GetBodyPackSize() > 0 condition to
  // stop the packMetaData() from excuting.
  if (m_Version == IGTL_HEADER_VERSION_3 && m_ExtendedHeader != NULL && GetBodyPackSize()>0)
  {
    igtl_extended_header* extended_header = (igtl_extended_header*) m_ExtendedHeader;
    extended_header->extended_header_size = IGTL_EXTENDED_HEADER_SIZE;
    extended_header->meta_data_size       = this->GetMetaDataSize();
    extended_header->msg_id               = 0;
    extended_header->reserved             = 0;
    igtl_extended_header_convert_byte_order(extended_header);
    igtl_uint16 index_count = this->indexCount; // first two byte are the total number of meta data
    if(igtl_is_little_endian())
    {
      index_count = BYTE_SWAP_INT16(index_count);
    }
    m_MetaData = &m_Body[GetBodyPackSize()-this->metaDataTotalSize];
    memcpy(m_MetaData, &index_count, 2);
    igtl_uint16 key_size = 0;
    igtl_uint16 value_encoding = 0;
    igtl_uint32 value_size = 0;
    for (int i = 0; i < this->indexCount; i++)
    {
      key_size = this->keySize[i];
      value_encoding = this->valueEncoding[i];
      value_size = this->valueSize[i];
      if(igtl_is_little_endian())
      {
        key_size = BYTE_SWAP_INT16(key_size);
        value_encoding = BYTE_SWAP_INT16(value_encoding);
        value_size = BYTE_SWAP_INT32(value_size);
      }
      memcpy(m_MetaData+2+i*8, &key_size, 2);
      memcpy(m_MetaData+4+i*8, &value_encoding, 2);
      memcpy(m_MetaData+6+i*8, &value_size, 4);
    }
    std::string key= "";
    std::string value= "";
    int stride = 2+this->indexCount*8;
    for (int i = 0; i < this->indexCount; i++)
    {
      memcpy(m_MetaData+stride, this->keys[i].c_str(), this->keySize[i]);
      stride += this->keySize[i];
      memcpy(m_MetaData+stride, this->values[i].c_str(), this->valueSize[i]);
      stride += this->valueSize[i];
    }
  }
}
void MessageBase::UnpackExtendedHeader()
{
  if (m_Version == IGTL_HEADER_VERSION_3)
  {
    m_ExtendedHeader = m_Body;
    igtl_extended_header* extended_header = (igtl_extended_header*)m_ExtendedHeader;
    igtl_extended_header_convert_byte_order(extended_header);
    this->metaDataTotalSize   = extended_header->meta_data_size;
    this->msgId   = extended_header->msg_id;
  }
}
  
void MessageBase::UnpackMetaData()
{
  if (m_Version == IGTL_HEADER_VERSION_3)
  {
    m_MetaData = &m_Body[GetPackBodySize()-this->metaDataTotalSize];
    igtl_uint16 index_count = 0; // first two byte are the total number of meta data
    memcpy(&index_count, m_MetaData, 2);
    if(igtl_is_little_endian())
    {
      index_count = BYTE_SWAP_INT16(index_count);
    }
    this->indexCount = index_count;
    this->keys.resize(index_count);
    this->values.resize(index_count);
    this->keySize.resize(index_count);
    this->valueSize.resize(index_count);
    this->valueEncoding.resize(index_count);
    igtl_uint16 key_size = 0;
    igtl_uint16 value_encoding = 0;
    igtl_uint32 value_size = 0;
    for (int i = 0; i < this->indexCount; i++)
    {
      
      memcpy(&key_size, m_MetaData+2+i*8, 2);
      memcpy(&value_encoding, m_MetaData+4+i*8, 2);
      memcpy(&value_size, m_MetaData+6+i*8, 4);
      if(igtl_is_little_endian())
      {
        key_size = BYTE_SWAP_INT16(key_size);
        value_encoding = BYTE_SWAP_INT16(value_encoding);
        value_size = BYTE_SWAP_INT32(value_size);
      }
      this->keySize[i] = key_size;
      this->valueEncoding[i] = value_encoding;
      this->valueSize[i] = value_size;
    }
    int stride = 2+this->indexCount*8;
    for (int i = 0; i < this->indexCount; i++)
    {
      this->keys[i].assign(m_MetaData+stride, m_MetaData+stride+this->keySize[i]);
      stride += this->keySize[i];
      this->values[i].assign(m_MetaData+stride, m_MetaData+stride+this->valueSize[i]);
      stride += this->valueSize[i];
    }
  }
}
#endif


int MessageBase::SetTimeStamp(unsigned int sec, unsigned int frac)
{
  m_TimeStampSec         = sec;
  m_TimeStampSecFraction = frac;
  return 1;
}

int MessageBase::GetTimeStamp(unsigned int* sec, unsigned int* frac)
{
  *sec  = m_TimeStampSec;
  *frac = m_TimeStampSecFraction;
  return 1;
}

void MessageBase::SetTimeStamp(igtl::TimeStamp::Pointer& ts)
{
  m_TimeStampSec = ts->GetSecond();
  m_TimeStampSecFraction = igtl_nanosec_to_frac(ts->GetNanosecond());

}

void MessageBase::GetTimeStamp(igtl::TimeStamp::Pointer& ts)
{
  ts->SetTime(m_TimeStampSec, igtl_frac_to_nanosec(m_TimeStampSecFraction));
}


int MessageBase::Pack()
{
  PackBody();
#if OpenIGTLink_PROTOCOL_VERSION >= 3
  ///To pack the meta data, the m_MetaData must be initialized,
  ///However, the position of the m_MetaData in the m_Body is different for each derived class
  ///due to the extra body header(such as m_ImageHeader...)
  ///So after the body is packed, the m_MetaData must be set.
  PackMetaData();
#endif
  
  m_IsBodyUnpacked   = 0;
  
  // pack header
  igtl_header* h = (igtl_header*) m_Header;

  igtl_uint64 crc = crc64(0, 0, 0LL); // initial crc
  if (m_Version == IGTL_HEADER_VERSION_3)
  {
    h->version   = IGTL_HEADER_VERSION_3;
  }
  else
  {
    h->version   = IGTL_HEADER_VERSION_1;
  }

  igtl_uint64 ts  =  m_TimeStampSec & 0xFFFFFFFF;
  ts = (ts << 32) | (m_TimeStampSecFraction & 0xFFFFFFFF);

  h->timestamp = ts;
  h->body_size = GetBodyPackSize();
  h->crc       = crc64((unsigned char*)m_Body, GetBodyPackSize(), crc);


  strncpy(h->name, m_DefaultBodyType.c_str(), 12);
  // TODO: this does not allow creating pack with MessageBase class...

  strncpy(h->device_name, m_DeviceName.c_str(), 20);

  igtl_header_convert_byte_order(h);

  m_IsHeaderUnpacked = 0;
  
  return 1;
}

int MessageBase::Unpack(int crccheck)
{
  int r = UNPACK_UNDEF;

  // Check if the pack exists and if it has not been unpacked.
  if (m_Header != NULL && m_PackSize >= IGTL_HEADER_SIZE &&
      m_IsHeaderUnpacked == 0)
    {
      // Unpack (deserialize) the header
      igtl_header* h = (igtl_header*) m_Header;
      igtl_header_convert_byte_order(h);
      m_TimeStampSecFraction = h->timestamp & 0xFFFFFFFF;
      m_TimeStampSec = (h->timestamp >> 32 ) & 0xFFFFFFFF;
      m_Version = h->version;
      m_BodySizeToRead = h->body_size;
      
      char bodyType[13];
      char deviceName[21];
      
      bodyType[12]   = '\0';
      deviceName[20] = '\0';
      strncpy(bodyType, h->name, 12);
      strncpy(deviceName, h->device_name, 20);
      
      m_BodyType   = bodyType;  // TODO: should check if the class is MessageBase...
      m_DeviceName = deviceName;
      
      // Mark as unpacked.
      m_IsHeaderUnpacked = 1;
      r |= UNPACK_HEADER;
    }

  // Check if the body exists and it has not been unpacked
  if (GetPackBodySize() > 0 && m_IsBodyUnpacked == 0)
    {
      igtl_header* h   = (igtl_header*) m_Header;
      igtl_uint64  crc = crc64(0, 0, 0LL); // initial crc

      if (crccheck)
        {
          // Calculate CRC of the body
          crc = crc64((unsigned char*)m_Body, m_BodySizeToRead, crc);
        }
      else
        {
          crc = h->crc;
        }

      if (crc == h->crc)
        {
          // Unpack (deserialize) the Body
#if OpenIGTLink_PROTOCOL_VERSION >= 3
          UnpackExtendedHeader();
#endif
          UnpackBody();
#if OpenIGTLink_PROTOCOL_VERSION >= 3
          UnpackMetaData();
#endif
          m_IsBodyUnpacked = 1;
          r |= UNPACK_BODY;
        }
      else
        {
          m_IsBodyUnpacked = 0;
        }
    }

  return r;
}


  
void* MessageBase::GetPackPointer()
{
  return (void*) m_Header;
}

void* MessageBase::GetPackBodyPointer()
{
  return (void*) m_Body;
}
 
int MessageBase::GetPackSize()
{
  return m_PackSize;
}

int MessageBase::GetPackBodySize()
{
  return (m_PackSize - IGTL_HEADER_SIZE);
}

void MessageBase::AllocatePack()
{
  if (m_BodySizeToRead > 0)
    {
      // called after receiving general header
      AllocatePack(m_BodySizeToRead);
    }
  else
    {
      // called for creating pack to send
      AllocatePack(GetBodyPackSize());
    }
#if OpenIGTLink_PROTOCOL_VERSION >= 3
  if (m_Version == IGTL_HEADER_VERSION_3)
  {
    m_ExtendedHeader = m_Body;
  }
#endif
}

void MessageBase::InitPack()
{
  m_IsHeaderUnpacked = 0;
  m_IsBodyUnpacked   = 0;
  m_BodySizeToRead   = 0;

  m_DeviceName       = "";
  m_BodyType         = "";

  // Re-allocate header area
  AllocatePack(0);
}

void MessageBase::AllocatePack(int bodySize)
{

  if (bodySize <= 0)
    {
      bodySize = 0;
      m_IsBodyUnpacked = 0;
    }

  int s = IGTL_HEADER_SIZE + bodySize;

  if (m_Header == NULL)
    {
      // For the first time
      m_Header = new unsigned char [s];
      m_IsHeaderUnpacked = 0;
      m_IsBodyUnpacked = 0;
    }
  else if (m_PackSize != s)
    {
      // If the pack area exists but needs to be reallocated
      // m_IsHeaderUnpacked status is not changed in this case.
      unsigned char* old = m_Header;
      m_Header = new unsigned char [s];
      memcpy(m_Header, old, IGTL_HEADER_SIZE);
      delete [] old;
      m_IsBodyUnpacked = 0;
    }
  m_Body   = &m_Header[IGTL_HEADER_SIZE];
  m_PackSize = s;
}

int MessageBase::CopyHeader(const MessageBase* mb)
{

  if (m_Header != NULL && mb->m_Header != NULL)
    {
    memcpy(m_Header, mb->m_Header, IGTL_HEADER_SIZE);
    m_Body           = &m_Header[IGTL_HEADER_SIZE];
    }
  m_PackSize             = mb->m_PackSize;
  m_BodyType             = mb->m_BodyType;
  m_DeviceName           = mb->m_DeviceName;
  m_TimeStampSec         = mb->m_TimeStampSec;
  m_TimeStampSecFraction = mb->m_TimeStampSecFraction;
  m_IsHeaderUnpacked     = mb->m_IsHeaderUnpacked;
  m_IsBodyUnpacked       = mb->m_IsBodyUnpacked;
  m_BodySizeToRead       = mb->m_BodySizeToRead;
  m_Version              = mb->m_Version;
  
  return 1;
}

int MessageBase::CopyBody(const MessageBase *mb)
{
  int s = m_PackSize - IGTL_HEADER_SIZE;
  if (m_Body != NULL && mb->m_Body != NULL && s > 0)
    {
    memcpy(m_Body, mb->m_Body, s);
    return 1;
    }

  return 0;
}

int MessageBase::Copy(const MessageBase* mb)
{
  if (this == mb)
    {
      return 0;
    }

  // Check if the destination (this class) is MessageBase or
  // the source and destination class arethe same type.
  // The pack size is also checked if it is larger than the header size.
  if ((m_DefaultBodyType.length() == 0 || m_DefaultBodyType == mb->m_BodyType)
      && mb->m_PackSize >= IGTL_HEADER_SIZE)
    {
      int bodySize = mb->m_PackSize - IGTL_HEADER_SIZE;
      AllocatePack(bodySize);
      CopyHeader(mb);
      if (bodySize > 0)
        {
          CopyBody(mb);
        }
      return 1;
    }
  else
    {
      return 0;
    }
}

}



