/*=========================================================================
 
 Program:   VPXEncoder
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#ifndef __igtlVP9Decoder_h
#define __igtlVP9Decoder_h

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
#include "vpx_decoder.h"
#include "vp8dx.h"
#include "vpx_codec.h"
#include "vpx_image.h"
#include "vpx_integer.h"

namespace igtl {


#ifdef __cplusplus
extern "C" {
#endif
  typedef struct VpxInterfaceDecoder {
    vpx_codec_iface_t *(*const codec_interface)();
  } VpxInterfaceDecoder;
  
#ifdef __cplusplus
} /* extern "C" */
#endif

#define NO_DELAY_DECODING
class IGTLCommon_EXPORT VP9Decoder:public GenericDecoder
{
public:
  igtlTypeMacro(VP9Decoder, GenericDecoder);
  igtlNewMacro(VP9Decoder);

  VP9Decoder();
  ~VP9Decoder();
  
  int DecodeBitStreamIntoFrame(unsigned char* bitStream,igtl_uint8* outputFrame, igtl_uint32 iDimensions[2], igtl_uint64 &iStreamSize) override;
  
  int DecodeVideoMSGIntoSingleFrame(igtl::VideoMessage* videoMessage, SourcePicture* pDecodedPic) override;
  
private:
  using GenericDecoder::ComposeByteSteam; // disable the clang warning about virtual function overload.
  virtual void ComposeByteSteam(igtl_uint8** inputData, int dimension[2], int iStride[2], igtl_uint8 *outputFrame, VideoFormatType format);
  
  const VpxInterfaceDecoder* decoder;
  
  int vpx_img_plane_width(const vpx_image_t *img, int plane);
  
  int vpx_img_plane_height(const vpx_image_t *img, int plane);
  
  vpx_codec_ctx_t codec;
  
  vpx_image_t* outputImage;
  
  vpx_codec_iter_t iter;
  
};

} // namespace igtl

#endif
