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

void igtl_export igtl_extended_header_convert_byte_order(igtl_extended_header * extended_header)
{
  if (igtl_is_little_endian()) {
    extended_header->extended_header_size   = BYTE_SWAP_INT32(extended_header->extended_header_size);
    extended_header->meta_data_size   = BYTE_SWAP_INT32(extended_header->meta_data_size);
    extended_header->msg_id   = BYTE_SWAP_INT32(extended_header->msg_id);
    extended_header->reserved   = BYTE_SWAP_INT16(extended_header->reserved);
  }
}