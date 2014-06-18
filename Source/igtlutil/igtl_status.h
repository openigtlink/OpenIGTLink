/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_STATUS_H
#define __IGTL_STATUS_H

#include "igtl_util.h"
#include "igtl_types.h"

#define IGTL_STATUS_HEADER_SIZE         30

#define IGTL_STATUS_ERROR_NAME_LENGTH   20

/* Status codes */

#define IGTL_STATUS_INVALID              0
#define IGTL_STATUS_OK                   1
#define IGTL_STATUS_UNKNOWN_ERROR        2
#define IGTL_STATUS_PANICK_MODE          3 /* emergency */
#define IGTL_STATUS_NOT_FOUND            4 /* file, configuration, device etc */
#define IGTL_STATUS_ACCESS_DENIED        5 
#define IGTL_STATUS_BUSY                 6
#define IGTL_STATUS_TIME_OUT             7 /* Time out / Connection lost */
#define IGTL_STATUS_OVERFLOW             8 /* Overflow / Can't be reached */
#define IGTL_STATUS_CHECKSUM_ERROR       9 /* Checksum error */
#define IGTL_STATUS_CONFIG_ERROR        10 /* Configuration error */
#define IGTL_STATUS_RESOURCE_ERROR      11 /* Not enough resource (memory, storage etc) */
#define IGTL_STATUS_ILLEGAL_INSTRUCTION 12 /* Illegal/Unknown instruction */
#define IGTL_STATUS_NOT_READY           13 /* Device not ready (starting up)*/
#define IGTL_STATUS_MANUAL_MODE         14 /* Manual mode (device does not accept commands) */
#define IGTL_STATUS_DISABLED            15 /* Device disabled */
#define IGTL_STATUS_NOT_PRESENT         16 /* Device not present */
#define IGTL_STATUS_UNKNOWN_VERSION     17 /* Device version not known */
#define IGTL_STATUS_HARDWARE_FAILURE    18 /* Hardware failure */
#define IGTL_STATUS_SHUT_DOWN           19 /* Exiting / shut down in progress */


#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

/** Status data header for OpenIGTLinik protocol */
typedef struct {
  igtl_uint16    code;             /* status code defined above */
  igtl_int64     subcode;          /* sub code for the error    */
  char           error_name[IGTL_STATUS_ERROR_NAME_LENGTH]; 
                                   /* error name -- can be anything, don't rely on this */
  /*char           status_message[]; */
} igtl_status_header;

#pragma pack()

/** Converts endianness of each member variable
 *  in igtl_status_header from host byte order to network byte order,
 *  or vice versa. */
void igtl_export igtl_status_convert_byte_order(igtl_status_header* status);

/** Calculates CRC of status data body including status message part */
igtl_uint64 igtl_export igtl_status_get_crc(igtl_status_header* status, igtl_uint32 msglen, const char* msg);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_IMAGE_H */


