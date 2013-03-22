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

#include "igtl_string.h"
#include "igtl_util.h"

igtl_uint32 igtl_export igtl_string_get_string_length(igtl_string_header * header)
{
 
  igtl_uint32 length;
  length = (igtl_uint32)(header->length);
  
  return length;
}


void igtl_export igtl_string_convert_byte_order(igtl_string_header* header)
{

  if (igtl_is_little_endian()) 
    {
    header->encoding = BYTE_SWAP_INT16(header->encoding);
    header->length   = BYTE_SWAP_INT16(header->length);
    }
}


igtl_uint64 igtl_export igtl_string_get_crc(igtl_string_header * header, void* string)
{
  igtl_uint64   crc;
  igtl_uint64   string_length;

  /* convert byte order to get string length */
  igtl_string_convert_byte_order(header);
  string_length = (igtl_uint32)(header->length);
  igtl_string_convert_byte_order(header);

  crc = crc64(0, 0, 0);
  crc = crc64((unsigned char*) header, IGTL_STRING_HEADER_SIZE, crc);
  crc = crc64((unsigned char*) string, (int)string_length, crc);

  return crc;
}

