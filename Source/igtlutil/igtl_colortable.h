/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_COLORTABLE_H
#define __IGTL_COLORTABLE_H

#include "igtl_win32header.h"
#include "igtl_util.h"
#include "igtl_types.h"

#define IGTL_COLORTABLE_HEADER_SIZE    2

#define IGTL_COLORTABLE_INDEX_UINT8    3
#define IGTL_COLORTABLE_INDEX_UINT16   5
#define IGTL_COLORTABLE_MAP_UINT8      3
#define IGTL_COLORTABLE_MAP_UINT16     5
#define IGTL_COLORTABLE_MAP_RGB        19

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

/** Status data header for OpenIGTLinik protocol */
typedef struct {
  igtl_int8    indexType;      /* Index Type: 3:uint8 5:uint16 */
  igtl_int8    mapType;        /* 3: uint8 5:uint16 19:RGB color */
} igtl_colortable_header;

#pragma pack()

/** igtl_colortable_get_data_size(n) calculates the size of body based on the index 
 *  and map types. The size of body is used in the message header.
 *  igtl_colortable_get_data_n(size) calculates the number of images in the body, based on
 *  the body size. This function may be used when a client program parses a COLORTABLE message. */
igtl_uint64 igtl_export igtl_colortable_get_table_size(igtl_colortable_header* header);

/** Converts endianness of each element in an array of
 *  igtl_igtl_colortable_header and color table from host byte order to network byte order,
 *  or vice versa (if necessary). The function automatically determins the endian type of the host. */
void igtl_export igtl_colortable_convert_byte_order(igtl_colortable_header* header, void* table);

/** Calculates CRC of color table message */
igtl_uint64 igtl_export igtl_colortable_get_crc(igtl_colortable_header* header, void* table);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_COLORTABLE_H */


