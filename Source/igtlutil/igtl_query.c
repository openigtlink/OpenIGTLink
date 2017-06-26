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

#include "igtl_query.h"
#include "igtl_util.h"


void igtl_export igtl_query_convert_byte_order(igtl_query_header* header)
{

  if (igtl_is_little_endian()) 
    {
    header->queryID   = BYTE_SWAP_INT32(header->queryID);
    header->deviceUIDLength   = BYTE_SWAP_INT16(header->deviceUIDLength);
    }
}


igtl_uint64 igtl_export igtl_query_get_crc(igtl_query_header * header, void* query)
{
  igtl_uint64   crc;
  igtl_uint32   query_length;

  /* convert byte order to get query length */
  igtl_query_convert_byte_order(header);
  query_length = (igtl_uint32)(header->deviceUIDLength);
  igtl_query_convert_byte_order(header);

  crc = crc64(0, 0, 0);
  crc = crc64((unsigned char*) header, IGTL_QUERY_HEADER_SIZE, crc);
  crc = crc64((unsigned char*) query, query_length, crc);

  return crc;
}

