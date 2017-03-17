/*=========================================================================

  Program:   OpenIGTLink
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

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

typedef struct tagFilesSet {
  string strBsFile;
  string strSeqFile;    // for cmd lines
  string strLayerCfgFile[MAX_DEPENDENCY_LAYER];
  uint32_t uiFrameToBeCoded;
} SFilesSet;


class ISVCEncoder;

class H264Encoder
{
public:
  H264Encoder(char * configFile);
  ~H264Encoder();
  
  void SetConfigurationFile(std::string configFile);
  
  /**
   Parse the configuration file to initialize the encoder and server.
   */
  bool InitializeEncoder();
  
  /**
   Encode a frame, for performance issue, before encode the frame, make sure the frame pointer is updated with a new frame.
   Otherwize, the old frame will be encoded.
   */
  int EncodeSingleFrameIntoVideoMSG(SSourcePicture* pSrcPic, igtl::VideoMessage* videoMessage, bool isGrayImage );
  
  /**
   Get the encoder and server initialization status.
   */
  bool GetInitializationStatus(){return InitializationDone;};
  
  /**
   Get the type of encoded frame
   */
  int GetVideoFrameType(){return encodedFrameType;};
  
  void SetCodecName(std::string name);
  
  void SetDeviceName(std::string name);
  
private:
  int ParseLayerConfig (CReadConfig& cRdLayerCfg, const int iLayer, SEncParamExt& pSvcParam, SFilesSet& sFileSet);
  
  int ParseConfig();
  
  int encodedFrameType;
  
  std::string deviceName;
  
  ISVCEncoder*  pSVCEncoder;
  
  SEncParamExt sSvcParam;
  
  SFilesSet layerConfigFiles;
  // for configuration file
  
  CReadConfig cRdCfg;
  
  SFrameBSInfo sFbi;
  
  bool  useCompress;
  
  char  codecName[IGTL_VIDEO_CODEC_NAME_SIZE];
  
  std::string configFile;
    
  bool InitializationDone;
};

#endif
