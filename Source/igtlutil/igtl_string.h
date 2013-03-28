/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_STRING_H
#define __IGTL_STRING_H

#include "igtl_win32header.h"
#include "igtl_util.h"
#include "igtl_types.h"
#include "igtl_win32header.h"

#define IGTL_STRING_HEADER_SIZE          4

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

typedef struct {
  igtl_uint16    encoding;         /* Character encoding type as MIBenum value (defined by IANA). Default=3. */
                                   /* Please refer http://www.iana.org/assignments/character-sets for detail */
  igtl_uint16    length;           /* Length of string */
} igtl_string_header;

#pragma pack()

/** Calculates size of the pixel array, which will be
 *  transferred with the specified header. */
igtl_uint32 igtl_export igtl_string_get_string_length(igtl_string_header * header);

/** Converts endianness from host byte order to network byte order,
 *  or vice versa. NOTE: It is developer's responsibility to have the string body with BOM
 *  (byte order mark) or in big endian ordrer. */
void igtl_export igtl_string_convert_byte_order(igtl_string_header * header);

/** Calculates CRC of image data body including header
 *  and array of pixel data. */
igtl_uint64 igtl_export igtl_string_get_crc(igtl_string_header * header, void* string);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_STRING_H */


