/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlQueryMessage.h"

#include "igtl_header.h"

// Disable warning C4996 (strncpy() may be unsafe) in Windows. 
#define _CRT_SECURE_NO_WARNINGS

#include <string.h>

namespace igtl {

QueryMessage::QueryMessage()
{
  this->m_SendMessageType = "QUERY";
}

QueryMessage::~QueryMessage()
{
}


int QueryMessage::SetDeviceUID(const char* string)
{
  if (string == NULL || strlen(string) > 0xFFFF) /* If the length is beyond the range of unsigned short */
    {
    return 0;
    }
  m_DeviceUID = string;
  return (int) this->m_DeviceUID.length();
}


int QueryMessage::SetDeviceUID(const std::string & string)
{
  if (string.length() > 0xFFFF) /* If the length is beyond the range of unsigned short */
  {
    return 0;
  }
  this->m_DeviceUID = string;
  return (int) this->m_DeviceUID.length();
}


std::string QueryMessage::GetDeviceUID()
{
  return this->m_DeviceUID;
}
  
  
  
int QueryMessage::SetDataType(const char* dataType)
{
  if (dataType == NULL || strlen(dataType) > IGTL_QUERY_DATE_TYPE_SIZE) /* If the length is beyond the range specified by the spec */
  {
    return 0;
  }
  strcpy((char*)m_DataType, dataType);
  return 1;
}


int QueryMessage::SetDataType(const std::string& dataType)
{
  return this->SetDataType(dataType.c_str());
}



igtlUint64 QueryMessage::CalculateContentBufferSize()
{
  // Body pack size is the sum of DeviceUID and data type fields
  return IGTL_QUERY_HEADER_SIZE + this->m_DeviceUID.length();
}

  
int QueryMessage::PackContent()
{
  // Allocate buffer
  AllocateBuffer();
  igtl_query_header * query_header;
  char * deviceName;
  
  // Set pointers
#if OpenIGTLink_HEADER_VERSION >= 2
  query_header = (igtl_query_header*) this->m_Content;
  deviceName        = (char *) this->m_Content + sizeof(igtl_query_header);
#else
  query_header = (igtl_query_header*) this->m_Body;
  deviceName        = (char *) this->m_Body + sizeof(igtl_query_header);
#endif
  
  // Copy data
  query_header->deviceUIDLength   = static_cast<igtlUint16>(this->m_DeviceUID.length());
  query_header->queryID = this->m_QueryID;
  memcpy(query_header->queryDataType, this->m_DataType, IGTL_QUERY_DATE_TYPE_SIZE);
  strncpy(deviceName, this->m_DeviceUID.c_str(), query_header->deviceUIDLength);
  
  // Convert byte order from host to network
  igtl_query_convert_byte_order(query_header);
  
  return 1;
}

int QueryMessage::UnpackContent()
{
  igtl_query_header * query_header;
  char * deviceName;
  
#if OpenIGTLink_HEADER_VERSION >= 2
  query_header = (igtl_query_header*) this->m_Content;
  deviceName        = (char *) this->m_Content + sizeof(igtl_query_header);
#else
  query_header = (igtl_query_header*) this->m_Body;
  deviceName        = (char *) this->m_Body + sizeof(igtl_query_header);
#endif
  
  // Convert byte order from network to host
  igtl_query_convert_byte_order(query_header);
  
  // Copy data
  this->m_QueryID = query_header->queryID;
  memcpy(m_DataType, query_header->queryDataType, IGTL_QUERY_DATE_TYPE_SIZE);
  this->m_DeviceUID.clear();
  this->m_DeviceUID.append(deviceName, query_header->deviceUIDLength);
  
  return 1;
}

} // namespace igtl