/*=========================================================================
 
 Program:   VPXEncoder
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#ifndef __igtlVPXDecoder_h
#define __igtlVPXDecoder_h

#include <time.h>
#if defined(_WIN32) /*&& defined(_DEBUG)*/
  #include <windows.h>
  #include <stdio.h>
  #include <stdarg.h>
  #include <sys/types.h>
#else
  #include <sys/time.h>
#endif

#include <vector>
#include <limits.h>
#include <string.h>
#include "igtl_types.h"
#include "igtlVideoMessage.h"
#include "igtlCodecCommonClasses.h"
#include "OpenVPX/vpx/vpx_decoder.h"
#include "OpenVPX/tools_common.h"
#include "OpenVPX/video_reader.h"
#include "OpenVPX/vpx_config.h"

#define NO_DELAY_DECODING
class VPXDecoder:public GenericDecoder
{
public: 
  VPXDecoder();
  ~VPXDecoder();
  
  virtual int DecodeBitStreamIntoFrame(unsigned char* bitStream,igtl_uint8* outputFrame, igtl_uint32 iDimensions[2], igtl_uint64 &iStreamSize);
  
  virtual int DecodeVideoMSGIntoSingleFrame(igtl::VideoMessage* videoMessage, SourcePicture* pDecodedPic);
  
private:
  virtual void ComposeByteSteam(igtl_uint8** inputData, int dimension[2], int iStride[2], igtl_uint8 *outputFrame);
  
  const VpxInterface* decoder;
  
  vpx_codec_ctx_t codec;
  
  vpx_image_t* outputImage;
  
  vpx_codec_iter_t iter;
  
};

#endif