/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_UTIL_H
#define __IGTL_UTIL_H

#include <stdio.h>
#include "igtl_win32header.h"
#include "igtl_types.h"

#ifdef __cplusplus
extern "C" {
#endif



/* Scalar type for point data */
#define IGTL_SCALAR_INT8      2
#define IGTL_SCALAR_UINT8     3
#define IGTL_SCALAR_INT16     4
#define IGTL_SCALAR_UINT16    5
#define IGTL_SCALAR_INT32     6
#define IGTL_SCALAR_UINT32    7
#define IGTL_SCALAR_FLOAT32   10
#define IGTL_SCALAR_FLOAT64   11
#define IGTL_SCALAR_COMPLEX   13


/** Byte order conversion macros */
#define BYTE_SWAP_INT16(S) (((S) & 0xFF) << 8 \
                            | (((S) >> 8) & 0xFF))
#define BYTE_SWAP_INT32(L) ((BYTE_SWAP_INT16 ((L) & 0xFFFF) << 16) \
                            | BYTE_SWAP_INT16 (((L) >> 16) & 0xFFFF))
#define BYTE_SWAP_INT64(LL) ((BYTE_SWAP_INT32 ((LL) & 0xFFFFFFFF) << 32) \
                             | BYTE_SWAP_INT32 (((LL) >> 32) & 0xFFFFFFFF))

/** Tests endian of the host */
int igtl_export igtl_is_little_endian();
igtl_uint64 igtl_export crc64(unsigned char *data, igtl_uint64 len, igtl_uint64 crc);

/** Converts nanosecond to fraction / fraction to nanosec. */
igtl_uint32 igtl_export igtl_nanosec_to_frac(igtl_uint32 nanosec);
igtl_uint32 igtl_export igtl_frac_to_nanosec(igtl_uint32 frac);
void igtl_export igtl_message_dump_hex(FILE* stream, const void* message, int max_size);

/** Gets size of scalar. Type should be IGTL_SCALAR_* */
igtl_uint32 igtl_export igtl_get_scalar_size(int type);

#ifdef __cplusplus
}
#endif

#endif  /*__IGTL_UTIL_H*/

