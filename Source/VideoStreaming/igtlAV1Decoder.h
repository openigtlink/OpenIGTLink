/*=========================================================================
 
 Program:   AOMDecoder
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#ifndef __igtlAV1Decoder_h
#define __igtlAV1Decoder_h

// OpenIGTLink includes
#include "igtlCodecCommonClasses.h"

// AV1 includes
#include "aom_decoder.h"

namespace igtl {

#ifdef __cplusplus
extern "C" {
#endif
  typedef struct AomInterfaceDecoder {
    aom_codec_iface_t *(*const codec_interface)();
  } AomInterfaceDecoder;
  
#ifdef __cplusplus
} /* extern "C" */
#endif

#define NO_DELAY_DECODING
class IGTLCommon_EXPORT igtlAV1Decoder : public GenericDecoder
{
public:
  igtlTypeMacro(igtlAV1Decoder, GenericDecoder);
  igtlNewMacro(igtlAV1Decoder);

  igtlAV1Decoder();
  ~igtlAV1Decoder();
  
  int DecodeBitStreamIntoFrame(unsigned char* bitStream,igtl_uint8* outputFrame, igtl_uint32 iDimensions[2], igtl_uint64 &iStreamSize) override;
  
  int DecodeVideoMSGIntoSingleFrame(igtl::VideoMessage* videoMessage, SourcePicture* pDecodedPic) override;
  
private:
  void ComposeByteSteam(igtl_uint8** inputData, int dimension[2], int iStride[2], igtl_uint8 *outputFrame) override;
  
  const AomInterfaceDecoder* decoder;
  
  int aom_img_plane_width(const aom_image_t *img, int plane);
  
  int aom_img_plane_height(const aom_image_t *img, int plane);
  
  aom_codec_ctx_t codec;
  
  aom_image_t* outputImage;
  
  aom_codec_iter_t iter;
  
};


}// Namepace igtl
#endif