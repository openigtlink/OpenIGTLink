/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_POLYDATA_H
#define __IGTL_POLYDATA_H

#include "igtl_win32header.h"
#include "igtl_header.h"
#include "igtl_util.h"
#include "igtl_types.h"
#include "igtl_win32header.h"


#ifdef __cplusplus
extern "C" {
#endif


#define IGTL_POLY_MAX_ATTR_NAME_LEN 255

#define IGTL_POLY_ATTR_TYPE_SCALAR 0x00
#define IGTL_POLY_ATTR_TYPE_VECTOR 0x01
#define IGTL_POLY_ATTR_TYPE_NORMAL 0x02
#define IGTL_POLY_ATTR_TYPE_TENSOR 0x03
#define IGTL_POLY_ATTR_TYPE_RGBA   0x04

#pragma pack(1)     /* For 1-byte boundary in memroy */

/** POLYDATA Header */
typedef struct {
  igtl_uint32      npoints;                  /* Number of points */

  igtl_uint32      nvertices;                /* Number of vertices */
  igtl_uint32      size_vertices;            /* Size of vertice data (bytes) */

  igtl_uint32      nlines;                   /* Number of lines */
  igtl_uint32      size_lines;               /* Size of line data (bytes) */

  igtl_uint32      npolygons;                /* Number of polygons */
  igtl_uint32      size_polygons;            /* Size of polygon data (bytes) */

  igtl_uint32      ntriangle_strips;         /* Number of triangle strips */
  igtl_uint32      size_triangle_strips;     /* Size of triangle strips data (bytes) */

  igtl_uint32      nattributes;              /* Number of attributes */
} igtl_polydata_header;


typedef struct {
  igtl_uint8       type;                     /* attribute type */
                                             /* Values for TYPE_ATTRIBUTE (16-bit)
                                                0x00: POINT_DATA / Scalars
                                                0x01: POINT_DATA / Vectors
                                                0x02: POINT_DATA / Normals
                                                0x03: POINT_DATA / Tensors
                                                0x10: CELL_DATA / Scalars
                                                0x11: CELL_DATA / Vectors
                                                0x12: CELL_DATA / Normals
                                                0x13: CELL_DATA Tensors  */
  igtl_uint8       ncomponents;              /* number of components */
                                             /* must be 3 for Vectors and Normal, 9 for Tensor.*/
  igtl_uint32      n;
} igtl_polydata_attribute_header;

#pragma pack()

/** Attribute info */
typedef struct {
  igtl_uint8       type;
  igtl_uint8       ncomponents;
  igtl_uint32      n;
  char *           name;
  igtl_float32 *   data;
} igtl_polydata_attribute;

/** POLYDATA info */
typedef struct {
  igtl_polydata_header   header;             /* Header */
  igtl_float32*                 points;             /* Points */
  igtl_uint32 *          vertices;           /* Vertices -- array of (N, i1, i2, i3 ...iN) */
  igtl_uint32 *          lines;              /* Lines -- array of (N, i1, i2, i3 ...iN) */
  igtl_uint32 *          polygons;           /* Polygons -- array of (N, i1, i2, i3 ...iN) */
  igtl_uint32 *          triangle_strips;    /* Triangle strips -- array of (N, i1, i2, i3 ...iN) */
  igtl_polydata_attribute * attributes; /* Array of attributes */
} igtl_polydata_info;


/** Initializes igtl_polydata_info */
void igtl_export igtl_polydata_init_info(igtl_polydata_info * info);

/** Allocates free arrays in polydata_info.
 *  Note that igtl_polydata_alloc_info() does not allocate memory for 'name' and 'data'
 *  in each igtl_polydata_attribute. Those elements have to be allocated in the developers
 *  responsibility. 
 *  igtl_polydata_free_info() function assumes that igtl_polydata_info is allocated by
 *  igtl_polydata_alloc_info() and all memory blocks pointed from igtl_polydata_attribute
 *  have been allocated by malloc().
 *  Return 1 if the array is successfully allocated/freed. */
int igtl_export igtl_polydata_alloc_info(igtl_polydata_info * info);
int igtl_export igtl_polydata_free_info(igtl_polydata_info * info);

/** Extracts information about child messages in a byte array of POLYDATA messages and store
 *  it in a igtl_polydata_info structure. 'type' argument specifies a message type prefix
 *  (none, GET_, STT_, STP_ or RTS_) by IGTL_TYPE_PREFIX_* macro.
 *  Returns 1 if success, otherwise 0. */
int igtl_export igtl_polydata_unpack(int type, void * byte_array, igtl_polydata_info * info, igtl_uint64 size);

/** Converts an igtl_polydata_info structure to a byte array. 
 *  'byte_array' should be allocated prior to calling igtl_polydata_pack() with memory size
 *  calculated by igtl_polydata_get_size(). 'type' argument specifies a message type prefix
 *  (none, GET_, STT_, STP_ or RTS_) by IGTL_TYPE_PREFIX_* macro.
 *  Returns 1 if success, otherwise 0. */
int igtl_export igtl_polydata_pack(igtl_polydata_info * info, void * byte_array, int type);

/** igtl_polydata_get_size() calculates the size of polydata header, consisting of
 *  POLYDATA hearder section (including number of child messages) and
 *  name table section based on a igtl_polydata_header.
 *  The size returned from this function does not include size of child message data.
 *  'type' argument specifies a message type prefix
 *  (none, GET_, STT_, STP_ or RTS_) by IGTL_TYPE_PREFIX_* macro. */
igtl_uint64 igtl_export igtl_polydata_get_size(igtl_polydata_info * info, int type);

/** Calculates CRC of POLYDATA message. Note that 'info' is used only for
 * getting size of the message. */
igtl_uint64 igtl_export igtl_polydata_get_crc(igtl_polydata_info * info, int type, void* polydata_message);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_POLYDATA_H */


