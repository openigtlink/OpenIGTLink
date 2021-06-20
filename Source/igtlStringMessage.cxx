/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlStringMessage.h"

#include "igtl_header.h"
#include "igtl_string.h"

#include <cstring>

// Disable warning C4996 (strncpy() may be unsafe) in Windows. 
#define _CRT_SECURE_NO_WARNINGS

namespace igtl {

StringMessage::StringMessage()
{
  this->m_SendMessageType = "STRING";
  this->m_Encoding = IGTL_STRING_MESSAGE_DEFAULT_ENCODING;
  this->m_String.clear();
}

StringMessage::~StringMessage()
{
}


int StringMessage::SetString(const char* string)
{
  if (strlen(string) > 0xFFFF) /* If the length is beyond the range of unsigned short */
    {
    return 0;
    }
  m_IsBodyPacked = false;
  this->m_String = string;
  return (int) this->m_String.length();
}


int StringMessage::SetString(const std::string & string)
{
  if (string.length() > 0xFFFF) /* If the length is beyond the range of unsigned short */
    {
    return 0;
    }
  m_IsBodyPacked = false;
  this->m_String = string;
  return (int) this->m_String.length();
}


int StringMessage::SetEncoding(igtlUint16 enc)
{
  // TODO: the argument should be validated before it is substituted
  this->m_Encoding = enc;
  m_IsBodyPacked = false;
  return 1;
}


const char* StringMessage::GetString()
{
  return this->m_String.c_str();
}


igtlUint16 StringMessage::GetEncoding()
{
  return this->m_Encoding;
}


igtlUint64 StringMessage::CalculateContentBufferSize()
{
  // Body pack size is the sum of ENCODING, LENGTH and STRING fields
  return sizeof(igtlUint16)*2 + this->m_String.length();
}


int StringMessage::PackContent()
{
  // Allocate buffer
  AllocateBuffer();
  igtl_string_header * string_header;
  char * string;
  // Set pointers
#if OpenIGTLink_HEADER_VERSION >= 2
  string_header = (igtl_string_header*) this->m_Content;
  string        = (char *) this->m_Content + sizeof(igtlUint16)*2;
#elif OpenIGTLink_PROTOCOL_VERSION <=2
  string_header = (igtl_string_header*) this->m_Body;
  string        = (char *) this->m_Body + sizeof(igtlUint16)*2;
#endif

  // Copy data
  string_header->encoding = static_cast<igtlUint16>(this->m_Encoding);
  string_header->length   = static_cast<igtlUint16>(this->m_String.length());
  strncpy(string, this->m_String.c_str(), string_header->length);

  // Convert byte order from host to network
  igtl_string_convert_byte_order(string_header);

  return 1;
}

int StringMessage::UnpackContent()
{
  igtl_string_header * string_header;
  char * string;
#if OpenIGTLink_HEADER_VERSION >= 2
  string_header = (igtl_string_header*) (this->m_Content);
  string        = (char *) this->m_Content + sizeof(igtlUint16)*2;
#elif OpenIGTLink_PROTOCOL_VERSION <=2
  string_header = (igtl_string_header*) this->m_Body;
  string        = (char *) this->m_Body + sizeof(igtlUint16)*2;
#endif

  // Convert byte order from network to host
  igtl_string_convert_byte_order(string_header);
  
  // Copy data
  this->m_Encoding = string_header->encoding;
  this->m_String.clear();
  this->m_String.append(string, string_header->length);

  return 1;
}

} // namespace igtl
