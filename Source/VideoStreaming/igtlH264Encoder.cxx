/*=========================================================================
 
 Program:   H264Encoder
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
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <stdarg.h>
#if defined (ANDROID_NDK)
#include <android/log.h>
#endif
#ifdef ONLY_ENC_FRAMES_NUM
#undef ONLY_ENC_FRAMES_NUM
#endif//ONLY_ENC_FRAMES_NUM
#define ONLY_ENC_FRAMES_NUM INT_MAX // 2, INT_MAX // type the num you try to encode here, 2, 10, etc

#if defined (WINDOWS_PHONE)
float   g_fFPS           = 0.0;
double  g_dEncoderTime   = 0.0;
int     g_iEncodedFrame  = 0;
#endif

#if defined (ANDROID_NDK)
#define LOG_TAG "welsenc"
#define LOGI(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define printf(...) LOGI(__VA_ARGS__)
#define fprintf(a, ...) LOGI(__VA_ARGS__)
#endif

#ifdef _MSC_VER
#include <io.h>     /* _setmode() */
#include <fcntl.h>  /* _O_BINARY */
#endif//_MSC_VER

#include "codec_def.h"
#include "codec_api.h"

#ifdef _WIN32
#ifdef WINAPI_FAMILY
#include <winapifamily.h>
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#define HAVE_PROCESS_AFFINITY
#endif
#else /* defined(WINAPI_FAMILY) */
#define HAVE_PROCESS_AFFINITY
#endif
#endif /* _WIN32 */

#if defined(__linux__) || defined(__unix__)
#define _FILE_OFFSET_BITS 64
#endif


#include "igtlH264Encoder.h"
#include "igtlVideoMessage.h"

