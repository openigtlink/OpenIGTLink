/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlCommandMessage.h"

#include "igtl_header.h"

// Disable warning C4996 (strncpy() may be unsafe) in Windows. 
#define _CRT_SECURE_NO_WARNINGS

#include <string.h>

#define NUM_ENCODINGS 257

namespace
{
  
bool isEncodingValid(int encoding)
{
  for( int i = 0; i < NUM_ENCODINGS-1; ++i )
    {
    if( encoding == igtl::CommandMessage::validEncodings[i] )
      {
      return true;
      }
    }
  return false;
}

}


namespace igtl {

const int CommandMessage::validEncodings[NUM_ENCODINGS] = {3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,109,110,111,112,113,114,115,116,117,118,119,1000,1001,1002,1003,1004,1005,1006,1007,1008,1009,1010,1011,1012,1013,1014,1015,1016,1017,1018,1019,1020,2000,2001,2002,2003,2004,2005,2006,2007,2008,2009,2012,2013,2014,2015,2016,2017,2018,2019,2020,2021,2022,2023,2024,2025,2026,2027,2028,2029,2030,2031,2032,2033,2034,2035,2036,2037,2038,2039,2040,2041,2042,2043,2011,2044,2045,2010,2046,2047,2048,2049,2050,2051,2052,2053,2054,2055,2056,2057,2058,2059,2060,2061,2062,2063,2064,2065,2066,2067,2068,2069,2070,2071,2072,2073,2074,2075,2076,2077,2078,2079,2080,2081,2082,2083,2084,2085,2086,2087,2088,2089,2090,2091,2092,2093,2094,2095,2096,2097,2098,2099,2100,2101,2102,2103,2104,2105,2106,2107,2108,2109,2250,2251,2252,2253,2254,2255,2256,2257,2258,2259,2260};

CommandMessage::CommandMessage()
  : MessageBase()
    , m_CommandId(0)
    , m_Encoding(3)
{
  memset(m_CommandName, 0, IGTL_COMMAND_NAME_SIZE);
  this->m_HeaderVersion = IGTL_HEADER_VERSION_2;
  this->m_SendMessageType = "COMMAND";
  this->m_Command.clear();
}


CommandMessage::~CommandMessage()
{
}
  
  
int CommandMessage::SetCommandId(igtlUint32 aId)
{
  m_IsBodyPacked = false;
  this->m_CommandId = aId;
  return 1;
}


int CommandMessage::SetCommandName(const char* aCommandName)
{
  if (strlen(aCommandName) > IGTL_COMMAND_NAME_SIZE) /* If the length is beyond the range specified by the spec */
    {
    return 0;
    }
  m_IsBodyPacked = false;
  strcpy((char*)m_CommandName, aCommandName);
  return 1;
}


int CommandMessage::SetCommandName(const std::string& aCommandName)
{
  return this->SetCommandName(aCommandName.c_str());
}


int CommandMessage::SetCommandContent(const char* string)
{
  if (strlen(string) > 0xFFFF) /* If the length is beyond the range of unsigned short */
    {
    return 0;
    }
  m_IsBodyPacked = false;
  this->m_Command = string;
  return (int) this->m_Command.length();
}


int CommandMessage::SetCommandContent(const std::string & string)
{
  if (string.length() > 0xFFFF) /* If the length is beyond the range of unsigned short */
    {
    return 0;
    }
  m_IsBodyPacked = false;
  this->m_Command = string;
  return (int) this->m_Command.length();
}


int CommandMessage::SetContentEncoding(igtlUint16 enc)
{
  if( !isEncodingValid(enc) )
    {
    return 0;
    }
  m_IsBodyPacked = false;
  this->m_Encoding = enc;
  return 1;
}


igtlUint32 CommandMessage::GetCommandId() const
{
  return this->m_CommandId;
}


std::string CommandMessage::GetCommandName() const
{
  return std::string((char*)this->m_CommandName);
}


std::string CommandMessage::GetCommandContent() const
{
  return this->m_Command;
}


igtlUint32 CommandMessage::GetCommandContentLength() const
{
  return this->m_Command.length();
}


igtlUint16 CommandMessage::GetContentEncoding() const
{
  return this->m_Encoding;
}


igtlUint64 CommandMessage::CalculateContentBufferSize()
{
  // Body pack size is the sum of ENCODING, LENGTH and STRING fields
  return sizeof(igtl_command_header) + this->m_Command.length();
}


int CommandMessage::PackContent()
{
  // Allocate buffer
  AllocateBuffer();
  igtl_command_header * command_header;
  char * command;

  // Set pointers
#if OpenIGTLink_HEADER_VERSION >= 2
  command_header = (igtl_command_header*) this->m_Content;
  command        = (char *) this->m_Content + sizeof(igtl_command_header);
#else
  command_header = (igtl_command_header*) this->m_Body;
  command        = (char *) this->m_Body + sizeof(igtl_command_header);
#endif

  // Copy data
  command_header->encoding = static_cast<igtlUint16>(this->m_Encoding);
  command_header->length   = static_cast<igtlUint32>(this->m_Command.length());
  command_header->commandId = this->m_CommandId;
  memcpy(command_header->commandName, this->m_CommandName, IGTL_COMMAND_NAME_SIZE);
  strncpy(command, this->m_Command.c_str(), command_header->length);

  // Convert byte order from host to network
  igtl_command_convert_byte_order(command_header);

  return 1;
}

int CommandMessage::UnpackContent()
{
  igtl_command_header * command_header;
  char * command;

#if OpenIGTLink_HEADER_VERSION >= 2
  command_header = (igtl_command_header*) this->m_Content;
  command        = (char *) this->m_Content + sizeof(igtl_command_header);
#else
  command_header = (igtl_command_header*) this->m_Body;
  command        = (char *) this->m_Body + sizeof(igtl_command_header);
#endif

  // Convert byte order from network to host
  igtl_command_convert_byte_order(command_header);
  
  // Copy data
  this->m_CommandId = command_header->commandId;
  memcpy(m_CommandName, command_header->commandName, IGTL_COMMAND_NAME_SIZE);
  this->m_Encoding = command_header->encoding;
  this->m_Command.clear();
  this->m_Command.append(command, command_header->length);

  return 1;
}


int RTSCommandMessage::SetCommandErrorString(const char* anErrorString)
{
  if (strlen(anErrorString) > IGTL_COMMAND_NAME_SIZE) /* If the length is beyond the range specified by the spec */
    {
    return 0;
    }
  m_IsBodyPacked = false;
  strcpy((char*)m_CommandName, anErrorString);
  return 1;
}


int RTSCommandMessage::SetCommandErrorString(const std::string& anErrorString)
{
  if( anErrorString.length() > IGTL_COMMAND_NAME_SIZE )
    {
    return 0;
    }
  m_IsBodyPacked = false;
  strcpy((char*)m_CommandName, anErrorString.c_str());
  return 1;
}


std::string RTSCommandMessage::GetCommandErrorString() const
{
  return std::string((char*)this->m_CommandName);
}


RTSCommandMessage::RTSCommandMessage()
  : CommandMessage()
{
  this->m_SendMessageType = std::string("RTS_COMMAND");
}


RTSCommandMessage::~RTSCommandMessage()
{

}

} // namespace igtl
