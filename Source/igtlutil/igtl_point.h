/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_POINT_H
#define __IGTL_POINT_H

#include "igtl_win32header.h"
#include "igtl_util.h"
#include "igtl_types.h"

#define  IGTL_POINT_ELEMENT_SIZE           136

#define IGTL_POINT_LEN_NAME              64
#define IGTL_POINT_LEN_GROUP_NAME        32
#define IGTL_POINT_LEN_OWNER             20

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

/** Status data header for OpenIGTLinik protocol */
typedef struct {
  char         name[IGTL_POINT_LEN_NAME]; /* Name or description of the point */
  char         group_name[IGTL_POINT_LEN_GROUP_NAME]; /* Can be "Labeled Point", "Landmark", Fiducial", ... */
  igtl_uint8   rgba[4]; /* Color in R/G/B/A */
  igtl_float32 position[3]; /* Coordinate of the point */
  igtl_float32 radius; /* Radius of the point. Can be 0. */
  char         owner[IGTL_POINT_LEN_OWNER];/* Device name of the ower image */
} igtl_point_element;

#pragma pack()

/** igtl_point_get_data_size(n) calculates the size of body based on the number
 *  of points. The size of body is used in the message header.*/
#define igtl_point_get_data_size(n)  ((n) * IGTL_POINT_ELEMENT_SIZE)

/** igtl_point_get_data_n(size) calculates the number of images in the body, based on
 *  the body size. This function may be used when a client program parses a POINT message. */
#define igtl_point_get_data_n(size)  ((size) / IGTL_POINT_ELEMENT_SIZE)

/** Converts endianness of each element in an array of
 *  igtl_igtl_point_element from host byte order to network byte order,
 *  or vice versa. */
void igtl_export igtl_point_convert_byte_order(igtl_point_element* pointlist, int nelem);

/** Calculates CRC of point message */
igtl_uint64 igtl_export igtl_point_get_crc(igtl_point_element* pointlist, int nelem);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_POINT_H */


