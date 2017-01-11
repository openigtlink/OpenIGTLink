/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_QUERY_H
#define __IGTL_QUERY_H

#include "igtl_win32header.h"
#include "igtl_util.h"
#include "igtl_types.h"
#include "igtl_win32header.h"

#define IGTL_QUERY_HEADER_SIZE    38
#define IGTL_QUERY_DATE_TYPE_SIZE     32
#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memory */

typedef struct {
  igtl_uint32    queryID;        /* The unique ID of this QUERY */
  igtl_uint8     queryDataType[IGTL_QUERY_DATE_TYPE_SIZE];  /* The query data type of the message */
  
  igtl_uint16    deviceUIDLength;           /* Length of DEVICE Name */
} igtl_query_header;

#pragma pack()

/** Converts endian-ness from host byte order to network byte order,
 *  or vice versa. NOTE: It is developer's responsibility to have the command body with BOM
 *  (byte order mark) or in big endian order. */
void igtl_export igtl_query_convert_byte_order(igtl_query_header * header);

/** Calculates CRC of image data body including header
 *  and array of pixel data. */
igtl_uint64 igtl_export igtl_query_get_crc(igtl_query_header * header, void* query);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_QUERY_H */