namespace igtl {

/* Ctrl-C handler */
static int     g_iCtrlC = 0;
static void    SigIntHandler (int a) {
  g_iCtrlC = 1;
}

#define   CALC_BI_STRIDE(width,bitcount)  ((((width * bitcount) + 31) & ~31) >> 3)

H264Encoder::H264Encoder(char *configFile):GenericEncoder()
{
  this->pSVCEncoder = NULL;
  
  memset (&sFbi, 0, sizeof (SFrameBSInfo));
  
#ifdef _MSC_VER
  _setmode (_fileno (stdin), _O_BINARY);  /* thanks to Marcoss Morais <morais at dee.ufcg.edu.br> */
  _setmode (_fileno (stdout), _O_BINARY);
  
#endif
  
  /* Control-C handler */
  signal (SIGINT, SigIntHandler);
  
  WelsCreateSVCEncoder(&(this->pSVCEncoder));
  
  this->pSVCEncoder->GetDefaultParams (&sSvcParam);
  
  FillSpecificParameters ();
}

H264Encoder::~H264Encoder()
{
  WelsDestroySVCEncoder (this->pSVCEncoder);
  this->pSVCEncoder = NULL;
}

int H264Encoder::FillSpecificParameters() {
  /* Test for temporal, spatial, SNR scalability */
  sSvcParam.iUsageType = CAMERA_VIDEO_REAL_TIME;
  sSvcParam.fMaxFrameRate = 200.0f;                // input frame rate
  sSvcParam.iPicWidth = 256;                 // width of picture in samples
  sSvcParam.iPicHeight = 256;                  // height of picture in samples
  sSvcParam.iTargetBitrate = 2500000;              // target bitrate desired
  sSvcParam.iMaxBitrate = UNSPECIFIED_BIT_RATE;
  sSvcParam.iRCMode = RC_OFF_MODE;      //  rc mode control
  sSvcParam.bIsLosslessLink = true;
  sSvcParam.iTemporalLayerNum = 1;    // layer number at temporal level
  sSvcParam.iSpatialLayerNum = 1;    // layer number at spatial level
  sSvcParam.iEntropyCodingModeFlag = 1;
  sSvcParam.bEnableDenoise = 0;    // denoise control
  sSvcParam.bEnableBackgroundDetection = 1; // background detection control
  sSvcParam.bEnableAdaptiveQuant = 1; // adaptive quantization control
  sSvcParam.bEnableFrameSkip = 0; // frame skipping
  sSvcParam.bEnableLongTermReference = 0; // long term reference control
  sSvcParam.iLtrMarkPeriod = 30;
  sSvcParam.uiIntraPeriod = 16;           // period of Intra frame
  sSvcParam.bUseLoadBalancing = false;
  sSvcParam.eSpsPpsIdStrategy = INCREASING_ID;
  sSvcParam.bPrefixNalAddingCtrl = 0;
  sSvcParam.iComplexityMode = HIGH_COMPLEXITY;
  sSvcParam.bSimulcastAVC = false;
  sSvcParam.iMaxQp = 51;
  sSvcParam.iMinQp = 0;
  int iIndexLayer = 0;
  sSvcParam.sSpatialLayers[iIndexLayer].uiProfileIdc = PRO_BASELINE;
  sSvcParam.sSpatialLayers[iIndexLayer].uiLevelIdc = LEVEL_5_2;
  sSvcParam.sSpatialLayers[iIndexLayer].iVideoWidth = 256;
  sSvcParam.sSpatialLayers[iIndexLayer].iVideoHeight = 256;
  sSvcParam.sSpatialLayers[iIndexLayer].iDLayerQp = 1;
  sSvcParam.sSpatialLayers[iIndexLayer].fFrameRate = 60.0;
  sSvcParam.sSpatialLayers[iIndexLayer].iSpatialBitrate = 1500000;
  sSvcParam.sSpatialLayers[iIndexLayer].iMaxSpatialBitrate = UNSPECIFIED_BIT_RATE;
  sSvcParam.sSpatialLayers[iIndexLayer].sSliceArgument.uiSliceMode = SM_SINGLE_SLICE;
  sSvcParam.sSpatialLayers[iIndexLayer].sSliceArgument.uiSliceNum = 1;
  
  float fMaxFr = sSvcParam.sSpatialLayers[sSvcParam.iSpatialLayerNum - 1].fFrameRate;
  for (int32_t i = sSvcParam.iSpatialLayerNum - 2; i >= 0; --i) {
    if (sSvcParam.sSpatialLayers[i].fFrameRate > (fMaxFr+EPSN))
      fMaxFr = sSvcParam.sSpatialLayers[i].fFrameRate;
  }
  sSvcParam.fMaxFrameRate = fMaxFr;
  
  return 0;
}

int H264Encoder::SetRCTaregetBitRate(unsigned int bitRate)
{
  this->sSvcParam.iTargetBitrate = bitRate;
  for (int i = 0; i < this->sSvcParam.iSpatialLayerNum; i++)
    {
    this->sSvcParam.sSpatialLayers[i].iSpatialBitrate = bitRate/this->sSvcParam.iSpatialLayerNum;
    }
  if (this->pSVCEncoder->InitializeExt (&sSvcParam)) {
    fprintf (stderr, "Set target bit rate failed. \n");
    return -1;
  }
  this->initializationDone = true;
  return 0;
}

int H264Encoder::SetRCMode(int value)
{
  sSvcParam.iRCMode = (RC_MODES)value;
  if(sSvcParam.iRCMode == RC_QUALITY_MODE || sSvcParam.iRCMode == RC_BITRATE_MODE || sSvcParam.iRCMode == RC_TIMESTAMP_MODE)
    {
    sSvcParam.bEnableFrameSkip = true;
    }
  if (this->pSVCEncoder->InitializeExt (&sSvcParam)) {
    fprintf (stderr, "Set RC mode failed. \n");
    return -1;
  }
  this->initializationDone = true;
  return 0;
}

int H264Encoder::SetQP(int maxQP, int minQP)
{
  sSvcParam.iMaxQp = maxQP<51?maxQP:51;
  sSvcParam.iMinQp = minQP>0?minQP:0;
  for(int i = 0; i <sSvcParam.iSpatialLayerNum; i++)
    {
    sSvcParam.sSpatialLayers[i].iDLayerQp = (sSvcParam.iMaxQp + sSvcParam.iMinQp)/2;
    }
  sSvcParam.iRCMode = RC_OFF_MODE;
  if (this->pSVCEncoder->InitializeExt (&sSvcParam)) {
    fprintf (stderr, "Set QP value failed.\n");
    return -1;
  }
  this->initializationDone = true;
  return 0;
}

int H264Encoder::ParseLayerConfig (string strTag[], const int iLayer, SEncParamExt& pSvcParam) {
  
  SSpatialLayerConfig* pDLayer = &pSvcParam.sSpatialLayers[iLayer];
  int iLeftTargetBitrate = (pSvcParam.iRCMode != RC_OFF_MODE) ? pSvcParam.iTargetBitrate : 0;
  SLayerPEncCtx sLayerCtx;
  memset (&sLayerCtx, 0, sizeof (SLayerPEncCtx));
  
  string str_ ("SlicesAssign");
  const int kiSize = (int)str_.size();
  
  if (!strTag[0].empty())
    {
    if (strTag[0].compare ("FrameWidth") == 0)
      {
      pDLayer->iVideoWidth = atoi (strTag[1].c_str());
      }
    else if (strTag[0].compare ("FrameHeight") == 0)
      {
      pDLayer->iVideoHeight = atoi (strTag[1].c_str());
      }
    else if (strTag[0].compare ("FrameRateOut") == 0)
      {
      pDLayer->fFrameRate = (float)atof (strTag[1].c_str());
      }
    else if (strTag[0].compare ("ProfileIdc") == 0)
      {
      pDLayer->uiProfileIdc = (EProfileIdc)atoi (strTag[1].c_str());
      }
    else if (strTag[0].compare ("SpatialBitrate") == 0)
      {
      pDLayer->iSpatialBitrate = 1000 * atoi (strTag[1].c_str());
      if (pSvcParam.iRCMode != RC_OFF_MODE)
        {
        if (pDLayer->iSpatialBitrate <= 0)
          {
          fprintf (stderr, "Invalid spatial bitrate(%d) in dependency layer #%d.\n", pDLayer->iSpatialBitrate, iLayer);
          return -1;
          }
        if (pDLayer->iSpatialBitrate > iLeftTargetBitrate)
          {
          fprintf (stderr, "Invalid spatial(#%d) bitrate(%d) setting due to unavailable left(%d)!\n", iLayer,
                   pDLayer->iSpatialBitrate, iLeftTargetBitrate);
          return -1;
          }
        iLeftTargetBitrate -= pDLayer->iSpatialBitrate;
        }
      }
    else if (strTag[0].compare ("MaxSpatialBitrate") == 0)
      {
      pDLayer->iMaxSpatialBitrate = 1000 * atoi (strTag[1].c_str());
      if (pSvcParam.iRCMode != RC_OFF_MODE)
        {
        if (pDLayer->iMaxSpatialBitrate < 0)
          {
          fprintf (stderr, "Invalid max spatial bitrate(%d) in dependency layer #%d.\n", pDLayer->iMaxSpatialBitrate, iLayer);
          return -1;
          }
        if (pDLayer->iMaxSpatialBitrate > 0 && pDLayer->iMaxSpatialBitrate < pDLayer->iSpatialBitrate)
          {
          fprintf (stderr, "Invalid max spatial(#%d) bitrate(%d) setting::: < layerBitrate(%d)!\n", iLayer,
                   pDLayer->iMaxSpatialBitrate, pDLayer->iSpatialBitrate);
          return -1;
          }
        }
      }
    else if (strTag[0].compare ("InitialQP") == 0)
      {
      sLayerCtx.iDLayerQp = atoi (strTag[1].c_str());
      }
    else if (strTag[0].compare ("SliceMode") == 0)
      {
      sLayerCtx.sSliceArgument.uiSliceMode = (SliceModeEnum)atoi (strTag[1].c_str());
      }
    else if (strTag[0].compare ("SliceSize") == 0)
      { //SM_SIZELIMITED_SLICE
        sLayerCtx.sSliceArgument.uiSliceSizeConstraint = atoi (strTag[1].c_str());
      }
    else if (strTag[0].compare ("SliceNum") == 0)
      {
      sLayerCtx.sSliceArgument.uiSliceNum = atoi (strTag[1].c_str());
      }
    else if (strTag[0].compare (0, kiSize, str_) == 0)
      {
      const char* kpString = strTag[0].c_str();
      int uiSliceIdx = atoi (&kpString[kiSize]);
      sLayerCtx.sSliceArgument.uiSliceMbNum[uiSliceIdx] = atoi (strTag[1].c_str());
      }
    }
  pDLayer->iDLayerQp             = sLayerCtx.iDLayerQp;
  pDLayer->sSliceArgument.uiSliceMode = sLayerCtx.sSliceArgument.uiSliceMode;
  
  memcpy (&pDLayer->sSliceArgument, &sLayerCtx.sSliceArgument, sizeof (SSliceArgument)); // confirmed_safe_unsafe_usage
  memcpy (&pDLayer->sSliceArgument.uiSliceMbNum[0], &sLayerCtx.sSliceArgument.uiSliceMbNum[0],
          sizeof (sLayerCtx.sSliceArgument.uiSliceMbNum)); // confirmed_safe_unsafe_usage
  
  return 0;
}

int H264Encoder::ParseConfig() {
  string strTag[4];
  
  while (!cRdCfg.EndOfFile())
    {
    long iRd = cRdCfg.ReadLine (&strTag[0]);
    if (iRd > 0)
      {
      if (strTag[0].empty())
        continue;
      if (strTag[0].compare ("PicWidth") == 0)
        {
        sSvcParam.iPicWidth = atoi(strTag[1].c_str());
        }
      if (strTag[0].compare ("PicHeight") == 0)
        {
        sSvcParam.iPicHeight = atoi(strTag[1].c_str());
        }
      if (strTag[0].compare ("UsageType") == 0)
        {
        sSvcParam.iUsageType = (EUsageType)atoi (strTag[1].c_str());
        }
      else if (strTag[0].compare ("SimulcastAVC") == 0)
        {
        sSvcParam.bSimulcastAVC = atoi (strTag[1].c_str()) ? true : false;
        }
      else if (strTag[0].compare ("TemporalLayerNum") == 0)
        {
        sSvcParam.iTemporalLayerNum = atoi (strTag[1].c_str());
        }
      else if (strTag[0].compare ("IntraPeriod") == 0)
        {
        sSvcParam.uiIntraPeriod = atoi (strTag[1].c_str());
        }
      else if (strTag[0].compare ("MaxNalSize") == 0)
        {
        sSvcParam.uiMaxNalSize = atoi (strTag[1].c_str());
        }
      else if (strTag[0].compare ("SpsPpsIDStrategy") == 0)
        {
        int32_t iValue = atoi (strTag[1].c_str());
        switch (iValue)
          {
            case 0:
            sSvcParam.eSpsPpsIdStrategy  = CONSTANT_ID;
            break;
            case 0x01:
            sSvcParam.eSpsPpsIdStrategy  = INCREASING_ID;
            break;
            case 0x02:
            sSvcParam.eSpsPpsIdStrategy  = SPS_LISTING;
            break;
            case 0x03:
            sSvcParam.eSpsPpsIdStrategy  = SPS_LISTING_AND_PPS_INCREASING;
            break;
            case 0x06:
            sSvcParam.eSpsPpsIdStrategy  = SPS_PPS_LISTING;
            break;
            default:
            sSvcParam.eSpsPpsIdStrategy  = CONSTANT_ID;
            break;
          }
        }
      else if (strTag[0].compare ("EnableScalableSEI") == 0)
        {
        sSvcParam.bEnableSSEI = atoi (strTag[1].c_str()) ? true : false;
        }
      else if (strTag[0].compare ("EnableFrameCropping") == 0)
        {
        sSvcParam.bEnableFrameCroppingFlag = (atoi (strTag[1].c_str()) != 0);
        }
      else if (strTag[0].compare ("EntropyCodingModeFlag") == 0)
        {
        sSvcParam.iEntropyCodingModeFlag = (atoi (strTag[1].c_str()) != 0);
        }
      else if (strTag[0].compare ("LoopFilterDisableIDC") == 0)
        {
        sSvcParam.iLoopFilterDisableIdc = (int8_t)atoi (strTag[1].c_str());
        if (sSvcParam.iLoopFilterDisableIdc > 6 || sSvcParam.iLoopFilterDisableIdc < 0)
          {
          fprintf (stderr, "Invalid parameter in iLoopFilterDisableIdc: %d.\n", sSvcParam.iLoopFilterDisableIdc);
          return -1;
          }
        }
      else if (strTag[0].compare ("LoopFilterAlphaC0Offset") == 0)
        {
        sSvcParam.iLoopFilterAlphaC0Offset = (int8_t)atoi (strTag[1].c_str());
        if (sSvcParam.iLoopFilterAlphaC0Offset < -6)
          sSvcParam.iLoopFilterAlphaC0Offset = -6;
        else if (sSvcParam.iLoopFilterAlphaC0Offset > 6)
          sSvcParam.iLoopFilterAlphaC0Offset = 6;
        }
      else if (strTag[0].compare ("LoopFilterBetaOffset") == 0)
        {
        sSvcParam.iLoopFilterBetaOffset = (int8_t)atoi (strTag[1].c_str());
        if (sSvcParam.iLoopFilterBetaOffset < -6)
          sSvcParam.iLoopFilterBetaOffset = -6;
        else if (sSvcParam.iLoopFilterBetaOffset > 6)
          sSvcParam.iLoopFilterBetaOffset = 6;
        }
      else if (strTag[0].compare ("MultipleThreadIdc") == 0)
        {
        // # 0: auto(dynamic imp. internal encoder); 1: multiple threads imp. disabled; > 1: count number of threads;
        sSvcParam.iMultipleThreadIdc = atoi (strTag[1].c_str());
        if (sSvcParam.iMultipleThreadIdc < 0)
          sSvcParam.iMultipleThreadIdc = 0;
        else if (sSvcParam.iMultipleThreadIdc > MAX_THREADS_NUM)
          sSvcParam.iMultipleThreadIdc = MAX_THREADS_NUM;
        }
      else if (strTag[0].compare ("UseLoadBalancing") == 0)
        {
        sSvcParam.bUseLoadBalancing = (atoi (strTag[1].c_str())) ? true : false;
        }
      else if (strTag[0].compare ("RCMode") == 0)
        {
        sSvcParam.iRCMode = (RC_MODES) atoi (strTag[1].c_str());
        }
      else if (strTag[0].compare ("TargetBitrate") == 0)
        {
        sSvcParam.iTargetBitrate = 1000 * atoi (strTag[1].c_str());
        if ((sSvcParam.iRCMode != RC_OFF_MODE) && sSvcParam.iTargetBitrate <= 0)
          {
          fprintf (stderr, "Invalid target bitrate setting due to RC enabled. Check TargetBitrate field please!\n");
          return -1;
          }
        }
      else if (strTag[0].compare ("MaxOverallBitrate") == 0)
        {
        sSvcParam.iMaxBitrate = 1000 * atoi (strTag[1].c_str());
        if ((sSvcParam.iRCMode != RC_OFF_MODE) && sSvcParam.iMaxBitrate < 0)
          {
          fprintf (stderr, "Invalid max overall bitrate setting due to RC enabled. Check MaxOverallBitrate field please!\n");
          return -1;
          }
        }
      else if (strTag[0].compare ("MaxQp") == 0)
        {
        sSvcParam.iMaxQp = atoi (strTag[1].c_str());
        }
      else if (strTag[0].compare ("MinQp") == 0)
        {
        sSvcParam.iMinQp = atoi (strTag[1].c_str());
        }
      else if (strTag[0].compare ("EnableDenoise") == 0)
        {
        sSvcParam.bEnableDenoise = atoi (strTag[1].c_str()) ? true : false;
        }
      else if (strTag[0].compare ("EnableSceneChangeDetection") == 0)
        {
        sSvcParam.bEnableSceneChangeDetect = atoi (strTag[1].c_str()) ? true : false;
        }
      else if (strTag[0].compare ("EnableBackgroundDetection") == 0)
        {
        sSvcParam.bEnableBackgroundDetection = atoi (strTag[1].c_str()) ? true : false;
        }
      else if (strTag[0].compare ("EnableAdaptiveQuantization") == 0)
        {
        sSvcParam.bEnableAdaptiveQuant = atoi (strTag[1].c_str()) ? true : false;
        }
      else if (strTag[0].compare ("EnableFrameSkip") == 0)
        {
        sSvcParam.bEnableFrameSkip = atoi (strTag[1].c_str()) ? true : false;
        }
      else if (strTag[0].compare ("EnableLongTermReference") == 0)
        {
        sSvcParam.bEnableLongTermReference = atoi (strTag[1].c_str()) ? true : false;
        }
      else if (strTag[0].compare ("LongTermReferenceNumber") == 0)
        {
        sSvcParam.iLTRRefNum = atoi (strTag[1].c_str());
        }
      else if (strTag[0].compare ("LtrMarkPeriod") == 0)
        {
        sSvcParam.iLtrMarkPeriod = (uint32_t)atoi (strTag[1].c_str());
        }
      else if (strTag[0].compare ("LosslessLink") == 0)
        {
        sSvcParam.bIsLosslessLink = atoi (strTag[1].c_str()) ? true : false;
        }
      else if (strTag[0].compare ("NumLayers") == 0)
        {
        sSvcParam.iSpatialLayerNum = (int8_t)atoi (strTag[1].c_str());
        if (sSvcParam.iSpatialLayerNum > MAX_DEPENDENCY_LAYER || sSvcParam.iSpatialLayerNum <= 0)
          {
          fprintf (stderr, "Invalid parameter in iSpatialLayerNum: %d.\n", sSvcParam.iSpatialLayerNum);
          return -1;
          }
        break;
        }
      else if (strTag[0].compare ("PrefixNALAddingCtrl") == 0)
        {
        int ctrl_flag = atoi (strTag[1].c_str());
        if (ctrl_flag > 1)
          ctrl_flag = 1;
        else if (ctrl_flag < 0)
          ctrl_flag = 0;
        sSvcParam.bPrefixNalAddingCtrl = ctrl_flag ? true : false;
        }
      }
    }
  igtlUint8 actLayerConfigNum = 0;
  igtlUint8 layerIndex = -1;
  while (!cRdCfg.EndOfFile())
    {
    long iRd = cRdCfg.ReadLine (&strTag[0]);
    if (iRd > 0)
      {
      if (strTag[0].empty())
        continue;
      if (strTag[0].compare ("LayerIndex") == 0)
        {
        layerIndex = atoi(strTag[1].c_str());
        if(layerIndex>=0)
          {
          SSpatialLayerConfig* pDLayer = &sSvcParam.sSpatialLayers[layerIndex];
          pDLayer->iVideoWidth = this->sSvcParam.iPicWidth;
          pDLayer->iVideoHeight = this->sSvcParam.iPicHeight;
          actLayerConfigNum++;
          }
        }
      if(layerIndex>=0)
        {
        this->ParseLayerConfig(strTag, layerIndex, sSvcParam);
        }
      }
    }
  
  const igtlUint8 kiActualLayerNum = sSvcParam.iSpatialLayerNum<actLayerConfigNum ? sSvcParam.iSpatialLayerNum:actLayerConfigNum;
  if (sSvcParam.iSpatialLayerNum >
      kiActualLayerNum) { // fixed number of dependency layer due to parameter error in settings
    sSvcParam.iSpatialLayerNum = kiActualLayerNum;
  }
  return 0;
}

int H264Encoder::SetLosslessLink(bool linkMethod)
{
  this->isLossLessLink = linkMethod;
  this->sSvcParam.bIsLosslessLink = isLossLessLink;
  if (this->pSVCEncoder->InitializeExt (&sSvcParam))
    {
    fprintf (stderr, "parse svc parameter config file failed.\n");
    return -1;
    }
  this->initializationDone = true;
  return 0;
}

int H264Encoder::SetSpeed(int speed)
{
  speed = speed>=LOW_COMPLEXITY?speed:LOW_COMPLEXITY;
  speed = speed<=HIGH_COMPLEXITY?speed:HIGH_COMPLEXITY;
  this->codecSpeed = speed;
  this->sSvcParam.iComplexityMode = (ECOMPLEXITY_MODE)(2-this->codecSpeed);
  if (this->pSVCEncoder->InitializeExt (&sSvcParam))
    {
    fprintf (stderr, "Set speed mode failed.\n");
    return -1;
    }
  return 0;
}

int H264Encoder::InitializeEncoder()
{
  //------------------------------------------------------------
  int iRet = 0;
  
  if (this->configFile=="" && this->pSVCEncoder)
    {
    fprintf (stderr, "No configuration file specified. Use Default Parameters\n");
    iRet = this->pSVCEncoder->InitializeExt (&sSvcParam);
    if (iRet) {
      fprintf (stderr, "parse svc parameter config file failed.\n");
      goto INSIDE_MEM_FREE;
    }
    this->initializationDone = true;
    return iRet;
    }
  else
    {
    cRdCfg.OpenFile (this->configFile.c_str());// to do get the first augments from this->augments.
    if (cRdCfg.ExistFile())
      {
      iRet = ParseConfig();
      if (iRet) {
        fprintf (stderr, "parse svc parameter config file failed.\n");
        goto INSIDE_MEM_FREE;
      }
      }
    else
      {
      fprintf (stderr, "Specified file: %s not exist, maybe invalid path or parameter settting.\n",
               cRdCfg.GetFileName().c_str());
      iRet = 1;
      goto INSIDE_MEM_FREE;
      }
    static int     g_LevelSetting = WELS_LOG_ERROR;
    this->pSVCEncoder->SetOption (ENCODER_OPTION_TRACE_LEVEL, &g_LevelSetting);
    //finish reading the configuration
    
    //  sSvcParam.bSimulcastAVC = true;
    if (cmResultSuccess != this->pSVCEncoder->InitializeExt (&sSvcParam)) { // SVC encoder initialization
      fprintf (stderr, "SVC encoder Initialize failed\n");
      iRet = 1;
      goto INSIDE_MEM_FREE;
    }
    this->initializationDone = true;
    return 0;
    }
  
INSIDE_MEM_FREE:
  this->initializationDone = false;
  return -1;
}

int H264Encoder::SetPicWidthAndHeight(unsigned int Width, unsigned int Height)
{
  this->sSvcParam.iPicWidth = Width;
  this->sSvcParam.iPicHeight = Height;
  if(sSvcParam.iSpatialLayerNum)
    {
    SSpatialLayerConfig* pDLayer = &sSvcParam.sSpatialLayers[0]; //reset only the first spatial layer
    pDLayer->iVideoWidth = this->sSvcParam.iPicWidth;
    pDLayer->iVideoHeight = this->sSvcParam.iPicHeight;
    }
  if (this->pSVCEncoder->InitializeExt (&sSvcParam)) {
    fprintf (stderr, "parse svc parameter config file failed.\n");
    return -1;
  }
  this->initializationDone = true;
  return 0;
}


igtlInt32 H264Encoder::InitPic (const void* kpSrc, const int32_t colorspace, const int32_t width, const int32_t height) {
  SSourcePicture* pSrcPic = (SSourcePicture*)kpSrc;
  
  if (NULL == pSrcPic || width == 0 || height == 0)
    return 1;
  
  pSrcPic->iColorFormat = colorspace;
  pSrcPic->iPicWidth    = width;
  pSrcPic->iPicHeight   = height;
  
  //currently encoder only supports videoFormatI420.
  if ((colorspace & (~videoFormatVFlip)) != videoFormatI420)
    return 2;
  switch (colorspace & (~videoFormatVFlip)) {
    case videoFormatI420:
    case videoFormatYV12:
      pSrcPic->pData[0]   = NULL;
      pSrcPic->pData[1]   = NULL;
      pSrcPic->pData[2]   = NULL;
      pSrcPic->pData[3]   = NULL;
      pSrcPic->iStride[0] = width;
      pSrcPic->iStride[2] = pSrcPic->iStride[1] = width >> 1;
      pSrcPic->iStride[3] = 0;
      break;
    case videoFormatYUY2:
    case videoFormatYVYU:
    case videoFormatUYVY:
      pSrcPic->pData[0]   = NULL;
      pSrcPic->pData[1]   = NULL;
      pSrcPic->pData[2]   = NULL;
      pSrcPic->pData[3]   = NULL;
      pSrcPic->iStride[0] = CALC_BI_STRIDE (width,  16);
      pSrcPic->iStride[3] = pSrcPic->iStride[2] = pSrcPic->iStride[1] = 0;
      break;
    case videoFormatRGB:
    case videoFormatBGR:
      pSrcPic->pData[0]   = NULL;
      pSrcPic->pData[1]   = NULL;
      pSrcPic->pData[2]   = NULL;
      pSrcPic->pData[3]   = NULL;
      pSrcPic->iStride[0] = CALC_BI_STRIDE (width, 24);
      pSrcPic->iStride[3] = pSrcPic->iStride[2] = pSrcPic->iStride[1] = 0;
      if (colorspace & videoFormatVFlip)
        pSrcPic->iColorFormat = colorspace & (~videoFormatVFlip);
      else
        pSrcPic->iColorFormat = colorspace | videoFormatVFlip;
      break;
    case videoFormatBGRA:
    case videoFormatRGBA:
    case videoFormatARGB:
    case videoFormatABGR:
      pSrcPic->pData[0]   = NULL;
      pSrcPic->pData[1]   = NULL;
      pSrcPic->pData[2]   = NULL;
      pSrcPic->pData[3]   = NULL;
      pSrcPic->iStride[0] = width << 2;
      pSrcPic->iStride[3] = pSrcPic->iStride[2] = pSrcPic->iStride[1] = 0;
      if (colorspace & videoFormatVFlip)
        pSrcPic->iColorFormat = colorspace & (~videoFormatVFlip);
      else
        pSrcPic->iColorFormat = colorspace | videoFormatVFlip;
      break;
    default:
      return 2; // any else?
  }
  
  return 0;
}

int H264Encoder::ConvertToLocalImageFormat(SourcePicture* pSrcPic)
{
  if(pSrcPic->colorFormat==FormatI420)
    {
    if((pSrcPic->picWidth != this->h264SrcPicture.iPicWidth || pSrcPic->picHeight != this->h264SrcPicture.iPicHeight))
      {
      InitPic (&this->h264SrcPicture, videoFormatI420, pSrcPic->picWidth, pSrcPic->picHeight);
      }
    for(int i = 0; i < 4; i++)
      {
      h264SrcPicture.iStride[i] = pSrcPic->stride[i];
      h264SrcPicture.pData[i] = pSrcPic->data[i];
      }
    h264SrcPicture.iPicWidth = pSrcPic->picWidth;
    h264SrcPicture.iPicHeight = pSrcPic->picHeight;
    return 1;
    }
  return -1;// image format not supported
}


int H264Encoder::EncodeSingleFrameIntoVideoMSG(SourcePicture* pSrcPic, igtl::VideoMessage* videoMessage, bool isGrayImage)
{
  int encodeRet = -1;
  int iSourceWidth = pSrcPic->picWidth;
  int iSourceHeight = pSrcPic->picHeight;
  if (iSourceWidth != this->sSvcParam.iPicWidth || iSourceHeight != this->sSvcParam.iPicHeight)
    {
    this->SetPicWidthAndHeight(iSourceWidth, iSourceHeight);
    this->InitializeEncoder();
    }
  if (this->initializationDone == true)
    {
    pSrcPic->stride[0] = iSourceWidth;
    pSrcPic->stride[1] = pSrcPic->stride[2] = pSrcPic->stride[0] >> 1;
    int frameSize = 0;
    int iLayer = 0;
    this->ConvertToLocalImageFormat(pSrcPic);
    encodeRet = pSVCEncoder->EncodeFrame(&h264SrcPicture, &sFbi);
    videoMessage->SetBitStreamSize(sFbi.iFrameSizeInBytes);
    videoMessage->AllocateScalars();
    videoMessage->SetCodecType(IGTL_VIDEO_CODEC_NAME_H264);
    videoMessage->SetEndian(igtl_is_little_endian()==true?2:1); //little endian is 2 big endian is 1
    videoMessage->SetWidth(pSrcPic->picWidth);
    videoMessage->SetHeight(pSrcPic->picHeight);
    if (sFbi.eFrameType == videoFrameTypeIDR)
      {
      encodedFrameType = FrameTypeKey;
      }
    else
      {
      // To do, assign other frame type too.
      encodedFrameType = FrameTypeUnKnown;
      }
    if (isGrayImage)
      {
      encodedFrameType = sFbi.eFrameType<<8;
      }
    videoMessage->SetFrameType(encodedFrameType);
    while (iLayer < sFbi.iLayerNum) {
      SLayerBSInfo* pLayerBsInfo = &sFbi.sLayerInfo[iLayer];
      if (pLayerBsInfo != NULL) {
        int iLayerSize = 0;
        int iNalIdx = pLayerBsInfo->iNalCount - 1;
        do {
          iLayerSize += pLayerBsInfo->pNalLengthInByte[iNalIdx];
          -- iNalIdx;
        } while (iNalIdx >= 0);
        frameSize += iLayerSize;
        for (int i = 0; i < iLayerSize ; i++)
          {
          videoMessage->GetPackFragmentPointer(2)[frameSize-iLayerSize+i] = pLayerBsInfo->pBsBuf[i];
          }
      }
      ++ iLayer;
    }
    videoMessage->Pack();
    }
  return encodeRet;
};

} // Namespace igtl
