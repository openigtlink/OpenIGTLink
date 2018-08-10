/*=========================================================================
 
 Program:   OpenIGTLink
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#ifndef __igtlH265Encoder_h
#define __igtlH265Encoder_h

#include <time.h>
#if defined(_WIN32) /*&& defined(_DEBUG)*/
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <signal.h>
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

#if defined ( __cplusplus)
  extern "C"
  {
#include "x265.h"
  };
#else
#include "x265.h"
#endif

using namespace std;

namespace igtl {

/**
 * @brief Enumerate  video frame type
 * This is H265 related frame types.
 */
typedef enum {
  H265FrameTypeInvalid,    ///< encoder not ready or parameters are invalidate
  H265FrameTypeIDR,        ///< IDR frame in H.264
  H265FrameTypeI,          ///< I frame type
  H265FrameTypeP,          ///< P frame type
  H265FrameTypeSkip,       ///< skip the frame based encoder kernel
  H265FrameTypeIPMixed     ///< a frame where I and P slices are mixing, not supported yet
} H265VideoFrameType;

class IGTLCommon_EXPORT H265Encoder: public GenericEncoder
{
public:
  igtlTypeMacro(H265Encoder, GenericEncoder);
  igtlNewMacro(H265Encoder);

  H265Encoder(char * configFile = NULL);
  ~H265Encoder();
  
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
  
  virtual unsigned int GetPicWidth(){return this->sSvcParam->sourceWidth;};
  
  virtual unsigned int GetPicHeight(){return this->sSvcParam->sourceHeight;};
  
  virtual int SetLosslessLink(bool linkMethod);
  
  virtual int SetSpeed(int speed);
  
  virtual int SetRCTaregetBitRate(unsigned int bitRate);
  
  virtual bool GetLosslessLink(){return this->sSvcParam->bLossless;};
  
protected:
  
  void CopySettingToAnother(x265_param* srcSetting, x265_param* dstSetting);
  
private:
  
  x265_encoder*  pSVCEncoder;
  
  x265_param* sSvcParam;
  
  x265_nal *pNals;
  
  x265_picture* H265SrcPicture;
  
};

} //namespace igtl
#endif