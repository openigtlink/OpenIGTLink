/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <string.h>
#include <math.h>

#include "igtl_video.h"
#include "igtl_util.h"

void igtl_export igtl_stt_video_convert_byte_order(igtl_stt_video* stt_video)
{
  igtl_int32* tmp;

  if (igtl_is_little_endian()) 
    {
     tmp = (igtl_int32*)&(stt_video->time_interval);
     *tmp = BYTE_SWAP_INT32(*tmp);
    }
}


igtl_uint64 igtl_export igtl_stt_video_get_crc(igtl_stt_video* stt_video)
{
  igtl_uint64  crc;

  crc = crc64(0, 0, 0);
  crc = crc64((unsigned char*) stt_video, IGTL_STT_VIDEO_SIZE, crc);
  return crc;
}



void igtl_export igtl_frame_convert_byte_order(igtl_frame_header * header)
{
  if (igtl_is_little_endian())
  {
    header->version = BYTE_SWAP_INT16(header->version);
    header->width = BYTE_SWAP_INT32(header->width);
    header->height = BYTE_SWAP_INT32(header->height);
    header->frameType = BYTE_SWAP_INT16(header->frameType);
  }
}
