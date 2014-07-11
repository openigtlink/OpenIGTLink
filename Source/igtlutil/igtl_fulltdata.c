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

#include "igtl_fulltdata.h"
#include "igtl_util.h"

void igtl_export igtl_fulltdata_convert_byte_order(igtl_fulltdata_element* tdatalist, int nitem)
{
  igtl_fulltdata_element* elem;
  int i;
  int j;
  igtl_int32* tmp;
      
  for (i = 0; i < nitem; i ++)
    {
    elem = &(tdatalist[i]);
    if (igtl_is_little_endian()) 
      {
      for (j = 0; j < 16; j ++)
        {
        tmp = (igtl_int32*)&(elem->transform[j]);
        *tmp = BYTE_SWAP_INT32(*tmp);
        }
      tmp = ( igtl_int32*)&(elem->error);
      *tmp = BYTE_SWAP_INT32(*tmp);
      }

    }
}


igtl_uint64 igtl_export igtl_fulltdata_get_crc(igtl_fulltdata_element* tdatalist, int nitem)
{
  igtl_fulltdata_element* elem;
  int i;
  igtl_uint64  crc;

  crc = crc64(0, 0, 0);
  for (i = 0; i < nitem; i ++)
    {
    elem = &(tdatalist[i]);
    crc = crc64((unsigned char*) elem, IGTL_FULLTDATA_ELEMENT_SIZE, crc);
    }

  return crc;
}


