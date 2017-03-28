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
#include "sha1.h"
#include "igtl_header.h"
#include "igtl_video.h"
#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlVideoMessage.h"
#include "igtlTimeStamp.h"
#include "codec_def.h"
#include "codec_app_def.h"
#include "read_config.h"
#include "wels_const.h"

#include <iostream>
using namespace std;

typedef struct LayerpEncCtx_s {
  int32_t       iDLayerQp;
  SSliceArgument  sSliceArgument;
} SLayerPEncCtx;

class ISVCEncoder;

class H264Encoder
{
public:
  H264Encoder(char * configFile = NULL);
  ~H264Encoder();

  int FillSpecificParameters(SEncParamExt& sParam);

  void SetConfigurationFile(std::string configFile);
  
  /**
   Parse the configuration file to initialize the encoder and server.
   */
  int InitializeEncoder();
  
  /**
   Encode a frame, for performance issue, before encode the frame, make sure the frame pointer is updated with a new frame.
   Otherwize, the old frame will be encoded.
   */
  int EncodeSingleFrameIntoVideoMSG(SSourcePicture* pSrcPic, igtl::VideoMessage* videoMessage, bool isGrayImage = false );
  
  /**
   Get the encoder and server initialization status.
   */
  bool GetInitializationStatus(){return InitializationDone;};
  
  /**
   Get the type of encoded frame
   */
  int GetVideoFrameType(){return encodedFrameType;};
  
  void SetPicWidth(unsigned int width);
  
  void SetPicHeight(unsigned int height);
  
  unsigned int GetPicWidth(){return this->sSvcParam.iPicWidth;};
  
  unsigned int GetPicHeight(){return this->sSvcParam.iPicHeight;};
  
  void SetUseCompression(bool useCompression);
  
  bool GetUseCompression(){return useCompress;};
  
  void SetLosslessLink(bool isLossLessLink);

  bool GetLosslessLink(){return this->sSvcParam.bIsLosslessLink;};
  
private:
  int ParseLayerConfig (string strTag[], const int iLayer, SEncParamExt& pSvcParam);
  
  int ParseConfig();
  
  int encodedFrameType;
  
  ISVCEncoder*  pSVCEncoder;
  
  SEncParamExt sSvcParam;
  
  std::string layerConfigFiles[MAX_DEPENDENCY_LAYER];
  // for configuration file
  
  CReadConfig cRdCfg;
  
  SFrameBSInfo sFbi;
  
  bool  useCompress;
  
  std::string configFile;
    
  bool InitializationDone;
};

#endif
