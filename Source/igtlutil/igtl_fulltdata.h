/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_FULLTDATA_H
#define __IGTL_FULLTDATA_H

#include "igtl_tdata.h"

#define  IGTL_FULLTDATA_ELEMENT_SIZE       74

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

/** Status data header for OpenIGTLinik protocol */
typedef struct {
  char         name[IGTL_TDATA_LEN_NAME];  /* Name of instrument / tracker */
  igtl_uint8   type;           /* Tracking data type (1-4) */
  igtl_uint8   reserved;       /* Reserved byte */
  igtl_float32 transform[12];  /* same as TRANSFORM */
  igtl_float32 error;          /* the tracking error */
} igtl_fulltdata_element;


#pragma pack()

/** igtl_tdata_get_data_size(n) calculates the size of body based on the number
 *  of tdatas. The size of body is used in the message header.*/
#define igtl_fulltdata_get_data_size(n)  ((n) * IGTL_FULLTDATA_ELEMENT_SIZE)

/** igtl_tdata_get_data_n(size) calculates the number of tdatas in the body, based on
 * the body size. This function may be used when a client program parses a TDATA message. */
#define igtl_fulltdata_get_data_n(size)  ((size) / IGTL_FULLTDATA_ELEMENT_SIZE)

/** Converts endianness of each element in an array of
 *  igtl_tdata_element from host byte order to network byte order,
 *  or vice versa.*/
void igtl_export igtl_fulltdata_convert_byte_order(igtl_fulltdata_element* tdatalist, int nelem);

/** Calculates CRC of TDATA, STT_TDATA and RTS_TDATA messages.*/
igtl_uint64 igtl_export igtl_fulltdata_get_crc(igtl_fulltdata_element* tdatalist, int nelem);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_FULLTDATA_H */


