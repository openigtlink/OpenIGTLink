/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_TRAJECTORY_H
#define __IGTL_TRAJECTORY_H

#include "igtl_win32header.h"
#include "igtl_util.h"
#include "igtl_types.h"

#define IGTL_TRAJECTORY_ELEMENT_SIZE           150
#define IGTL_TRAJECTORY_LEN_NAME                64
#define IGTL_TRAJECTORY_LEN_GROUP_NAME          32
#define IGTL_TRAJECTORY_LEN_OWNER               20

#define IGTL_TRAJECTORY_TYPE_ENTRY_ONLY          1
#define IGTL_TRAJECTORY_TYPE_TARGET_ONLY         2
#define IGTL_TRAJECTORY_TYPE_ENTRY_TARGET        3

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

/** Status data header for OpenIGTLinik protocol */
typedef struct {
  char         name[IGTL_TRAJECTORY_LEN_NAME];          /* Name or description of the trajectory */
  char         group_name[IGTL_TRAJECTORY_LEN_GROUP_NAME];    /* Can be "Trajectory",  ... */
  igtl_int8    type;              /* Trajectory type (see IGTL_TRAJECTORY_TYPE_* macros) */
  igtl_int8    reserved;
  igtl_uint8   rgba[4];           /* Color in R/G/B/A */
  igtl_float32 entry_pos[3];      /* Coordinate of the entry point */
  igtl_float32 target_pos[3];     /* Coordinate of the target point */
  igtl_float32 radius;            /* Radius of the trajectory. Can be 0. */
  char         owner_name[IGTL_TRAJECTORY_LEN_OWNER];    /* Device name of the ower image */
} igtl_trajectory_element;

#pragma pack()


/** igtl_trajectory_get_data_size(n) calculates the size of body based on the number
 *  of trajectorys. The size of body is used in the message header. */
#define igtl_trajectory_get_data_size(n)  ((n) * IGTL_TRAJECTORY_ELEMENT_SIZE)

/** igtl_trajectory_get_data_n(size) calculates the number of images in the body, based on
 *  the body size. This function may be used when a client program parses a TRAJECTORY message. */
#define igtl_trajectory_get_data_n(size)  ((size) / IGTL_TRAJECTORY_ELEMENT_SIZE)

/** Converts endianness of each element in an array of
 *  igtl_igtl_trajectory_element from host byte order to network byte order,
 *  or vice versa. */
void igtl_export igtl_trajectory_convert_byte_order(igtl_trajectory_element* trajectorylist, int nelem);

/** Calculates CRC of trajectory message */
igtl_uint64 igtl_export igtl_trajectory_get_crc(igtl_trajectory_element* trajectorylist, int nelem);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_POSITION_H */


