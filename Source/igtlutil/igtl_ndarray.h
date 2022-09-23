/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_NDARRAY_H
#define __IGTL_NDARRAY_H

#include "igtl_win32header.h"
#include "igtl_util.h"
#include "igtl_types.h"
#include "igtl_win32header.h"

#define IGTL_NDARRAY_HEADER_SIZE          2

/* Scalar type */
#define IGTL_NDARRAY_STYPE_TYPE_INT8      2
#define IGTL_NDARRAY_STYPE_TYPE_UINT8     3
#define IGTL_NDARRAY_STYPE_TYPE_INT16     4
#define IGTL_NDARRAY_STYPE_TYPE_UINT16    5
#define IGTL_NDARRAY_STYPE_TYPE_INT32     6
#define IGTL_NDARRAY_STYPE_TYPE_UINT32    7
#define IGTL_NDARRAY_STYPE_TYPE_FLOAT32   10
#define IGTL_NDARRAY_STYPE_TYPE_FLOAT64   11
#define IGTL_NDARRAY_STYPE_TYPE_COMPLEX   13

#define IGTL_NDARRAY_HOST_TO_NETWORK      0
#define IGTL_NDARRAY_NETWORK_TO_HOST      1


#ifdef __cplusplus
extern "C" {
#endif

/** NDARRAY is a data type, which is designed to transfer N-dimensional numerical array.
 *  The message body consists of N-D array header, size table, and N-D array body. */
typedef struct {
  igtl_uint8     type;   /* Scalar type (2:int8 3:uint8 4:int16 5:uint16 6:int32
                            7:uint32 10:float32 11:float64 13:complex) */
  igtl_uint8     dim;    /* Dimension of array */
  igtl_uint16 *  size;   /* Array size */
  void *         array;
} igtl_ndarray_info;


/** Initializes igtl_ndarray_info */
void igtl_export igtl_ndarray_init_info(igtl_ndarray_info * info);

/** Allocates size array and ND-array pointed from igtl_ndarray_info.
 *  'type' and 'dim' in igtl_ndarray_info must be specified before
 *  calling igtl_ndarray_alloc_info().
 *
 *  For example: Given info->type = 10 (i.e. 4 bytes), info->dim = 2 and size[dim] = {2, 3},
 *  the amount of memory allocated is 2 * 3 * 4 = 24 bytes.
 */
int igtl_export igtl_ndarray_alloc_info(igtl_ndarray_info * info, const igtl_uint16 * size);

/** Frees ndarray */
int igtl_export igtl_ndarray_free_info(igtl_ndarray_info * info);

/** Unpacks and extracts information in a byte array of NDARRAY messages and store
 * it in a igtl_ndarray_info structure. 'type' argument specifies
 * a message type prefix (none, GET_, STT_, STP_ or RTS_) by IGTL_TYPE_PREFIX_* macro.
 * Returns 1 if success, otherwise 0. */
int igtl_export igtl_ndarray_unpack(int type, void * byte_array, igtl_ndarray_info * info, igtl_uint64 pack_size);

/** Converts an igtl_ndarray_info structure to a byte array. 
 *  'byte_array' should be allocated prior to calling igtl_ndarray_pack() with memory size
 *  calculated by igtl_ndarray_get_size(). 'type' argument specifies a message type prefix
 *  (none, or GET_) by IGTL_TYPE_PREFIX_* macro. Returns 1 if success, otherwise 0. */
int igtl_export igtl_ndarray_pack(igtl_ndarray_info * info, void * byte_array, int type);

/** Calculates size of N-D array body including
 * size table (defined by UINT16[dim]) and array data. */
igtl_uint64 igtl_export igtl_ndarray_get_size(igtl_ndarray_info * info, int type);

/** Calculates CRC of image data body including header and array of pixel data. */
igtl_uint64 igtl_export igtl_ndarray_get_crc(igtl_ndarray_info * info, int type, void* byte_array);


#ifdef __cplusplus
}
#endif

#endif /* __IGTL_NDARRAY_H */
