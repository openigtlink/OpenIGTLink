/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <string.h>
#include <math.h>

#include "igtl_command.h"
#include "igtl_util.h"


void igtl_export igtl_command_convert_byte_order(igtl_command_header* header)
{

  if (igtl_is_little_endian()) 
    {
      header->commandId   = BYTE_SWAP_INT32(header->commandId);
      header->encoding = BYTE_SWAP_INT16(header->encoding);
      header->length   = BYTE_SWAP_INT32(header->length);
    }
}


igtl_uint64 igtl_export igtl_command_get_crc(igtl_command_header * header, void* command)
{
  igtl_uint64   crc;
  igtl_uint32   command_length;

  /* convert byte order to get command length */
  igtl_command_convert_byte_order(header);
  command_length = (igtl_uint32)(header->length);
  igtl_command_convert_byte_order(header);

  crc = crc64(0, 0, 0);
  crc = crc64((unsigned char*) header, IGTL_COMMAND_HEADER_SIZE, crc);
  crc = crc64((unsigned char*) command, command_length, crc);

  return crc;
}

