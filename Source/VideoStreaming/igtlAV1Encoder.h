/*=========================================================================

  Program:   OpenIGTLink
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlAV1Encoder_h
#define __igtlAV1Encoder_h

// OpenIGTLink includes
#include "igtlCodecCommonClasses.h"

// AV1 includes
#include "aom_encoder.h"

namespace igtl {

#ifdef __cplusplus
extern "C" {
#endif
  typedef struct AomInterfaceEncoder {
    aom_codec_iface_t *(*const codec_interface)();
  } AomInterfaceEncoder;
  
#ifdef __cplusplus
} /* extern "C" */
#endif

class IGTLCommon_EXPORT igtlAV1Encoder : public GenericEncoder
{
public:
  igtlTypeMacro(igtlAV1Encoder, GenericEncoder);
  igtlNewMacro(igtlAV1Encoder);

  igtlAV1Encoder(char * configFile = NULL);
  ~igtlAV1Encoder();

  virtual int FillSpecificParameters();
  
  /**
   Parse the configuration file to initialize the encoder and server.
   */
  virtual int InitializeEncoder();
  
  virtual int ConvertToLocalImageFormat(SourcePicture* pSrcPic);
  
  /**
   Encode a frame, for performance issue, before encode the frame, make sure the frame pointer is updated with a new frame.
   Otherwize, the old frame will be encoded.
   */
  virtual int EncodeSingleFrameIntoVideoMSG(SourcePicture* pSrcPic, igtl::VideoMessage* videoMessage, bool isGrayImage = false );
  
  virtual int SetPicWidthAndHeight(unsigned int width, unsigned int height);
  
  virtual unsigned int GetPicWidth(){return this->picWidth;};
  
  virtual unsigned int GetPicHeight(){return this->picHeight;};
  
  virtual int SetRCMode(int value);
  
  virtual int SetKeyFrameDistance(int frameNum);
  
  virtual int SetQP(int maxQP, int minQP);
  
  virtual int SetRCTargetBitRate(unsigned int bitRate);
  
  virtual int SetLosslessLink(bool linkMethod);
  
  /* Speed value Should be set between -8 to 8 for AV1.
     Values greater than 0 will increase encoder speed at the expense of quality.
     negative value is not used.*/
  enum
  {
    SlowestSpeed = 0,
    FastestSpeed = 8
  };
  virtual int SetSpeed(int speed);
  
  /*!\brief deadline parameter analogous to VPx REALTIME mode. */
  //#define VPX_DL_REALTIME (1)
  /*!\brief deadline parameter analogous to  VPx GOOD QUALITY mode. */
  //#define VPX_DL_GOOD_QUALITY (1000000)
  /*!\brief deadline parameter analogous to VPx BEST QUALITY mode. */
  //#define VPX_DL_BEST_QUALITY (0)
  /*!\brief Encode a frame*/
  int SetDeadlineMode(unsigned long mode);
  
private:
  
  const AomInterfaceEncoder *encoder;
  
  void error_output(aom_codec_ctx_t *ctx, const char *s);
  
  int aom_img_plane_width(const aom_image_t *img, int plane);
  
  int aom_img_plane_height(const aom_image_t *img, int plane);
  
  aom_codec_enc_cfg_t cfg;
  
  aom_codec_ctx_t* codec;
  
  aom_image_t* inputImage;
  
  aom_codec_iter_t iter;
  
  const aom_fixed_buf_t *encodedBuf;
  
  const aom_codec_cx_pkt_t *pkt;
  
  unsigned long deadlineMode;
  
};


} // namespace igtl
#endif
