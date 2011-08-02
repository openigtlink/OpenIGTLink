/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlutil/igtl_header.c $
  Language:  C
  Date:      $Date: 2009-11-13 11:37:44 -0500 (Fri, 13 Nov 2009) $
  Version:   $Revision: 5335 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtl_util.h"
#include "igtl_header.h"

void igtl_export igtl_header_convert_byte_order(igtl_header * header)
{
  if (igtl_is_little_endian()) {
    header->version   = BYTE_SWAP_INT16(header->version);
    header->timestamp = BYTE_SWAP_INT64(header->timestamp);
    header->body_size = BYTE_SWAP_INT64(header->body_size);
    header->crc       = BYTE_SWAP_INT64(header->crc);
  }  
}

