/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlMessageBase.h"
#include "igtlMessageFactory.h"
#include "igtl_header.h"
#include "igtl_util.h"

#include <algorithm>
#include <string>
#include <cstring>
#include <limits>

namespace
{
  static const int META_DATA_INDEX_COUNT_SIZE = sizeof(igtlUint16);
}

namespace igtl
{

MessageBase::MessageBase()
  : Object()
    , m_MessageSize(0)
    , m_Header(NULL)
    , m_Body(NULL)
    , m_Content(NULL)
    , m_BodySizeToRead(0)
    , m_SendMessageType("")
    , m_ReceiveMessageType("")
    , m_HeaderVersion(IGTL_HEADER_VERSION_1)
    , m_DeviceName("")
    , m_TimeStampSec(0)
    , m_TimeStampSecFraction(0)
    , m_IsHeaderUnpacked(false)
    , m_IsBodyUnpacked(false)
    , m_IsBodyPacked(false)
#if OpenIGTLink_HEADER_VERSION >= 2
    , m_ExtendedHeader(NULL)
    , m_IsExtendedHeaderUnpacked(false)
    , m_MetaData(NULL)
    , m_MessageId(0)
    , m_MetaDataMap(MetaDataMap())
#endif
{
}

MessageBase::~MessageBase()
{
  if (this->m_MessageSize > 0 && this->m_Header != NULL)
    {
    delete [] m_Header;
    m_MessageSize = 0;
    m_Header      = NULL;
    m_Body        = NULL;
    m_Content     = NULL;
#if OpenIGTLink_HEADER_VERSION >= 2
    m_ExtendedHeader = NULL;
    m_MetaDataHeader = NULL;
    m_MetaData = NULL;
#endif
    }
}

int MessageBase::CalculateContentBufferSize()
{
  return 0;
}

igtl::MessageBase::Pointer MessageBase::Clone()
{
  igtl::MessageBase::Pointer clone;
    {
    igtl::MessageFactory::Pointer factory = igtl::MessageFactory::New();
    clone = factory->CreateSendMessage(this->GetMessageType(), this->GetHeaderVersion());
    }

    int bodySize = this->m_MessageSize - IGTL_HEADER_SIZE;
    clone->InitBuffer();
    clone->CopyHeader(this);
    clone->AllocateBuffer(bodySize);
    if (bodySize > 0)
      {
      clone->CopyBody(this);
      }

#if OpenIGTLink_HEADER_VERSION >= 2
    clone->m_MetaDataHeader = this->m_MetaDataHeader;
    clone->m_MetaDataMap = this->m_MetaDataMap;
    clone->m_IsExtendedHeaderUnpacked = this->m_IsExtendedHeaderUnpacked;
#endif

    return clone;
}

void MessageBase::SetHeaderVersion(unsigned short version)
{
  m_HeaderVersion = version;
  m_IsBodyPacked = false;
}

unsigned short MessageBase::GetHeaderVersion() const
{
  return m_HeaderVersion;
}

void MessageBase::SetDeviceName(const char* name)
{
  if (name == NULL)
    {
    return;
    }
  SetDeviceName(std::string(name));
}

void MessageBase::SetDeviceName(const std::string& name)
{
  m_DeviceName = name;
  m_IsBodyPacked = false;
}

void MessageBase::SetDeviceType(const std::string& type)
{
  this->m_ReceiveMessageType = type;
  m_IsBodyPacked = false;
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
  if (m_SendMessageType.length() > 0)
    {
    return m_SendMessageType.c_str();
    }
  else
    {
    return m_ReceiveMessageType.c_str();
    }
}

std::string MessageBase::GetMessageType() const
{
  if (m_SendMessageType.length() > 0)
    {
    return m_SendMessageType;
    }
  else
    {
    return m_ReceiveMessageType;
    }
}

#if OpenIGTLink_HEADER_VERSION >= 2
igtlUint32 MessageBase::GetMetaDataSize()
{
  if( m_HeaderVersion >= IGTL_HEADER_VERSION_2 )
    {
    igtlUint32 metaDataSize(0);
    for (MetaDataMap::const_iterator it = m_MetaDataMap.begin(); it != m_MetaDataMap.end(); ++it)
      {
      metaDataSize += it->first.length() + it->second.second.length();
      }

    return metaDataSize;
    }
  else
    {
    return 0;
    }
}

igtlUint16 MessageBase::GetMetaDataHeaderSize()
{
  if( m_HeaderVersion >= IGTL_HEADER_VERSION_2 )
    {
    return (m_MetaDataMap.size()*sizeof(igtl_metadata_header_entry)) + sizeof(igtlUint16); // index_count is at beginning of header
    }
  else
    {
    return 0;
    }
}

igtlUint32 MessageBase::GetMessageID()
{
  return m_MessageId;
}

void MessageBase::SetMessageID(igtlUint32 idValue)
{
  m_MessageId = idValue;
  m_IsBodyPacked = false;
}

bool MessageBase::SetMetaDataElement(const std::string& key, IANA_ENCODING_TYPE encodingScheme, std::string value)
{
  igtl_metadata_header_entry entry;
  if (key.length() > std::numeric_limits<igtl_uint16>::max())
    {
    return false;
    }
  entry.key_size = static_cast<igtl_uint16>(key.length());
  entry.value_encoding = static_cast<igtlUint16>(encodingScheme);
  entry.value_size = value.length();

  m_MetaDataMap[key] = std::pair<IANA_ENCODING_TYPE, std::string>(encodingScheme, value);

  m_IsBodyPacked = false;

  return true;
}

bool MessageBase::SetMetaDataElement(const std::string& key, igtl_uint8 value)
{
  std::stringstream ss;
  ss << value;
  return SetMetaDataElement(key, IANA_TYPE_US_ASCII, ss.str());
}

bool MessageBase::SetMetaDataElement(const std::string& key, igtl_int8 value)
{
  std::stringstream ss;
  ss << value;
  return SetMetaDataElement(key, IANA_TYPE_US_ASCII, ss.str());
}

bool MessageBase::SetMetaDataElement(const std::string& key, igtl_uint16 value)
{
  std::stringstream ss;
  ss << value;
  return SetMetaDataElement(key, IANA_TYPE_US_ASCII, ss.str());
}

bool MessageBase::SetMetaDataElement(const std::string& key, igtl_int16 value)
{
  std::stringstream ss;
  ss << value;
  return SetMetaDataElement(key, IANA_TYPE_US_ASCII, ss.str());
}

bool MessageBase::SetMetaDataElement(const std::string& key, igtl_uint32 value)
{
  std::stringstream ss;
  ss << value;
  return SetMetaDataElement(key, IANA_TYPE_US_ASCII, ss.str());
}

bool MessageBase::SetMetaDataElement(const std::string& key, igtl_int32 value)
{
  std::stringstream ss;
  ss << value;
  return SetMetaDataElement(key, IANA_TYPE_US_ASCII, ss.str());
}

bool MessageBase::SetMetaDataElement(const std::string& key, igtl_uint64 value)
{
  std::stringstream ss;
  ss << value;
  return SetMetaDataElement(key, IANA_TYPE_US_ASCII, ss.str());
}

bool MessageBase::SetMetaDataElement(const std::string& key, igtl_int64 value)
{
  std::stringstream ss;
  ss << value;
  return SetMetaDataElement(key, IANA_TYPE_US_ASCII, ss.str());
}

bool MessageBase::SetMetaDataElement(const std::string& key, float value)
{
  std::stringstream ss;
  ss << value;
  return SetMetaDataElement(key, IANA_TYPE_US_ASCII, ss.str());
}

bool MessageBase::SetMetaDataElement(const std::string& key, double value)
{
  std::stringstream ss;
  ss << value;
  return SetMetaDataElement(key, IANA_TYPE_US_ASCII, ss.str());
}

bool MessageBase::GetMetaDataElement(const std::string& key, std::string& value) const
{
  IANA_ENCODING_TYPE type;
  return GetMetaDataElement(key, type, value);
}

bool MessageBase::GetMetaDataElement(const std::string& key, IANA_ENCODING_TYPE& encoding, std::string& value) const
{
  if (m_MetaDataMap.find(key) != m_MetaDataMap.end())
    {
    encoding = m_MetaDataMap.find(key)->second.first;
    value = m_MetaDataMap.find(key)->second.second;
    return true;
    }

  return false;
}

const MessageBase::MetaDataMap& MessageBase::GetMetaData() const
{
  return this->m_MetaDataMap;
}

bool MessageBase::PackExtendedHeader()
{
  if( m_HeaderVersion == IGTL_HEADER_VERSION_2 )
    {
    int aSize = m_MessageSize - IGTL_HEADER_SIZE - sizeof(igtl_extended_header);
    if( aSize < 0 )
      {
      // Ensure we have enough space to write the header
      AllocateBuffer(0);
      }

    igtl_extended_header* extended_header   = (igtl_extended_header*) m_ExtendedHeader;
    extended_header->extended_header_size   = sizeof(igtl_extended_header);
    extended_header->meta_data_header_size  = this->GetMetaDataHeaderSize();
    extended_header->meta_data_size         = this->GetMetaDataSize();
    extended_header->message_id             = this->GetMessageID();
    igtl_extended_header_convert_byte_order(extended_header);

    return true;
    }

  return false;
}

bool MessageBase::PackMetaData()
{
  if( m_HeaderVersion == IGTL_HEADER_VERSION_2 )
    {
    if (m_MetaDataMap.size() > std::numeric_limits<igtl_uint16>::max())
      {
      return false;
      }
    igtl_uint16 index_count = static_cast<igtl_uint16>(m_MetaDataMap.size()); // first two byte are the total number of meta data
    if(igtl_is_little_endian())
      {
      index_count = BYTE_SWAP_INT16(index_count);
      }

    // Pack meta data header key/encoding/value trios
    memcpy(m_MetaDataHeader, &index_count, META_DATA_INDEX_COUNT_SIZE);

    if (m_MetaDataMap.size() > 0)
      {
      unsigned char* entryPointer = &m_MetaDataHeader[META_DATA_INDEX_COUNT_SIZE];
      for (MetaDataMap::const_iterator it = m_MetaDataMap.begin(); it != m_MetaDataMap.end();++it)
        {
        igtl_metadata_header_entry entry;
        entry.key_size = it->first.length();
        entry.value_encoding = it->second.first;
        entry.value_size = it->second.second.length();

        if(igtl_is_little_endian())
          {
          entry.key_size = BYTE_SWAP_INT16(entry.key_size);
          entry.value_encoding = BYTE_SWAP_INT16(entry.value_encoding);
          entry.value_size = BYTE_SWAP_INT32(entry.value_size);
          }
        memcpy(&entryPointer[0], &entry.key_size, sizeof(igtlUint16));
        memcpy(&entryPointer[2], &entry.value_encoding, sizeof(igtlUint16));
        memcpy(&entryPointer[4], &entry.value_size, sizeof(igtlUint32));

        entryPointer += sizeof(igtl_metadata_header_entry);
        }

      // Pack meta data key/value pairs
      std::string key("");
      std::string value("");

      unsigned char* metaDataPointer = m_MetaData;
      int i(0);
      for (MetaDataMap::iterator it = m_MetaDataMap.begin(); it != m_MetaDataMap.end(); ++it, ++i)
        {
        memcpy(metaDataPointer, it->first.c_str(), it->first.length());
        metaDataPointer += it->first.length();
        memcpy(metaDataPointer, it->second.second.c_str(), it->second.second.length());
        metaDataPointer += it->second.second.length();
        }

      return true;
      }
    else
      {
      return true;
      }
    }

  return false;
}

bool MessageBase::UnpackExtendedHeader()
{
  if (m_HeaderVersion == IGTL_HEADER_VERSION_2)
    {
    igtl_extended_header* extended_header = (igtl_extended_header*)m_ExtendedHeader;
    igtl_extended_header_convert_byte_order(extended_header);
    if( extended_header->extended_header_size != sizeof(igtl_extended_header) )
      {
      // any extra data will be dropped, if the order of variables is changed, this will be seriously broken
      // TODO : add error reporting?
      }
    this->m_MessageId           = extended_header->message_id;

    m_Content = &m_Body[extended_header->extended_header_size];
    m_MetaDataHeader = &m_Body[m_BodySizeToRead - extended_header->meta_data_header_size - extended_header->meta_data_size];
    m_MetaData = &m_Body[m_BodySizeToRead - extended_header->meta_data_size];

    m_IsExtendedHeaderUnpacked = true;

    return true;
    }

  return false;
}

bool MessageBase::UnpackMetaData()
{
  if (m_HeaderVersion == IGTL_HEADER_VERSION_2)
    {
    // Parse the header
    igtl_uint16 index_count = 0; // first two byte are the total number of meta data
    memcpy(&index_count, m_MetaDataHeader, META_DATA_INDEX_COUNT_SIZE);
    if(igtl_is_little_endian())
      {
      index_count = BYTE_SWAP_INT16(index_count);
      }

    if( index_count == 0 )
      {
      return true;
      }

    std::vector<igtl_metadata_header_entry> metaDataEntries;
    igtl_metadata_header_entry entry;
    unsigned char* entryPointer = &m_MetaDataHeader[META_DATA_INDEX_COUNT_SIZE];
    for (int i = 0; i < index_count; i++)
      {
      memcpy(&entry.key_size, &entryPointer[0], sizeof(igtlUint16));
      memcpy(&entry.value_encoding, &entryPointer[2], sizeof(igtlUint16));
      memcpy(&entry.value_size, &entryPointer[4], sizeof(igtlUint32));
      if(igtl_is_little_endian())
        {
        entry.key_size = BYTE_SWAP_INT16(entry.key_size);
        entry.value_encoding = BYTE_SWAP_INT16(entry.value_encoding);
        entry.value_size = BYTE_SWAP_INT32(entry.value_size);
        }
      metaDataEntries.push_back(entry);

      entryPointer += sizeof(igtl_metadata_header_entry);
      }

    // Parse the meta data
    m_MetaDataMap.clear();

    unsigned char* metaDataPointer = m_MetaData;
    for (int i = 0; i < index_count; i++)
      {
      std::string key;
      key.assign(metaDataPointer, metaDataPointer + metaDataEntries[i].key_size);
      metaDataPointer += metaDataEntries[i].key_size;
      std::string value;
      value.assign(metaDataPointer, metaDataPointer + metaDataEntries[i].value_size);
      metaDataPointer += metaDataEntries[i].value_size;
      m_MetaDataMap[key] = std::pair<IANA_ENCODING_TYPE, std::string>((IANA_ENCODING_TYPE)metaDataEntries[i].value_encoding, value);
      }

    return true;
    }

  return false;
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
  if (m_IsBodyPacked)
    {
    // Allow for multiple packs
    return 1;
    }

  if( m_SendMessageType.empty() )
    {
    // We do not allow sending of base class messages, aka igtl::MessageBase
    // TODO : error reporting?
    return 0;
    }

#if OpenIGTLink_HEADER_VERSION >= 2
  PackExtendedHeader();
#endif

  // Derived classes will re-call allocate pack with their required content size
  PackContent();

#if OpenIGTLink_HEADER_VERSION >= 2
  PackMetaData();
#endif

  m_IsBodyPacked = true;

  // pack header
  igtl_header* h = (igtl_header*) m_Header;

  igtl_uint64 crc = crc64(0, 0, 0LL); // initial crc
  h->header_version   = m_HeaderVersion;

  igtl_uint64 ts  =  m_TimeStampSec & 0xFFFFFFFF;
  ts = (ts << 32) | (m_TimeStampSecFraction & 0xFFFFFFFF);

  h->timestamp = ts;
  h->body_size = GetBufferBodySize();
  h->crc       = crc64((unsigned char*)m_Body, h->body_size, crc);

  strncpy(h->name, m_SendMessageType.c_str(), 12);

  strncpy(h->device_name, m_DeviceName.c_str(), 20);

  igtl_header_convert_byte_order(h);

  return 1;
}

int MessageBase::Unpack(int crccheck)
{
  int r = UNPACK_UNDEF;

  // Check if the pack exists and if it has not been unpacked.
  if (m_Header != NULL && m_MessageSize >= IGTL_HEADER_SIZE && !m_IsHeaderUnpacked )
    {
    InitBuffer();
    UnpackHeader(r);
    }

#if OpenIGTLink_HEADER_VERSION >= 2
  // Check if the body exists and it has not been unpacked
  // The extended header is technically located inside the body, so we have to check to see if the remaining body size
  // is > 0, or if full body size > sizeof(igtl_extended_header)
  if( m_HeaderVersion >= IGTL_HEADER_VERSION_2 )
    {
    if (GetBufferBodySize() > static_cast<int>(sizeof(igtl_extended_header)) + META_DATA_INDEX_COUNT_SIZE && !m_IsBodyUnpacked)
      {
      UnpackBody(crccheck, r);
      }
    }
  else
    {
#endif
    if(GetBufferBodySize() > 0 && !m_IsBodyUnpacked)
      {
      UnpackBody(crccheck, r);
      }
#if OpenIGTLink_HEADER_VERSION >= 2
    }
#endif

  return r;
}

void* MessageBase::GetBufferPointer()
{
  return (void*) m_Header;
}

void* MessageBase::GetBufferBodyPointer()
{
  return (void*) m_Body;
}

int MessageBase::GetBufferSize()
{
  return m_MessageSize;
}

int MessageBase::GetBufferBodySize()
{
  return GetBufferSize() - IGTL_HEADER_SIZE;
}

int MessageBase::CalculateReceiveContentSize()
{
#if OpenIGTLink_HEADER_VERSION >= 2
  if( m_HeaderVersion >= IGTL_HEADER_VERSION_2 )
    {
    if( !m_IsExtendedHeaderUnpacked )
      {
      return -1;
      }
    igtl_extended_header* header = (igtl_extended_header*)m_ExtendedHeader;
    return GetBufferSize() - IGTL_HEADER_SIZE - header->extended_header_size - header->meta_data_header_size - header->meta_data_size;
    }
  else
    {
#endif
    return m_BodySizeToRead;
#if OpenIGTLink_HEADER_VERSION >= 2
    }
#endif
}

const char* MessageBase::GetBodyType()
{
  return this->m_ReceiveMessageType.c_str();
}

int MessageBase::PackContent()
{
  return 0;
}

int MessageBase::UnpackContent()
{
  return 0;
}

void MessageBase::AllocateBuffer()
{
  if( m_BodySizeToRead > 0 )
    {
    // called after receiving general header
    AllocateUnpack(m_BodySizeToRead);
    }
  else
    {
    // called for creating pack to send
    AllocateBuffer(CalculateContentBufferSize());
    }
}

void MessageBase::InitBuffer()
{
  m_IsHeaderUnpacked = false;
  m_IsBodyPacked     = false;
  m_IsBodyUnpacked   = false;
  m_BodySizeToRead   = 0;

  m_DeviceName       = "";
  m_ReceiveMessageType         = "";

  // Re-allocate header area
  int message_size = IGTL_HEADER_SIZE;

  if (m_Header == NULL)
    {
    // For the first time
    m_Header = new unsigned char [message_size];
    m_IsHeaderUnpacked = false;
    m_IsBodyUnpacked = false;
    }
  else if (m_MessageSize != message_size)
    {
    // If the pack area exists but needs to be reallocated
    // m_IsHeaderUnpacked status is not changed in this case.
    unsigned char* old = m_Header;
    m_Header = new unsigned char [message_size];
    memcpy(m_Header, old, std::min<int>(m_MessageSize, message_size));
    delete [] old;
    m_IsBodyUnpacked = false;
    }
  m_Body   = &m_Header[IGTL_HEADER_SIZE];
#if OpenIGTLink_HEADER_VERSION >= 2
  if (m_HeaderVersion == IGTL_HEADER_VERSION_2)
    {
    m_ExtendedHeader = m_Body;
    // Other members can't be populated until the message is unpacked
    }
  else
    {
    m_Content = m_Body;
    }
#else
  m_Content = m_Body;
#endif
  m_MessageSize = message_size;
}

void MessageBase::AllocateBuffer(int contentSize)
{
#if OpenIGTLink_HEADER_VERSION >= 2
  int message_size(-1);
  if (m_HeaderVersion == IGTL_HEADER_VERSION_2)
    {
    message_size = IGTL_HEADER_SIZE + contentSize + sizeof(igtl_extended_header) + GetMetaDataHeaderSize() + GetMetaDataSize();
    }
  else
    {
    message_size = IGTL_HEADER_SIZE + contentSize;
    }
#else
  int message_size = IGTL_HEADER_SIZE + contentSize;
#endif

  if (m_Header == NULL)
    {
    // For the first time
    m_Header = new unsigned char [message_size];
    m_IsHeaderUnpacked = false;
    m_IsBodyUnpacked = false;
    m_IsBodyPacked = false;
    }
  else if (m_MessageSize != message_size)
    {
    // If the pack area exists but needs to be reallocated
    // m_IsHeaderUnpacked status is not changed in this case.
    unsigned char* old = m_Header;
    m_Header = new unsigned char [message_size];
    memcpy(m_Header, old, std::min<int>(m_MessageSize, message_size));
    delete [] old;
    m_IsBodyUnpacked = false;
    }
  m_Body   = &m_Header[IGTL_HEADER_SIZE];

#if OpenIGTLink_HEADER_VERSION >= 2
  if (m_HeaderVersion == IGTL_HEADER_VERSION_2)
    {
    m_ExtendedHeader = m_Body;
    m_Content = &m_Body[sizeof(igtl_extended_header)];
    m_MetaDataHeader = &m_Body[sizeof(igtl_extended_header)+contentSize];
    m_MetaData = &m_Body[sizeof(igtl_extended_header)+contentSize+GetMetaDataHeaderSize()];
    }
  else
    {
#endif
    m_Content = m_Body;
#if OpenIGTLink_HEADER_VERSION >= 2
    }
#endif

  m_MessageSize = message_size;
}

int MessageBase::CopyHeader(const MessageBase* mb)
{
  if (m_Header != NULL && mb->m_Header != NULL)
    {
    memcpy(m_Header, mb->m_Header, IGTL_HEADER_SIZE);
    m_Body = &m_Header[IGTL_HEADER_SIZE];
    if (mb->m_HeaderVersion < IGTL_HEADER_VERSION_2)
      {
      m_Content = m_Body;
      }
    }
  m_MessageSize          = mb->m_MessageSize;
  m_ReceiveMessageType   = mb->m_ReceiveMessageType;
  m_DeviceName           = mb->m_DeviceName;
  m_TimeStampSec         = mb->m_TimeStampSec;
  m_TimeStampSecFraction = mb->m_TimeStampSecFraction;
  m_IsHeaderUnpacked     = mb->m_IsHeaderUnpacked;
  m_IsBodyUnpacked       = mb->m_IsBodyUnpacked;
  m_IsBodyPacked         = mb->m_IsBodyPacked;
  m_BodySizeToRead       = mb->m_BodySizeToRead;
  m_HeaderVersion        = mb->m_HeaderVersion;

  return 1;
}

int MessageBase::CopyBody(const MessageBase *mb)
{
  int bodySize = m_MessageSize - IGTL_HEADER_SIZE;
  if (m_Body != NULL && mb->m_Body != NULL && bodySize > 0)
    {
    memcpy(m_Body, mb->m_Body, bodySize);

#if OpenIGTLink_HEADER_VERSION >= 2
    if( m_HeaderVersion == IGTL_HEADER_VERSION_2 )
      {
      igtl_extended_header* other_ext_header = (igtl_extended_header*)(mb->m_ExtendedHeader);
      if( other_ext_header->extended_header_size != sizeof(igtl_extended_header) )
        {
        return 0;
        }
      m_ExtendedHeader = m_Body;
      m_Content = &m_Body[other_ext_header->extended_header_size];
      m_MetaDataHeader = &m_Body[bodySize - other_ext_header->meta_data_header_size - other_ext_header->meta_data_size];
      m_MetaData = &m_Body[bodySize - other_ext_header->meta_data_size];
      }
    else
      {
#endif
      m_Content = m_Body;
#if OpenIGTLink_HEADER_VERSION >= 2
      }
#endif

    return 1;
    }

  return 0;
}

void MessageBase::UnpackHeader(int& r)
{
  // Unpack (deserialize) the header
  igtl_header* h = (igtl_header*) m_Header;
  igtl_header_convert_byte_order(h);
  m_TimeStampSecFraction = h->timestamp & 0xFFFFFFFF;
  m_TimeStampSec = (h->timestamp >> 32 ) & 0xFFFFFFFF;
  m_HeaderVersion = h->header_version;
  m_BodySizeToRead = h->body_size;

  char bodyType[IGTL_HEADER_TYPE_SIZE+1];
  char deviceName[IGTL_HEADER_NAME_SIZE+1];

  bodyType[IGTL_HEADER_TYPE_SIZE]   = '\0';
  deviceName[IGTL_HEADER_NAME_SIZE] = '\0';
  strncpy(bodyType, h->name, IGTL_HEADER_TYPE_SIZE);
  strncpy(deviceName, h->device_name, IGTL_HEADER_NAME_SIZE);

  m_ReceiveMessageType = bodyType;
  if( m_ReceiveMessageType.empty() )
    {
    // MessageBase class has been sent, this can't be good
    // TODO : how to report an error
    }
  m_DeviceName = deviceName;

  // Mark as unpacked.
  m_IsHeaderUnpacked = true;
  r |= UNPACK_HEADER;
}

void MessageBase::UnpackBody(int crccheck, int& r)
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
#if OpenIGTLink_HEADER_VERSION >= 2
    UnpackExtendedHeader();
#endif
    UnpackContent();
#if OpenIGTLink_HEADER_VERSION >= 2
    UnpackMetaData();
#endif
    m_IsBodyUnpacked = true;
    r |= UNPACK_BODY;
    }
  else
    {
    m_IsBodyUnpacked = false;
    }
}

