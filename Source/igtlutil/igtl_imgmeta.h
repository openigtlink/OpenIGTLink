/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_IMGMETA_H
#define __IGTL_IMGMETA_H

#include "igtl_win32header.h"
#include "igtl_util.h"
#include "igtl_types.h"
#include "igtl_image.h"

#define IGTL_IMGMETA_ELEMENT_SIZE          260

/* Scalar type */
/* Use the same numbers as IMAGE */
/*
#define IGTL_IMGMETA_STYPE_TYPE_INT8      2
#define IGTL_IMGMETA_STYPE_TYPE_UINT8     3
#define IGTL_IMGMETA_STYPE_TYPE_INT16     4
#define IGTL_IMGMETA_STYPE_TYPE_UINT16    5
#define IGTL_IMGMETA_STYPE_TYPE_INT32     6
#define IGTL_IMGMETA_STYPE_TYPE_UINT32    7
#define IGTL_IMGMETA_STYPE_TYPE_FLOAT32   10
#define IGTL_IMGMETA_STYPE_TYPE_FLOAT64   11
*/

#define IGTL_IMGMETA_LEN_NAME         64
#define IGTL_IMGMETA_LEN_DEVICE_NAME  20
#define IGTL_IMGMETA_LEN_MODALITY     32
#define IGTL_IMGMETA_LEN_PATIENT_NAME 64
#define IGTL_IMGMETA_LEN_PATIENT_ID   64


#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

/** IMAGEMETA is a message type to transfer a list of images available in a server.
 *  A IMGMETA message may contain information of multiple images.
 *  The client determins the number of image meta by the size of the body included
 *  in the message header (see igtl_imgmeta_getdata_n() macro). */
typedef struct {
  char           name[IGTL_IMGMETA_LEN_NAME];                 /* name / description */
  char           device_name[IGTL_IMGMETA_LEN_DEVICE_NAME];   /* device name to query the IMAGE and COLORT */
  char           modality[IGTL_IMGMETA_LEN_MODALITY];         /* modality name */
  char           patient_name[IGTL_IMGMETA_LEN_PATIENT_NAME]; /* patient name */ 
  char           patient_id[IGTL_IMGMETA_LEN_PATIENT_ID];     /* patient ID (MRN etc.) */  
  igtl_uint64    timestamp;        /* scan time */
  igtl_uint16    size[3];          /* entire image volume size */ 
  igtl_uint8     scalar_type;      /* scalar type. see scalar_type in IMAGE message */
  igtl_uint8     reserved;
} igtl_imgmeta_element;

#pragma pack()

/** igtl_imgmeta_get_data_size(n) macro calculates the size of body based on the number
 *  of images.The size of body is used in the message header.
 *  igtl_imgmeta_get_data_n(size) calculates the number of images in the body, based on
 *  the body size. This function may be used when a client program parses IMGMETA message. */
#define igtl_imgmeta_get_data_size(n)  ((n) * IGTL_IMGMETA_ELEMENT_SIZE)
#define igtl_imgmeta_get_data_n(size)  ((size) / IGTL_IMGMETA_ELEMENT_SIZE)

/** Converts endianness of each member variable in igtl_imgmeta_element from host
 *  byte order to network byte order, or vice versa. */
void igtl_export igtl_imgmeta_convert_byte_order(igtl_imgmeta_element* metalist, int nitem);

/** Clculates CRC of image meta data body. */
igtl_uint64 igtl_export igtl_imgmeta_get_crc(igtl_imgmeta_element* metalist, int nitem);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_IMGMETA_H */
