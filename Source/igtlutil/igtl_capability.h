/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_CAPABILITY_H
#define __IGTL_CAPABILITY_H

#include "igtl_win32header.h"
#include "igtl_util.h"
#include "igtl_types.h"
#include "igtl_win32header.h"

#define IGTL_CAPABILITY_HEADER_SIZE          4

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  igtl_uint32      ntypes;
  unsigned char ** typenames;
} igtl_capability_info;


/** Initializes igtl_ndarray_info */
void igtl_export igtl_capability_init_info(igtl_capability_info * info);

/** Allocates ndarray info. Allocate size array and ND-array pointed from igtl_ndarray_info.
 * 'type' and 'dim' in igtl_ndarray_info must be specified before
 * calling igtl_ndarray_alloc_info(). */
int igtl_export igtl_capability_alloc_info(igtl_capability_info * info, int ntypes);

/** Frees ndarray info. */
int igtl_export igtl_capability_free_info(igtl_capability_info * info);

/** Calculates capability data size of the pixel array, which will be
 *  transferred with the specified header. */
igtl_uint32 igtl_export igtl_capability_get_length(igtl_capability_info * info);

/** Unpacks CAPABILITY message. Extracts information in a byte array of CAPABILITY
 *  messages and store it in a igtl_capability_info structure. 'type' argument specifies
 *  a message type prefix (none, GET_, STT_, STP_ or RTS_) by IGTL_TYPE_PREFIX_* macro.
 *  Returns 1 if success, otherwise 0. */
int igtl_export igtl_capability_unpack(void * byte_array, igtl_capability_info * info, igtl_uint64 pack_size);

/** Packs CAPABILITY message. Converts an igtl_capability_info structure to a byte array. 
 * 'byte_array' should be allocated prior to calling igtl_capability_pack() with memory size
 * calculated by igtl_capability_get_size(). 'type' argument specifies a message type prefix
 * (none, or GET_) by IGTL_TYPE_PREFIX_* macro. Returns 1 if success, otherwise 0. */
int igtl_export igtl_capability_pack(igtl_capability_info * info, void * byte_array);

/** Calculates CRC of image data body including header
 *  and array of pixel data. */
igtl_uint64 igtl_export igtl_capability_get_crc(igtl_capability_info* info, void* capability);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_CAPABILITY_H */




