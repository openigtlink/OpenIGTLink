/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_VIDEOMETA_H
#define __IGTL_VIDEOMETA_H

#include "igtl_win32header.h"
#include "igtl_util.h"
#include "igtl_types.h"
#include "igtl_video.h"

#define IGTL_VIDEOMETA_ELEMENT_SIZE    322

#define IGTL_VIDEOMETA_LEN_NAME         64
#define IGTL_VIDEOMETA_LEN_DEVICE_NAME  64
#define IGTL_VIDEOMETA_LEN_PATIENT_NAME 64
#define IGTL_VIDEOMETA_LEN_PATIENT_ID   64

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

/** VIDEOMETA is a message type to transfer a list of video source available in a server.
 *  A VIDEOMETA message may contain information of multiple video source.
 *  The client determins the number of video source meta by the size of the body included
 *  in the message header (see igtl_videometa_getdata_n() macro). */
typedef struct {
  char           name[IGTL_VIDEOMETA_LEN_NAME];                 /* name / description */
  char           device_name[IGTL_VIDEOMETA_LEN_DEVICE_NAME];   /* device name to query the video and COLORT */
  char           patient_name[IGTL_VIDEOMETA_LEN_PATIENT_NAME]; /* patient name */ 
  char           patient_id[IGTL_VIDEOMETA_LEN_PATIENT_ID];     /* patient ID (MRN etc.) */
  
  igtl_int16     zoom_level;      /* camera zoom level. */
  
  igtl_float64   focal_length;      /* camera focal length. */
  
  igtl_uint16    size[3];          /* entire image volume size */
  
  igtl_float32   matrix[12];       /* orientation / origin of image   */
                                   /*  - matrix[0-2]: norm_i * pix_i  */
                                   /*  - matrix[3-5]: norm_j * pix_j  */
                                   /*  - matrix[6-8]: norm_k * pix_k  */
                                   /*  - matrix[9-11]:origin          */
                                   /* where norm_* are normal vectors */
                                   /* along each index, and pix_* are */
                                   /* pixel size in each direction    */

  igtl_uint8     scalar_type;      /* scalar type. see scalar_type in IMAGE message */
  igtl_uint8     reserved;
} igtl_videometa_element;

#pragma pack()

/** igtl_videometa_get_data_size(n) macro calculates the size of body based on the number
 *  of images.The size of body is used in the message header.
 *  igtl_videometa_get_data_n(size) calculates the number of images in the body, based on
 *  the body size. This function may be used when a client program parses VIDEOMETA message. */
#define igtl_videometa_get_data_size(n)  ((n) * IGTL_VIDEOMETA_ELEMENT_SIZE)
#define igtl_videometa_get_data_n(size)  ((size) / IGTL_VIDEOMETA_ELEMENT_SIZE)

  
/** Generates image orientation/origin matrix from spacing, origin and normal vectors. */
void igtl_export igtl_videometa_set_matrix(float spacing[3], float origin[3],
                                       float norm_i[3], float norm_j[3], float norm_k[3],
                                       igtl_videometa_element * element);

void igtl_export igtl_videometa_get_matrix(float spacing[3], float origin[3],
                                       float norm_i[3], float norm_j[3], float norm_k[3],
                                       igtl_videometa_element * element);

/** Sets the image orientation/origin matrix in 4x4 format */
void igtl_export igtl_videometa_set_matrix_4x4(float _matrix[4][4], igtl_videometa_element * element);

/** Gets the image orientation/origin matrix in 4x4 format */
void igtl_export igtl_videometa_get_matrix_4x4(float _matrix[4][4],igtl_videometa_element * element);

/** Converts endianness of each member variable in igtl_videometa_element from host
 *  byte order to network byte order, or vice versa. */
void igtl_export igtl_videometa_convert_byte_order(igtl_videometa_element* metalist, int nitem);

/** Clculates CRC of video meta data body. */
igtl_uint64 igtl_export igtl_videometa_get_crc(igtl_videometa_element* metalist, int nitem);

#ifdef __cplusplus
}
#endif

#endif /* __IGTL_VIDEOMETA_H */
