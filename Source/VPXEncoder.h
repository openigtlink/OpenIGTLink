/*=========================================================================

  Program:   OpenIGTLink
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igtlVPXEncoder_h
#define __igtlVPXEncoder_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "OpenVPX/vpx/vpx_encoder.h"
#include "OpenVPX/vpx/vp8cx.h"
#include "OpenVPX/vp9/common/vp9_common.h"
#include "OpenVPX/tools_common.h"
#include "OpenVPX/video_writer.h"

#include "igtlCodecCommonClasses.h"
#include "igtl_header.h"
#include "igtl_video.h"
#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlVideoMessage.h"
#include "igtlTimeStamp.h"


using namespace std;

class VPXEncoder: public GenericEncoder
{
public:
  VPXEncoder(char * configFile = NULL);
  ~VPXEncoder();

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
  
  virtual void SetPicWidth(unsigned int width);
  
  virtual void SetPicHeight(unsigned int height);
  
  virtual unsigned int GetPicWidth(){return this->picWidth;};
  
  virtual unsigned int GetPicHeight(){return this->picHeight;};
  
  virtual int SetRCMode(int value);
  
  virtual int SetQP(int maxQP, int minQP);
  
  virtual int SetRCTaregetBitRate(unsigned int bitRate);
  
  virtual int SetLosslessLink(bool linkMethod);
  
  /*!\brief deadline parameter analogous to VPx REALTIME mode. */
  //#define VPX_DL_REALTIME (1)
  /*!\brief deadline parameter analogous to  VPx GOOD QUALITY mode. */
  //#define VPX_DL_GOOD_QUALITY (1000000)
  /*!\brief deadline parameter analogous to VPx BEST QUALITY mode. */
  //#define VPX_DL_BEST_QUALITY (0)
  /*!\brief Encode a frame*/
  int SetDeadlineMode(unsigned long mode);
  
  int ParseConfig();
  
  const VpxInterface *encoder;
  
  vpx_codec_enc_cfg_t cfg;
  
  vpx_codec_ctx_t* codec;
  
  vpx_image_t* inputImage;
  
  vpx_codec_iter_t iter;
  
  const vpx_fixed_buf_t *encodedBuf;
  
  const vpx_codec_cx_pkt_t *pkt;
  
  unsigned long deadlineMode;
  
};

#endif
