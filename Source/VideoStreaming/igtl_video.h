/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_VIDEO_H
#define __IGTL_VIDEO_H

#include "igtl_win32header.h"
#include "igtl_util.h"
#include "igtl_types.h"

#define  IGTL_STT_VIDEO_SIZE               9
#define IGTL_VIDEO_CODEC_NAME_SIZE      4
#define IGTL_VIDEO_HEADER_VERSION         1
#define IGTL_VIDEO_HEADER_SIZE          23


/* Data type */
#define IGTL_VIDEO_DTYPE_SCALAR         1
#define IGTL_VIDEO_DTYPE_VECTOR         3

/* Scalar type */
#define IGTL_VIDEO_STYPE_TYPE_INT8      2
#define IGTL_VIDEO_STYPE_TYPE_UINT8     3
#define IGTL_VIDEO_STYPE_TYPE_INT16     4
#define IGTL_VIDEO_STYPE_TYPE_UINT16    5
#define IGTL_VIDEO_STYPE_TYPE_INT32     6
#define IGTL_VIDEO_STYPE_TYPE_UINT32    7

/* Endian */
#define IGTL_VIDEO_ENDIAN_BIG           1
#define IGTL_VIDEO_ENDIAN_LITTLE        2

#define IGTL_VIDEO_CODEC_NAME_H264 "H264"
#define IGTL_VIDEO_CODEC_NAME_VP9 "VP9 "
#define IGTL_VIDEO_CODEC_NAME_X265 "X265"
#define IGTL_VIDEO_CODEC_NAME_OPENHEVC "O265"
#define IGTL_VIDEO_CODEC_NAME_AV1 "AV1"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)     /* For 1-byte boundary in memroy */

typedef struct {
  char           codec[IGTL_VIDEO_CODEC_NAME_SIZE];         /* codec protocal                  */
  igtl_uint32   time_interval;     /* Minimum time between two frames. Use 0 for as fast as possible. */
                               /* If e.g. 50 ms is specified, the maximum update rate will be 20 Hz. */
  igtl_int8 useCompress;
} igtl_stt_video;
  
#pragma pack()

  
#pragma pack(1)     /* For 1-byte boundary in memroy */
  
  /** Image data consists of frame data header, which is defined in this
   *  structure, folowed by array of image pixel data.
   */
  typedef struct {
    igtl_uint16    version;          /* data format version number(1)   */
    char     codec[IGTL_VIDEO_CODEC_NAME_SIZE];         /* codec protocal                  */
    igtl_uint8     scalar_type;      /* scalar type                     */
    /*2:int8 3:uint8 4:int16 5:uint16 6:int32 7:uint32 10:float32 11:float64) */
    igtl_uint8     endian;           /* endian type of image data       */
    /* (1:big, 2:little)               */
    igtl_uint32    width;
    igtl_uint32    height;
    igtl_uint32    additionalZDimension;
    igtl_uint16    frameType;
    igtl_int8      useCompress;
  } igtl_frame_header;
  
#pragma pack()


/** Converts endianness of each element in an array of
 *  or vice versa.*/
void igtl_export igtl_stt_video_convert_byte_order(igtl_stt_video* stt_video);

/** Calculates, STT_VIDEO  messages.*/
igtl_uint64 igtl_export igtl_stt_video_get_crc(igtl_stt_video* stt_video);

void igtl_export igtl_frame_convert_byte_order(igtl_frame_header * header);
  
  
#ifdef __cplusplus
}
#endif

#endif /* __IGTL_VIDEO_H */


