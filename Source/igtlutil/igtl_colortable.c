/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtl_util.h"
#include "igtl_colortable.h"

igtl_uint64 igtl_export igtl_colortable_get_table_size(igtl_colortable_header* header)
{
  igtl_int32 n;
  igtl_int32 s;

  if (header->indexType == IGTL_COLORTABLE_INDEX_UINT8)
    {
    n = 256;
    }
  else /* IGTL_COLORTABLE_INDEX_UINT64 */
    {
    n = 256*256;
    }

  if (header->mapType == IGTL_COLORTABLE_MAP_UINT8)
    {
    s = 1;
    }
  else if (header->mapType == IGTL_COLORTABLE_MAP_UINT16)
    {
    s = 2;
    }
  else /* IGTL_COLORTABLE_MAP_RGB */
    {
    s = 3;
    }
  
  return (igtl_uint64) n*s;
}


void igtl_export igtl_colortable_convert_byte_order(igtl_colortable_header* header, void* table)
{
  int i;
  int n;
  igtl_uint16* tmp;
  
  if (igtl_is_little_endian()) 
    {
    if (header->mapType == IGTL_COLORTABLE_MAP_UINT16)
      {
      n = (header->indexType == IGTL_COLORTABLE_INDEX_UINT16)? 256*256 : 256;
      tmp = (igtl_uint16*) table;
      for (i = 0; i < n; i ++)
        {
        tmp[i] = BYTE_SWAP_INT16(tmp[i]);
        }
      }
    }
}


igtl_uint64 igtl_export igtl_colortable_get_crc(igtl_colortable_header* header, void* table)
{
  igtl_uint64   crc;
  igtl_uint64   data_size;

  /* calculate header size using igtl_colortable_get_table_size().
   * This funciton can be called after byte order conversion, since
   * both index and map type field are uint8 and not affected by byte conversion.
   */

  data_size = igtl_colortable_get_table_size(header);
  crc = crc64(0, 0, 0);
  crc = crc64((unsigned char*) header, IGTL_COLORTABLE_HEADER_SIZE, crc);
  crc = crc64((unsigned char*) table, (int)data_size, crc);

  return crc;
}
