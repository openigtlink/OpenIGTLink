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

#include "igtl_qtdata.h"
#include "igtl_util.h"

void igtl_export igtl_qtdata_convert_byte_order(igtl_qtdata_element* qtdatalist, int nitem)
{
  igtl_qtdata_element* elem;
  int i;
  int j;
  int k;
  igtl_int32* tmp;
      
  for (i = 0; i < nitem; i ++)
    {
    elem = &(qtdatalist[i]);
    if (igtl_is_little_endian()) 
      {
      for (j = 0; j < 3; j ++)
        {
        tmp = (igtl_int32*)&(elem->position[j]);
        *tmp = BYTE_SWAP_INT32(*tmp);
        }
      for (k = 0; k < 4; k ++)
        {
        tmp = (igtl_int32*)&(elem->quaternion[k]);
        *tmp = BYTE_SWAP_INT32(*tmp);
        }
      }
    }
}


void igtl_export igtl_stt_qtdata_convert_byte_order(igtl_stt_qtdata* stt_qtdata)
{
  igtl_int32* tmp;

  if (igtl_is_little_endian()) 
    {
    tmp = (igtl_int32*)&(stt_qtdata->resolution);
    *tmp = BYTE_SWAP_INT32(*tmp);
    }
}


void igtl_export igtl_rts_qtdata_convert_byte_order(igtl_rts_qtdata* rts_qtdata)
{
  /* do nothing */
}


igtl_uint64 igtl_export igtl_qtdata_get_crc(igtl_qtdata_element* qtdatalist, int nitem)
{
  igtl_qtdata_element* elem;
  int i;
  igtl_uint64  crc;

  crc = crc64(0, 0, 0);
  for (i = 0; i < nitem; i ++)
    {
    elem = &(qtdatalist[i]);
    crc = crc64((unsigned char*) elem, IGTL_QTDATA_ELEMENT_SIZE, crc);
    }

  return crc;
}


igtl_uint64 igtl_export igtl_stt_qtdata_get_crc(igtl_stt_qtdata* stt_qtdata)
{
  igtl_uint64  crc;

  crc = crc64(0, 0, 0);
  crc = crc64((unsigned char*) stt_qtdata, IGTL_STT_QTDATA_SIZE, crc);
  return crc;
}


igtl_uint64 igtl_export igtl_rts_qtdata_get_crc(igtl_rts_qtdata* rts_qtdata)
{
  igtl_uint64  crc;

  crc = crc64(0, 0, 0);
  crc = crc64((unsigned char*) rts_qtdata, IGTL_RTS_QTDATA_SIZE, crc);
  return crc;
}

