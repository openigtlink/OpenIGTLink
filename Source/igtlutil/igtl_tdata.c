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

#include "igtl_tdata.h"
#include "igtl_util.h"

void igtl_export igtl_tdata_convert_byte_order(igtl_tdata_element* tdatalist, int nitem)
{
  igtl_tdata_element* elem;
  int i;
  int j;
  igtl_int32* tmp;
      
  for (i = 0; i < nitem; i ++)
    {
    elem = &(tdatalist[i]);
    if (igtl_is_little_endian()) 
      {
      for (j = 0; j < 12; j ++)
        {
        tmp = (igtl_int32*)&(elem->transform[j]);
        *tmp = BYTE_SWAP_INT32(*tmp);
        }
      }
    }
}


void igtl_export igtl_stt_tdata_convert_byte_order(igtl_stt_tdata* stt_tdata)
{
  igtl_int32* tmp;

  if (igtl_is_little_endian()) 
    {
    tmp = (igtl_int32*)&(stt_tdata->resolution);
    *tmp = BYTE_SWAP_INT32(*tmp);
    }
}


void igtl_export igtl_rts_tdata_convert_byte_order(igtl_rts_tdata* rts_tdata)
{
  /* do nothing */
}


igtl_uint64 igtl_export igtl_tdata_get_crc(igtl_tdata_element* tdatalist, int nitem)
{
  igtl_tdata_element* elem;
  int i;
  igtl_uint64  crc;

  crc = crc64(0, 0, 0);
  for (i = 0; i < nitem; i ++)
    {
    elem = &(tdatalist[i]);
    crc = crc64((unsigned char*) elem, IGTL_TDATA_ELEMENT_SIZE, crc);
    }

  return crc;
}


igtl_uint64 igtl_export igtl_stt_tdata_get_crc(igtl_stt_tdata* stt_tdata)
{
  igtl_uint64  crc;

  crc = crc64(0, 0, 0);
  crc = crc64((unsigned char*) stt_tdata, IGTL_STT_TDATA_SIZE, crc);
  return crc;
}


igtl_uint64 igtl_export igtl_rts_tdata_get_crc(igtl_rts_tdata* rts_tdata)
{
  igtl_uint64  crc;

  crc = crc64(0, 0, 0);
  crc = crc64((unsigned char*) rts_tdata, IGTL_RTS_TDATA_SIZE, crc);
  return crc;
}

