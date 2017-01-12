/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_COMMAND_H
#define __IGTL_COMMAND_H

#include "igtl_win32header.h"
#include "igtl_util.h"
#include "igtl_types.h"
#include "igtl_win32header.h"

#define IGTL_COMMAND_HEADER_SIZE    138
#define IGTL_COMMAND_NAME_SIZE      128

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memory */

typedef struct {
  igtl_uint32    commandId;        /* The unique ID of this command */
  igtl_uint8     commandName[IGTL_COMMAND_NAME_SIZE];  /* The name of this command */
  igtl_uint16    encoding;         /* Character encoding type as MIBenum value (defined by IANA). Default=3 */
                                   /* Please refer http://www.iana.org/assignments/character-sets for detail */
  igtl_uint32    length;           /* Length of command */
} igtl_command_header;

#pragma pack()

/** Converts endian-ness from host byte order to network byte order,
 *  or vice versa. NOTE: It is developer's responsibility to have the command body with BOM
 *  (byte order mark) or in big endian order. */
void igtl_export igtl_command_convert_byte_order(igtl_command_header * header);

/** Calculates CRC of image data body including header
 *  and array of pixel data. */
igtl_uint64 igtl_export igtl_command_get_crc(igtl_command_header * header, void* command);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_COMMAND_H */