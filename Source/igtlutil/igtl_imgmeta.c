/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink2_beta/Source/igtlutil/igtl_image.c $
  Language:  C
  Date:      $Date: 2010-04-15 15:51:48 -0400 (Thu, 15 Apr 2010) $
  Version:   $Revision: 6269 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <string.h>
#include <math.h>

#include "igtl_imgmeta.h"
#include "igtl_util.h"

void igtl_export igtl_imgmeta_convert_byte_order(igtl_imgmeta_element* metalist, int nitem)
{
  igtl_imgmeta_element* elem;
  int i;
      
  for (i = 0; i < nitem; i ++)
    {
    elem = &(metalist[i]);
    if (igtl_is_little_endian()) 
      {
      elem->timestamp = BYTE_SWAP_INT64(elem->timestamp);
      elem->size[0] = BYTE_SWAP_INT16(elem->size[0]);
      elem->size[1] = BYTE_SWAP_INT16(elem->size[1]);
      elem->size[2] = BYTE_SWAP_INT16(elem->size[2]);
      }
    }
}


igtl_uint64 igtl_export igtl_imgmeta_get_crc(igtl_imgmeta_element* metalist, int nitem)
{
  igtl_imgmeta_element* elem;
  int i;
  igtl_uint64  crc;

  crc = crc64(0, 0, 0);
  for (i = 0; i < nitem; i ++)
    {
    elem = &(metalist[i]);
    crc = crc64((unsigned char*) elem, IGTL_IMGMETA_ELEMENT_SIZE, crc);
    }

  return crc;
}
