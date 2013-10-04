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

#include "igtl_sensor.h"
#include "igtl_util.h"

igtl_uint32 igtl_export igtl_sensor_get_data_size(igtl_sensor_header * header)
{
 
  igtl_uint32 data_size;

  data_size = (igtl_uint32)(header->larray) * sizeof(igtl_float64);

  return data_size;
}


void igtl_export igtl_sensor_convert_byte_order(igtl_sensor_header* header, igtl_float64* data)
{
  int i;
  int larray;
  igtl_uint64* tmp;

  if (igtl_is_little_endian()) 
    {
    larray = (int) header->larray; /* NOTE: larray is 8-bit (doesn't depend on endianness) */
    header->unit = BYTE_SWAP_INT64(header->unit);
    
    tmp = (igtl_uint64*) data;
    for (i = 0; i < larray; i ++)
      {
      tmp[i] = BYTE_SWAP_INT64(tmp[i]);
      }
    }
}


igtl_uint64 igtl_export igtl_sensor_get_crc(igtl_sensor_header * header, igtl_float64* data)
{
  igtl_uint64   crc;
  igtl_uint64   data_size;

  data_size = (igtl_uint32)(header->larray) * sizeof(igtl_float64);

  crc = crc64(0, 0, 0);
  crc = crc64((unsigned char*) header, IGTL_SENSOR_HEADER_SIZE, crc);
  crc = crc64((unsigned char*) data, (int)data_size, crc);

  return crc;
}
