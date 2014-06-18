/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_QTDATA_H
#define __IGTL_QTDATA_H

#include "igtl_win32header.h"
#include "igtl_util.h"
#include "igtl_types.h"

#define  IGTL_QTDATA_ELEMENT_SIZE           50
#define  IGTL_STT_QTDATA_SIZE               36
#define  IGTL_RTS_QTDATA_SIZE               1

#define  IGTL_QTDATA_LEN_NAME               20  /* Maximum length of tracking instrument name */
#define  IGTL_STT_QTDATA_LEN_COORDNAME      32  /* Maximum length of coordinate system name */

#define  IGTL_QTDATA_TYPE_TRACKER           1  /* Tracker */
#define  IGTL_QTDATA_TYPE_6D                2  /* 6D instrument (regular instrument) */
#define  IGTL_QTDATA_TYPE_3D                3  /* 3D instrument (only tip of the instrument defined) */
#define  IGTL_QTDATA_TYPE_5D                4  /* 5D instrument (tip and handle are defined,
                                                  but not the normal vector) */


#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

/** Status data header for OpenIGTLinik protocol */
typedef struct {
  char         name[IGTL_QTDATA_LEN_NAME];  /* Name of instrument / tracker */
  igtl_uint8   type;           /* Tracking data type (1-4) */
  igtl_uint8   reserved;       /* Reserved byte */
  igtl_float32 position[3];    /* position (x, y, z) */
  igtl_float32 quaternion[4];  /* orientation as quaternion (qx, qy, qz, w) */
} igtl_qtdata_element;


typedef struct {
  igtl_int32   resolution;     /* Minimum time between two frames. Use 0 for as fast as possible. */
                               /* If e.g. 50 ms is specified, the maximum update rate will be 20 Hz. */
  char         coord_name[IGTL_STT_QTDATA_LEN_COORDNAME]; /* Name of the coordinate system */
} igtl_stt_qtdata;

typedef struct {
  igtl_int8    status;         /* 0: Success 1: Error */
} igtl_rts_qtdata;

#pragma pack()

/** igtl_qtdata_get_data_size(n) calculates the size of body based on the number
 *  of qtdatas. The size of body is used in the message header.*/
#define igtl_qtdata_get_data_size(n)  ((n) * IGTL_QTDATA_ELEMENT_SIZE)

/** igtl_qtdata_get_data_n(size) calculates the number of qtdatas in the body, based on
 *  the body size. This function may be used when a client program parses a QTDATA message. */
#define igtl_qtdata_get_data_n(size)  ((size) / IGTL_QTDATA_ELEMENT_SIZE)

/** Byte order conversion for an array of QTDATA, STT_QTDATA and RTS_QTDATA data structure
 *  Converts endianness of each element in an array of igtl_qtdata_element from host byte
 *  order to network byte order, or vice versa. */
void igtl_export igtl_qtdata_convert_byte_order(igtl_qtdata_element* qtdatalist, int nelem);
void igtl_export igtl_stt_qtdata_convert_byte_order(igtl_stt_qtdata* stt_qtdata);
void igtl_export igtl_rts_qtdata_convert_byte_order(igtl_rts_qtdata* rts_qtdata);

/** Calculates CRC of QTDATA, STT_QTDATA and RTS_QTDATA messages */
igtl_uint64 igtl_export igtl_qtdata_get_crc(igtl_qtdata_element* qtdatalist, int nelem);
igtl_uint64 igtl_export igtl_stt_qtdata_get_crc(igtl_stt_qtdata* stt_qtdata);
igtl_uint64 igtl_export igtl_rts_qtdata_get_crc(igtl_rts_qtdata* rts_qtdata);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_QTDATA_H */


