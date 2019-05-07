/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_HEADER_H
#define __IGTL_HEADER_H

#define IGTL_HEADER_VERSION_1   1
#define IGTL_HEADER_VERSION_2   2
#define IGTL_HEADER_SIZE      58
#define IGTL_EXTENDED_HEADER_SIZE      12

#define IGTL_HEADER_TYPE_SIZE  12
#define IGTL_HEADER_NAME_SIZE  20

/* Following macros will be obsolete. Included for old programs*/
#define IGTL_HEADER_TYPESIZE  IGTL_HEADER_TYPE_SIZE
#define IGTL_HEADER_NAMESIZE  IGTL_HEADER_NAME_SIZE
#define IGTL_HEADER_DEVSIZE   IGTL_HEADER_NAME_SIZE

#define IGTL_DEVICE_NAME_METADATA_KEY "IGTL_DEVICE_NAME"

/* Device name prefix macro */
#define IGTL_TYPE_PREFIX_NONE     0
#define IGTL_TYPE_PREFIX_GET      1
#define IGTL_TYPE_PREFIX_STT      2
#define IGTL_TYPE_PREFIX_STP      3
#define IGTL_TYPE_PREFIX_RTS      4
#define IGTL_NUM_TYPE_PREFIX      5

#include "igtl_types.h"
#include "igtl_win32header.h"
#include "igtlConfigure.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(2)     /* For 2-byte boundary in memory */

/** Message header of OpenIGTLink message.
 *  igtl_header is an overall data header for OpenIGTLink protocol.
 *  It is transfered at beginning of every OpenIGTLink message to give
 *  type and size of following data body to a receiver.
 *  These parameters allow the receiver to parse or skip the data body. */
typedef struct {
  igtl_uint16    header_version;          /* header version number */
  char           name[IGTL_HEADER_TYPE_SIZE];       /* data type name          */
  char           device_name[IGTL_HEADER_NAME_SIZE]; /* device name             */
  igtl_uint64    timestamp;        /* time stamp message      */
  igtl_uint64    body_size;        /* size of the body        */
  igtl_uint64    crc;              /* CRC                     */
} igtl_header;
  
#if OpenIGTLink_HEADER_VERSION >= 2
/** Message extended header of OpenIGTLink message.
 *  igtl_extended_header is an overall data extended header for OpenIGTLink protocol.
 *  It is transfered with the data body to define the
 *  size of extended header and the meta data size */
typedef struct {
  igtl_uint16    extended_header_size;          /* size of extended header */
  igtl_uint16    meta_data_header_size;         /* size of the meta data header*/
  igtl_uint32    meta_data_size;                /* size of meta data */
  igtl_uint32    message_id;                    /* message id */
} igtl_extended_header;

typedef struct {
  igtl_uint16   key_size;
  igtl_uint16   value_encoding;
  igtl_uint32   value_size;
} igtl_metadata_header_entry;
#endif

#pragma pack()

/** igtl_header_convert_byte_order() convers endianness of each
 *  member variable in igtl_header structure from host byte order
 *  to network byte order, or vice versa. */
void igtl_export igtl_header_convert_byte_order(igtl_header * header);

#if OpenIGTLink_HEADER_VERSION >= 2
/** igtl_extended_header_convert_byte_order() convers endianness of each
 *  member variable in igtl_header structure from host byte order
 *  to network byte order, or vice versa. */
void igtl_export igtl_extended_header_convert_byte_order(igtl_extended_header * extended_header);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_HEADER_H */

