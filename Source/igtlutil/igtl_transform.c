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
#include "igtl_transform.h"
#include "igtl_util.h"
/*#include "crc32.h"*/

#include <stdio.h>

void igtl_export igtl_transform_convert_byte_order(igtl_float32* transform)
{
  int i;
  igtl_uint32 tmp[12];

  if (igtl_is_little_endian()) {
    memcpy(tmp, transform, sizeof(igtl_uint32)*12);
    for (i = 0; i < 12; i ++) {
      tmp[i] = BYTE_SWAP_INT32(tmp[i]);
    }
    memcpy(transform, tmp, sizeof(igtl_uint32)*12);
  }
}


igtl_uint64 igtl_export igtl_transform_get_crc(igtl_float32* transform)
{

  igtl_uint64 crc = crc64(0, 0, 0);

  crc = crc64((unsigned char*)transform, sizeof(igtl_float32)*12, crc);

  return crc;
}
