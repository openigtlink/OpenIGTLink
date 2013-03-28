/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_SENSOR_H
#define __IGTL_SENSOR_H

#include "igtl_win32header.h"
#include "igtl_util.h"
#include "igtl_types.h"
#include "igtl_unit.h"
#include "igtl_win32header.h"

#define IGTL_SENSOR_HEADER_SIZE          10

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

typedef struct {
  igtl_uint8     larray;           /* Length of array (0-255) */
  igtl_uint8     status;           /* (reserved) sensor status */
  igtl_unit      unit;             /* Unit */
} igtl_sensor_header;

#pragma pack()

/** This function calculates size of the pixel array, which will be
 *  transferred with the specified header. */
igtl_uint32 igtl_export igtl_sensor_get_data_size(igtl_sensor_header * header);

/** This function converts endianness from host byte order to network byte order,
 *  or vice versa. */
void igtl_export igtl_sensor_convert_byte_order(igtl_sensor_header * header, igtl_float64* data);

/** Calculates CRC of image data body including header
 *  and array of pixel data. */
igtl_uint64 igtl_export igtl_sensor_get_crc(igtl_sensor_header * header, igtl_float64* data);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_SENSOR_H */
