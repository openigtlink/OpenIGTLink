/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlStatusMessage.h"

#include "igtl_header.h"
#include "igtl_status.h"

// Disable warning C4996 (strncpy() may be unsafe) in Windows. 
#define _CRT_SECURE_NO_WARNINGS

#include <string.h>

namespace igtl {

StatusMessage::StatusMessage():
  MessageBase()
{
  m_StatusHeader = NULL;
  m_StatusMessage = NULL;

  m_ErrorName[0]        = '\n';
  m_SendMessageType     = "STATUS";
  m_StatusMessageString = "";

}

StatusMessage::~StatusMessage()
{
}

void StatusMessage::SetCode(int code)
{
  m_IsBodyPacked = false;
  if (code >= 0 && code < STATUS_NUM_TYPES)
    {
    this->m_Code = code;
    }
  else
    {
    this->m_Code = 0;
    }
}

int StatusMessage::GetCode()
{
  return this->m_Code;
}

void StatusMessage::SetSubCode(igtlInt64 subcode)
{
  m_IsBodyPacked = false;
  this->m_SubCode = subcode;
}

igtlInt64 StatusMessage::GetSubCode()
{
  return this->m_SubCode;
}

void StatusMessage::SetErrorName(const char* name)
{
  m_IsBodyPacked = false;
  this->m_ErrorName[IGTL_STATUS_ERROR_NAME_LENGTH-1] = '\0';
  strncpy(this->m_ErrorName, name, IGTL_STATUS_ERROR_NAME_LENGTH);
}

const char* StatusMessage::GetErrorName()
{
  return this->m_ErrorName;
}

void StatusMessage::SetStatusString(const char* str)
{
  m_IsBodyPacked = false;
  this->m_StatusMessageString = str;
}

const char* StatusMessage::GetStatusString()
{
  return this->m_StatusMessageString.c_str();
}


igtlUint64 StatusMessage::CalculateContentBufferSize()
{
  // The body size sum of the header size and status message size.
  // Note that the status message ends with '\0'
  return IGTL_STATUS_HEADER_SIZE + m_StatusMessageString.size() + 1;
}

int StatusMessage::PackContent()
{
  // Allocate buffer
  AllocateBuffer();
  m_StatusHeader = this->m_Content;
  
  m_StatusMessage = (char*)&m_StatusHeader[IGTL_STATUS_HEADER_SIZE];
  igtl_status_header* status_header = (igtl_status_header*)this->m_StatusHeader;

  status_header->code    = static_cast<igtlUint16>(this->m_Code);
  status_header->subcode = this->m_SubCode;
  strncpy(status_header->error_name, this->m_ErrorName, IGTL_STATUS_ERROR_NAME_LENGTH);
  strcpy(this->m_StatusMessage, this->m_StatusMessageString.c_str());
  
  igtl_status_convert_byte_order(status_header);

  return 1;
}

int StatusMessage::UnpackContent()
{
  m_StatusHeader = this->m_Content;
  m_StatusMessage = (char*)&m_StatusHeader[IGTL_STATUS_HEADER_SIZE];

  igtl_status_header* status_header = (igtl_status_header*)this->m_StatusHeader;
  igtl_status_convert_byte_order(status_header);

  this->m_Code    = status_header->code;
  this->m_SubCode = status_header->subcode;
  this->m_ErrorName[IGTL_STATUS_ERROR_NAME_LENGTH-1] = '\0';
  strncpy(this->m_ErrorName, status_header->error_name, IGTL_STATUS_ERROR_NAME_LENGTH);

  // make sure that the status message in the pack ends with '\0'
  bool isUnpacked(true);
  if (m_StatusMessage[CalculateReceiveContentSize(isUnpacked)-IGTL_STATUS_HEADER_SIZE-1] == '\0')
    {
    this->m_StatusMessageString = m_StatusMessage;
    }
  else
    {
    //std::cerr << "status message in the pack does not end with '\0'" << std::endl;
    }

  return 1;
}

} // namespace igtl
