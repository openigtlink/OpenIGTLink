/*=========================================================================

  Program:   OpenIGTLink
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

/*!
 * \copy
 *     Copyright (c)  2013, Cisco Systems
 *     All rights reserved.
 *
 *     Redistribution and use in source and binary forms, with or without
 *     modification, are permitted provided that the following conditions
 *     are met:
 *
 *        * Redistributions of source code must retain the above copyright
 *          notice, this list of conditions and the following disclaimer.
 *
 *        * Redistributions in binary form must reproduce the above copyright
 *          notice, this list of conditions and the following disclaimer in
 *          the documentation and/or other materials provided with the
 *          distribution.
 *
 *     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *     "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *     LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *     FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *     COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *     INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *     BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *     CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *     LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *     ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *     POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef __igtlH264Encoder_h
#define __igtlH264Encoder_h

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

#include <fstream>
#include <cstring>
#include <stdlib.h>
#include <iostream>

#include "igtlCodecCommonClasses.h"
#include "igtl_header.h"
#include "igtl_video.h"
#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlVideoMessage.h"
#include "igtlTimeStamp.h"
#include "codec_def.h"
#include "codec_app_def.h"

#define MAX_DEPENDENCY_LAYER 4
#define MAX_THREADS_NUM 4
#define EPSN (0.000001f) 

using namespace std;

class ISVCEncoder;

namespace igtl {

typedef struct LayerpEncCtx_s {
  int32_t       iDLayerQp;
  SSliceArgument  sSliceArgument;
} SLayerPEncCtx;

/**
 * @brief Enumerate  video frame type
 * This is H264 related frame types.
 */
typedef enum {
  H264FrameTypeInvalid,    ///< encoder not ready or parameters are invalidate
  H264FrameTypeIDR,        ///< IDR frame in H.264
  H264FrameTypeI,          ///< I frame type
  H264FrameTypeP,          ///< P frame type
  H264FrameTypeSkip,       ///< skip the frame based encoder kernel
  H264FrameTypeIPMixed     ///< a frame where I and P slices are mixing, not supported yet
} H264VideoFrameType;


class IGTLCommon_EXPORT H264Encoder: public GenericEncoder
{
public:
  igtlTypeMacro(H264Encoder, GenericEncoder);
  igtlNewMacro(H264Encoder);

  H264Encoder(char * configFile = NULL);
  ~H264Encoder();
  
  virtual int FillSpecificParameters();
  
  /**
   * @brief Enumerate the type of rate control mode
    typedef enum {
      RC_QUALITY_MODE = 0,     ///< quality mode
      RC_BITRATE_MODE = 1,     ///< bitrate mode
      RC_BUFFERBASED_MODE = 2, ///< no bitrate control,only using buffer status,adjust the video quality
      RC_TIMESTAMP_MODE = 3, //rate control based timestamp
      RC_BITRATE_MODE_POST_SKIP = 4, ///< this is in-building RC MODE, WILL BE DELETED after algorithm tuning!
      RC_OFF_MODE = -1,         ///< rate control off mode
    } RC_MODES;
   */
  virtual int SetRCMode(int value);
  
  virtual int SetKeyFrameDistance(int frameNum){return -1;};
  
  virtual int SetQP(int maxQP, int minQP);
  
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
  
  virtual int SetPicWidthAndHeight(unsigned int Width, unsigned int Height);
  
  virtual unsigned int GetPicWidth(){return this->sSvcParam.iPicWidth;};
  
  virtual unsigned int GetPicHeight(){return this->sSvcParam.iPicHeight;};
  
  virtual int SetLosslessLink(bool linkMethod);
  
  /**
   typedef enum {
   LOW_COMPLEXITY,             ///< the lowest compleixty,the fastest speed,
   MEDIUM_COMPLEXITY,          ///< medium complexity, medium speed,medium quality
   HIGH_COMPLEXITY             ///< high complexity, lowest speed, high quality
   } ECOMPLEXITY_MODE;
   */
  virtual int SetSpeed(int speed);
  
  virtual int SetRCTaregetBitRate(unsigned int bitRate);
  
  virtual bool GetLosslessLink(){return this->sSvcParam.bIsLosslessLink;};
  
private:
  int ParseLayerConfig (string strTag[], const int iLayer, SEncParamExt& pSvcParam);
  
  int ParseConfig();
  
  igtlInt32 InitPic (const void* kpSrc, const int32_t colorspace, const int32_t width, const int32_t height);
  
  ISVCEncoder*  pSVCEncoder;
  
  SEncParamExt sSvcParam;
  
  std::string layerConfigFiles[MAX_DEPENDENCY_LAYER];
  // for configuration file
  
  SFrameBSInfo sFbi;
  
  SSourcePicture h264SrcPicture;
  
};


}//Namespace igtl
#endif
