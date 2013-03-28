/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_LBMETA_H
#define __IGTL_LBMETA_H

#include "igtl_win32header.h"
#include "igtl_util.h"
#include "igtl_types.h"

#define IGTL_LBMETA_ELEMENT_SIZE          116

#define IGTL_LBMETA_LEN_NAME              64
#define IGTL_LBMETA_LEN_DEVICE_NAME       20
#define IGTL_LBMETA_LEN_OWNER             20

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Label meta data in OpenIGTLinik protocol
 *
 * LBMETA is a data type to send a list of label image data available in a server.
 * In OpenIGTLink protocol, actual label data is transfered as an IMAGE message.
 * LBMETA message lets the receiving client know which IMAGE data are available
 * as label maps. 
 *   Multiple labels in a LBMETA message can point the same IMAGE message, if there
 * are multiple lables are recoreded in a single IMAGE mesage. For example, given
 * the following label map as IMAGE data with name "LableImage1":
 * 
 *    0 0 0 1 0 0         value  | name / description
 *    2 0 1 1 1 0        --------+---------------------------- 
 *    2 2 1 1 1 0             0  | None
 *    2 2 2 1 0 0             1  | Liver
 *    2 2 2 2 0 0             2  | Kidney
 *
 * To send the label information recorded in "LabelImage1," one can create
 * a following LBMETA message:
 *
 *      DATA          |  Contents 
 *     ---------------+---------------------------
 *      NAME 1        | Liver
 *      IMAGE NAME 1  | LabelImage
 *      Labe l        | 1
 *      RGBA 1        | 0xFF0000
 *      SIZE 1        | (6, 5, 1)
 *      OWNER 1       | 
 *      --------------+---------------------------
 *      NAME 2        | Kidney
 *      IMAGE NAME 2  | LabelImage
 *      Labe 2        | 2
 *      RGBA 2        | 0xFF2222
 *      SIZE 2        | (6, 5, 1)
 *      OWNER 2       | 
 *      --------------+---------------------------
 *
 *         ...           ....
 *
 *      --------------+---------------------------
 *
 * The client determins the number of image meta by the size of the body included
 * in the message header (see igtl_lbmeta_getdata_n() macro).
 */

#pragma pack(1)     /* For 1-byte boundary in memroy */

typedef struct {
  char           name[IGTL_LBMETA_LEN_NAME];  /* name / description */
  char           device_name[IGTL_LBMETA_LEN_DEVICE_NAME];   /* device name to query the IMAGE */
  igtl_uint8     label;                       /* label */
  igtl_uint8     reserved;
  igtl_uint8     rgba[4];                     /* Color in RGBA. default: (0, 0, 0, 0) */
  igtl_uint16    size[3];                     /* Number of pixels in each direction */
  char           owner[IGTL_LBMETA_LEN_OWNER];/* Device name of the owner image. (can be empty) */
} igtl_lbmeta_element;

#pragma pack()


/** igtl_lbmeta_get_data_size(n) calculates the size of body based on the number
 *  of images.The size of body is used in the message header.*/
#define igtl_lbmeta_get_data_size(n)  ((n) * IGTL_LBMETA_ELEMENT_SIZE)

/** igtl_lbmeta_get_data_n(size) calculates the number of images in the body, based on
 *  the body size. This function may be used when a client program parses LBMETA message. */
#define igtl_lbmeta_get_data_n(size)  ((size) / IGTL_LBMETA_ELEMENT_SIZE)

/** Converts endianness of each member variable in igtl_lbmeta_element
 *  from host byte order to network byte order, or vice versa. */
void igtl_export igtl_lbmeta_convert_byte_order(igtl_lbmeta_element* metalist, int nitem);

/** Calculates CRC of image meta data body. */
igtl_uint64 igtl_export igtl_lbmeta_get_crc(igtl_lbmeta_element* metalist, int nitem);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_LBMETA_H */