void MessageBase::AllocateUnpack(int bodySizeToRead)
{
  if (bodySizeToRead <= 0)
    {
    bodySizeToRead = 0;
    m_IsBodyUnpacked = false;
    }

  int message_size = IGTL_HEADER_SIZE + bodySizeToRead;

  if (m_Header == NULL)
    {
    // For the first time
    m_Header = new unsigned char [message_size];
    m_IsHeaderUnpacked = false;
    m_IsBodyUnpacked = false;
    m_IsBodyPacked = false;
    }
  else if (m_MessageSize != message_size)
    {
    // If the pack area exists but needs to be reallocated
    // m_IsHeaderUnpacked status is not changed in this case.
    unsigned char* old = m_Header;
    m_Header = new unsigned char [message_size];
    memcpy(m_Header, old, std::min<int>(m_MessageSize, message_size));
    delete [] old;
    m_IsBodyUnpacked = false;
    }
  m_Body   = &m_Header[IGTL_HEADER_SIZE];

#if OpenIGTLink_HEADER_VERSION >= 2
  if (m_HeaderVersion >= IGTL_HEADER_VERSION_2)
    {
    m_ExtendedHeader = m_Body;
    // Other members can't be populated until the message is unpacked
    }
  else
    {
#endif
    m_Content = m_Body;
#if OpenIGTLink_HEADER_VERSION >= 2
    }
#endif

  m_MessageSize = message_size;
}

int MessageBase::Copy(const MessageBase* mb)
{
  if (this == mb)
    {
    return 0;
    }

  // Check if the destination (this class) is MessageBase or
  // the source and destination class are the same type.
  // The pack size is also checked if it is larger than the header size.
  if ((m_SendMessageType.length() == 0 || m_SendMessageType == mb->m_ReceiveMessageType)
      && mb->m_MessageSize >= IGTL_HEADER_SIZE)
    {
    // Set the header version before calling any functions, as it determines later behavior
    m_HeaderVersion = mb->m_HeaderVersion;

    int bodySize = mb->m_MessageSize - IGTL_HEADER_SIZE;
    AllocateBuffer(bodySize);
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

int MessageBase::SetMessageHeader(const MessageHeader* mb)
{
  return Copy(mb);
}

int MessageBase::GetBodySizeToRead()
{
  return m_BodySizeToRead;
}

}
