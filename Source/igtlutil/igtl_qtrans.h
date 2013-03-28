/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_QTRANS_H
#define __IGTL_QTRANS_H

#include "igtl_win32header.h"
#include "igtl_util.h"
#include "igtl_types.h"

#define  IGTL_QTRANS_MESSAGE_DEFAULT_SIZE           28
          /** NOTE: the size varies if orientation is omitted **/
#define  IGTL_QTRANS_MESSAGE_POSITON_ONLY_SIZE      12  /* size w/o quaternion       */
#define  IGTL_QTRANS_MESSAGE_WITH_QUATERNION3_SIZE  24  /* size 3-element quaternion */


#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

/** Status data header for OpenIGTLinik protocol */
typedef struct {
  igtl_float32 qtrans[3];    /* (x, y, z) */
  igtl_float32 quaternion[4];  /* (ox, oy, oz, w) */
} igtl_qtrans;

#pragma pack()

/** Converts endianness of each member variable in igtl_status_header
 *  from host byte order to network byte order, or vice versa. */
void igtl_export igtl_qtrans_convert_byte_order(igtl_qtrans* pos);

  /** Calculates CRC of qtrans message */
igtl_uint64 igtl_export igtl_qtrans_get_crc(igtl_qtrans* pos);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_QTRANS_H */


