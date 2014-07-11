/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_IMAGE_H
#define __IGTL_IMAGE_H

#include "igtl_win32header.h"
#include "igtl_util.h"
#include "igtl_types.h"
#include "igtl_win32header.h"

#define IGTL_IMAGE_HEADER_VERSION       1
#define IGTL_IMAGE_HEADER_SIZE          72

/* Data type */
#define IGTL_IMAGE_DTYPE_SCALAR         1
#define IGTL_IMAGE_DTYPE_VECTOR         3

/* Scalar type */
#define IGTL_IMAGE_STYPE_TYPE_INT8      2
#define IGTL_IMAGE_STYPE_TYPE_UINT8     3
#define IGTL_IMAGE_STYPE_TYPE_INT16     4
#define IGTL_IMAGE_STYPE_TYPE_UINT16    5
#define IGTL_IMAGE_STYPE_TYPE_INT32     6
#define IGTL_IMAGE_STYPE_TYPE_UINT32    7
#define IGTL_IMAGE_STYPE_TYPE_FLOAT32   10
#define IGTL_IMAGE_STYPE_TYPE_FLOAT64   11

/* Endian */
#define IGTL_IMAGE_ENDIAN_BIG           1
#define IGTL_IMAGE_ENDIAN_LITTLE        2

/* Image coordinate system */
#define IGTL_IMAGE_COORD_RAS            1
#define IGTL_IMAGE_COORD_LPS            2

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

/** Image data consists of image data header, which is defined in this
 *  structure, folowed by array of image pixel data.
 *  igtl_image_header helps a receiver to load array of image pixel data.
 *  The header supports "partial volume update", where a fraction of volume
 *  image is transferred from a sender to receiver. This fraction called
 *  "sub-volume" in this protocol, and its size and starting index is
 *  specified in 'subvol_size' and 'subvol_offset'.
 *  In case of transferring entire image in one message, 'size' and
 *  'subvol_size' should be same, and 'subvol_offset' equals (0, 0, 0). */
typedef struct {
  igtl_uint16    version;          /* data format version number(1)   */
  igtl_uint8     num_components;   /* number of components per element*/
  igtl_uint8     scalar_type;      /* scalar type                     */
  /*2:int8 3:uint8 4:int16 5:uint16 6:int32 7:uint32 10:float32 11:float64) */
  igtl_uint8     endian;           /* endian type of image data       */
                                   /* (1:big, 2:little)               */ 
  igtl_uint8     coord;            /* coordinate system (1:RAS 2:LPS) */
  igtl_uint16    size[3];          /* entire image volume size        */
  igtl_float32   matrix[12];       /* orientation / origin of image   */
                                   /*  - matrix[0-2]: norm_i * pix_i  */
                                   /*  - matrix[3-5]: norm_j * pix_j  */
                                   /*  - matrix[6-8]: norm_k * pix_k  */
                                   /*  - matrix[9-11]:origin          */
                                   /* where norm_* are normal vectors */
                                   /* along each index, and pix_* are */
                                   /* pixel size in each direction    */

  igtl_uint16    subvol_offset[3]; /* sub volume offset               */
  igtl_uint16    subvol_size[3];   /* sub volume size                 */
} igtl_image_header;

#pragma pack()


/** Calculates size of the pixel array, which will be transferred with the specified header. */
igtl_uint64 igtl_export igtl_image_get_data_size(igtl_image_header * header);


/** Generates image orientation/origin matrix from spacing, origin and normal vectors. */
void igtl_export igtl_image_set_matrix(float spacing[3], float origin[3],
                            float norm_i[3], float norm_j[3], float norm_k[3],
                            igtl_image_header * header);

void igtl_export igtl_image_get_matrix(float spacing[3], float origin[3],
                            float norm_i[3], float norm_j[3], float norm_k[3],
                            igtl_image_header * header);

/** Sets the image orientation/origin matrix in 4x4 format */
void igtl_export igtl_image_set_matrix_4x4(float _matrix[4][4],igtl_image_header * header);

/** Gets the image orientation/origin matrix in 4x4 format */
void igtl_export igtl_image_get_matrix_4x4(float _matrix[4][4],igtl_image_header * header);

/** Converts endianness of each member variable in igtl_image_header from host
 *  byte order to network byte order, or vice versa. */
void igtl_export igtl_image_convert_byte_order(igtl_image_header * header);

/** Calculates CRC of image data body including header and array of pixel data. */
igtl_uint64 igtl_export igtl_image_get_crc(igtl_image_header * header, void* image);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_IMAGE_H */
