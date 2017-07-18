//
//  CodecEvaluation.cpp
//  OpenIGTLink
//
//  Created by Longquan Chen on 4/28/17.
//
//

#include <stdio.h>


/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/
/*
 *  Copyright (c) 2014 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <sstream>
#include <map>
#include "igtlVideoMessage.h"
#include "igtlMessageDebugFunction.h"
#include "igtl_video.h"
#include "igtl_types.h"
#include "igtl_header.h"
#include "igtl_util.h"
#include "string.h"
#include <cstring>
#include <string>

#if OpenIGTLink_LINK_H264
#include "H264Encoder.h"
#include "H264Decoder.h"
#endif

#if OpenIGTLink_LINK_VP9
#include "VP9Encoder.h"
#include "VP9Decoder.h"
#include "../video_reader.h"
#include "./vpx_config.h"
#include "vpx_dsp_rtcd.h"
#include "vpx_dsp/ssim.h"
#endif

#if OpenIGTLink_LINK_X265
#include "H265Encoder.h"
#include "H265Decoder.h"
#endif

int startEncodeTime = 0;
int endEncodeTime = 0;
int startDecodeTime = 0;
int endDecodeTime = 0;
int totalEncodeTime = 0;
int totalDecodeTime =0;
double ssim = 0.0;
float compressionRate = 0.0;

int Width = 446;
int Height = 460;
std::string testFileDir("/Users/longquanchen/Documents/VideoStreaming/PaperCodecOptimization/UltrasonixImages");
std::string evalFileName("EvalFile.txt");
FILE* pEval = NULL;
int startIndex = 0;
int  inputFrameNum = 4337;
int totolFrameNumber = 0;

template <typename T>
std::string ToString(T variable)
{
  std::stringstream stream;
  stream << variable;
  return stream.str();
}

//------------------------
//Taken from the libvpx and modified, as the original code is broken,
//will be removed when the original code bug is fixed.
void vpx_ssim_parms_8x8_c(const uint8_t *s, int sp, const uint8_t *r, int rp,
                          uint32_t *sum_s, uint32_t *sum_r, uint32_t *sum_sq_s,
                          uint32_t *sum_sq_r, uint32_t *sum_sxr) {
  int i, j;
  for (i = 0; i < 8; i++, s += sp, r += rp) {
    for (j = 0; j < 8; j++) {
      *sum_s += s[j];
      *sum_r += r[j];
      *sum_sq_s += s[j] * s[j];
      *sum_sq_r += r[j] * r[j];
      *sum_sxr += s[j] * r[j];
    }
  }
}

static const int64_t cc1 = 26634;        // (64^2*(.01*255)^2
static const int64_t cc2 = 239708;       // (64^2*(.03*255)^2
static const int64_t cc1_10 = 428658;    // (64^2*(.01*1023)^2
static const int64_t cc2_10 = 3857925;   // (64^2*(.03*1023)^2
static const int64_t cc1_12 = 6868593;   // (64^2*(.01*4095)^2
static const int64_t cc2_12 = 61817334;  // (64^2*(.03*4095)^2

static double similarity(uint32_t sum_s, uint32_t sum_r, uint32_t sum_sq_s,
                         uint32_t sum_sq_r, uint32_t sum_sxr, int count,
                         uint32_t bd) {
  int64_t ssim_n, ssim_d;
  int64_t c1, c2;
  if (bd == 8) {
    // scale the constants by number of pixels
    c1 = (cc1 * count * count) >> 12;
    c2 = (cc2 * count * count) >> 12;
  } else if (bd == 10) {
    c1 = (cc1_10 * count * count) >> 12;
    c2 = (cc2_10 * count * count) >> 12;
  } else if (bd == 12) {
    c1 = (cc1_12 * count * count) >> 12;
    c2 = (cc2_12 * count * count) >> 12;
  } else {
    c1 = c2 = 0;
    //assert(0);
  }
  
  ssim_n = (2 * sum_s * sum_r + c1) *
  ((int64_t)2 * count * sum_sxr - (int64_t)2 * sum_s * sum_r + c2);
  
  ssim_d = (sum_s * sum_s + sum_r * sum_r + c1) *
  ((int64_t)count * sum_sq_s - (int64_t)sum_s * sum_s +
   (int64_t)count * sum_sq_r - (int64_t)sum_r * sum_r + c2);
  
  return ssim_n * 1.0 / ssim_d;
}

static double local_vpx_ssim2(const uint8_t *img1, const uint8_t *img2,
                              int stride_img1, int stride_img2, int width,
                              int height) {
  int i, j;
  int samples = 0;
  double ssim_total = 0;
  
  // sample point start with each 4x4 location
  for (i = 0; i <= height - 8;
       i += 4, img1 += stride_img1 * 4, img2 += stride_img2 * 4) {
    for (j = 0; j <= width - 8; j += 4) {
      uint32_t sum_s = 0, sum_r = 0, sum_sq_s = 0, sum_sq_r = 0, sum_sxr = 0;
      vpx_ssim_parms_8x8_c(img1 + j, stride_img1, img2 + j, stride_img2, &sum_s, &sum_r, &sum_sq_s, &sum_sq_r,
                           &sum_sxr);
      double v = similarity(sum_s, sum_r, sum_sq_s, sum_sq_r, sum_sxr, 64, 8);
      ssim_total += v;
      samples++;
    }
  }
  ssim_total /= samples;
  return ssim_total;
}
//Taken from the libvpx and modified, as the original code is broken,
//will be removed when the original code bug is fixed.
//-----------------------------------
void TestWithVersion(int version, GenericEncoder* videoStreamEncoder, GenericDecoder* videoStreamDecoder, bool compareImage)
{
  // Get thread information
  int kiPicResSize = Width*Height;
  igtl_uint8* imagePointer = new igtl_uint8[kiPicResSize*3/2];
  memset(imagePointer, 0, Width * Height * 3 / 2);
  SourcePicture* pSrcPic = new SourcePicture();
  pSrcPic->colorFormat = FormatI420;
  pSrcPic->picWidth = Width; // check the test image
  pSrcPic->picHeight = Height;
  pSrcPic->data[0] = imagePointer;
  pSrcPic->data[1] = pSrcPic->data[0] + kiPicResSize;
  pSrcPic->data[2] = pSrcPic->data[1] + kiPicResSize/4;
  pSrcPic->stride[0] = pSrcPic->picWidth;
  pSrcPic->stride[1] = pSrcPic->stride[2] = pSrcPic->stride[0] >> 1;
  
  SourcePicture* pDecodedPic = new SourcePicture();
  pDecodedPic->data[0] = new igtl_uint8[kiPicResSize*3/2];
  memset(pDecodedPic->data[0], 0, Width * Height * 3 / 2);
  int bitstreamTotalLength = 0;
  ssim = 0.0;
  totalEncodeTime = 0;
  totalDecodeTime = 0;
  pEval = fopen (evalFileName.c_str(), "a");
  totolFrameNumber=0;
  for(int i = startIndex; i <inputFrameNum; i++)
  {    std::string sep = "/";
#if defined(_WIN32) || defined(_WIN64)
    sep = "\\";
#endif
    std::string stringTemp = testFileDir;
    std::string testIndexedFileName = stringTemp.append(sep).append(std::to_string(i)).append(".yuv");
    FILE* pFileYUV = NULL;
    pFileYUV = fopen (testIndexedFileName.c_str(), "rb");
    if (pFileYUV != NULL) {
      while(fread (imagePointer, 1, kiPicResSize, pFileYUV ) == kiPicResSize)
      {
        bool isGrayImage = true;
        igtl::VideoMessage::Pointer videoMessageSend = igtl::VideoMessage::New();
        videoMessageSend->SetHeaderVersion(version);
        startEncodeTime = videoStreamDecoder->getCurrentTime();
        int iEncFrames = videoStreamEncoder->EncodeSingleFrameIntoVideoMSG(pSrcPic, videoMessageSend, isGrayImage);
        endEncodeTime = videoStreamDecoder->getCurrentTime();
        totalEncodeTime += (endEncodeTime - startEncodeTime);
        //std::cerr<<"totalEncodeTime: " <<totalEncodeTime << " packet Size: "<<videoMessageSend->GetPackBodySize()<<" frame Encoded: "<<totalFrame<<std::endl;
        if(iEncFrames == 0)
        {
          igtl::VideoMessage::Pointer videoMessageReceived = igtl::VideoMessage::New();
          igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
          headerMsg->AllocatePack();
          memcpy(headerMsg->GetPackPointer(), videoMessageSend->GetPackPointer(), IGTL_HEADER_SIZE);
          headerMsg->Unpack();
          videoMessageReceived->SetMessageHeader(headerMsg);
          videoMessageReceived->AllocatePack();
          memcpy(videoMessageReceived->GetPackBodyPointer(), videoMessageSend->GetPackBodyPointer(), videoMessageSend->GetPackBodySize());
          /*
          std::string ultraSonixFile = "/Users/longquanchen/UltrasonixBitStream";
          FILE *pFileBitStream = fopen (ultraSonixFile.c_str(), "rb");
          unsigned char messageBitStream[41186];
          fread (messageBitStream, 1, 41186, pFileBitStream);
          videoMessageReceived = igtl::VideoMessage::New();
          videoMessageReceived->SetBitStreamSize(41114);
          videoMessageReceived->AllocatePack();
          memcpy(videoMessageReceived->GetPackBodyPointer(), messageBitStream+IGTL_HEADER_SIZE, 41186-IGTL_HEADER_SIZE);*/
          videoMessageReceived->Unpack();
          bitstreamTotalLength += videoMessageReceived->GetBitStreamSize();
          startDecodeTime = videoStreamDecoder->getCurrentTime();
          int iRet= videoStreamDecoder->DecodeVideoMSGIntoSingleFrame(videoMessageReceived.GetPointer(), pDecodedPic);
          if(iRet)
          {
            totolFrameNumber ++;
            //igtl_uint32 dimension[2] = {Width, Height};
            //igtl_uint32 stride[2] = {Width, Width/2};
            //FILE* outFile = fopen("HDVideo.yuv", "ab");
            //videoStreamDecoder->Write2File(outFile, pDecodedPic->data, dimension, stride);
            //fclose(outFile);
            endDecodeTime = videoStreamDecoder->getCurrentTime();
            totalDecodeTime += (endDecodeTime - startDecodeTime);
            if(version == IGTL_HEADER_VERSION_2)
            {
              videoMessageReceived->SetMessageID(1); // Message ID is reset by the codec, so the comparison of ID is no longer valid, manually set to 1;
            }
            ssim += local_vpx_ssim2(pSrcPic->data[0], pDecodedPic->data[0],pSrcPic->stride[0], pDecodedPic->stride[0],pSrcPic->picWidth,pSrcPic->picHeight);
          }
        }
        igtl::Sleep(5);
      }
      if (pFileYUV)
      {
        fclose(pFileYUV);
        pFileYUV = NULL;
      }
    } else {
      fprintf (stderr, "Unable to open image file, check corresponding path!\n");
    }
    if(totolFrameNumber>=100)
    {
      break;
    }
  }
  delete pDecodedPic;
  delete pSrcPic;
  pDecodedPic = NULL;
  pSrcPic = NULL;
  float framePerSecondEncode = 1e6/((float)totalEncodeTime)*totolFrameNumber;
  float framePerSecondDecode = 1e6/((float)totalDecodeTime)*totolFrameNumber;
  ssim/=static_cast<float>(totolFrameNumber);
  std::cerr<<"SSIM Value: "<<ssim<<" Total Encoding Time: "<<totalEncodeTime/1e6<<std::endl;
  compressionRate = (float)(Width*Height*totolFrameNumber*3/2)/bitstreamTotalLength;
  std::cerr<<"Compression Ratio: "<<compressionRate<< std::endl;
  std::string localline = std::string("");
  localline.append(ToString(ssim));
  localline.append(" ");
  localline.append(ToString(compressionRate));
  localline.append(" ");
  localline.append(ToString(framePerSecondEncode));
  localline.append(" ");
  localline.append(ToString(framePerSecondDecode));
  localline.append("\r\n");
  fwrite(localline.c_str(), 1, localline.size(), pEval);
  fclose(pEval);
}

void H264CodecSpeedAndQualityEval()
{
#if OpenIGTLink_BUILD_H264
  H264Encoder* videoStreamEncoder = new H264Encoder();
  H264Decoder* videoStreamDecoder = new H264Decoder();
  videoStreamEncoder->SetPicWidthAndHeight(Width, Height);
  videoStreamEncoder->SetLosslessLink(true);
  videoStreamEncoder->SetQP(0,0);
  videoStreamEncoder->InitializeEncoder();
  TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder,false);
  float framePerSecondEncode = 1e6/((float)totalEncodeTime)*totolFrameNumber;
  float framePerSecondDecode = 1e6/((float)totalDecodeTime)*totolFrameNumber;
  std::cerr<<"Total encode and decode time for near lossless coding: "<<framePerSecondEncode<<",  "<<framePerSecondDecode<<std::endl;
  std::map<std::string, std::string> values, times;
  values.insert(std::pair<std::string, std::string>(ToString(ssim), ToString(compressionRate)));
  times.insert(std::pair<std::string, std::string>(ToString(framePerSecondEncode), ToString(framePerSecondDecode)));
  for (int i = 1; i<=51; i=i+3)
  {
    videoStreamEncoder->SetQP(i,i);
    videoStreamEncoder->InitializeEncoder();
    TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder, false);
    framePerSecondEncode = 1e6/((float)totalEncodeTime)*totolFrameNumber;
    framePerSecondDecode = 1e6/((float)totalDecodeTime)*totolFrameNumber;
    std::cerr<<"Total encode and decode frequency for target bitrate="<<i<<": "<<framePerSecondEncode<<",  "<<framePerSecondDecode<<std::endl;
    values.insert(std::pair<std::string, std::string>(ToString(ssim), ToString(compressionRate)));
    times.insert(std::pair<std::string, std::string>(ToString(framePerSecondEncode), ToString(framePerSecondDecode)));
  }
  std::map<std::string, std::string>::const_iterator it2 = times.begin();
  for( std::map<std::string, std::string>::const_iterator it = values.begin(); it != values.end(); ++it, ++it2)
  {
    std::cerr<<it->first<<" "<<it->second<<" "<<it2->first<<" "<<it2->second<<std::endl;
  }
  videoStreamDecoder->~H264Decoder();
  videoStreamEncoder->~H264Encoder();
#endif
}


void VP9CodecSpeedAndQualityEval()
{
#if OpenIGTLink_BUILD_VPX
  VPXEncoder* videoStreamEncoder = new VPXEncoder();
  VPXDecoder* videoStreamDecoder = new VPXDecoder();
  videoStreamEncoder->SetPicWidthAndHeight(Width, Height);
  videoStreamEncoder->SetLosslessLink(false);
  std::map<std::string, std::string> values, times;
  for (int i = 1; i<=63; i=i+3)
  {
    videoStreamEncoder->SetQP(i,i);
    videoStreamEncoder->InitializeEncoder();
    TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder, false);
    float framePerSecondEncode = 1e6/((float)totalEncodeTime)*totolFrameNumber;
    float framePerSecondDecode = 1e6/((float)totalDecodeTime)*totolFrameNumber;
    std::cerr<<"Total encode and decode frequency for target bitrate="<<i<<": "<<framePerSecondEncode<<",  "<<framePerSecondDecode<<std::endl;
    values.insert(std::pair<std::string, std::string>(ToString(ssim), ToString(compressionRate)));
    times.insert(std::pair<std::string, std::string>(ToString(framePerSecondEncode), ToString(framePerSecondDecode)));
  }
  std::map<std::string, std::string>::const_iterator it2 = times.begin();
  for( std::map<std::string, std::string>::const_iterator it = values.begin(); it != values.end(); ++it, ++it2)
  {
    std::cerr<<it->first<<" "<<it->second<<" "<<it2->first<<" "<<it2->second<<std::endl;
  }
  videoStreamDecoder->~VPXDecoder();
  videoStreamEncoder->~VPXEncoder();
#endif
}

void H264CodecSpeedAndRateEval()
{
#if OpenIGTLink_BUILD_H264
  H264Encoder* videoStreamEncoder = new H264Encoder();
  H264Decoder* videoStreamDecoder = new H264Decoder();
  videoStreamEncoder->SetPicWidthAndHeight(Width,Height);
  RC_MODES mode = RC_BITRATE_MODE;
  videoStreamEncoder->SetRCMode(mode); // bitrate mode
  std::map<std::string, std::string> values, times;
  for (int i = 1; i<=20; i++)
  {
    videoStreamEncoder->SetRCTaregetBitRate(Width*Height/100*8*20*i);
    videoStreamEncoder->InitializeEncoder();
    TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder, false);
    float framePerSecondEncode = 1e6/((float)totalEncodeTime)*totolFrameNumber;
    float framePerSecondDecode = 1e6/((float)totalDecodeTime)*totolFrameNumber;
    std::cerr<<"Total encode and decode frequency for target bitrate="<<i<<": "<<framePerSecondEncode<<",  "<<framePerSecondDecode<<std::endl;
    values.insert(std::pair<std::string, std::string>(ToString(ssim), ToString(compressionRate)));
    times.insert(std::pair<std::string, std::string>(ToString(framePerSecondEncode), ToString(framePerSecondDecode)));
  }
  std::map<std::string, std::string>::const_iterator it2 = times.begin();
  for( std::map<std::string, std::string>::const_iterator it = values.begin(); it != values.end(); ++it, ++it2)
  {
    std::cerr<<it->first<<" "<<it->second<<" "<<it2->first<<" "<<it2->second<<std::endl;
  }
  videoStreamDecoder->~H264Decoder();
  videoStreamEncoder->~H264Encoder();
#endif
}

void VP9CodecSpeedAndRateEval()
{
#if OpenIGTLink_LINK_VP9
  VP9Encoder* videoStreamEncoder = new VP9Encoder();
  VP9Decoder* videoStreamDecoder = new VP9Decoder();
  videoStreamEncoder->SetPicWidthAndHeight(Width, Height);
  videoStreamEncoder->SetLosslessLink(true);
  videoStreamEncoder->SetRCMode(1); // 1 is VPX_CBR
  videoStreamEncoder->InitializeEncoder();
  std::cerr<<"initialized"<<std::endl;
  //TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder,false);
  float framePerSecondEncode = 1e6/((float)totalEncodeTime)*totolFrameNumber;
  float framePerSecondDecode = 1e6/((float)totalDecodeTime)*totolFrameNumber;
  std::cerr<<"Total encode and decode frequency for lossless coding: "<<framePerSecondEncode<<",  "<<framePerSecondDecode<<std::endl;
  std::map<std::string, std::string> values, times;
  values.insert(std::pair<std::string, std::string>(ToString(ssim), ToString(compressionRate)));
  times.insert(std::pair<std::string, std::string>(ToString(framePerSecondEncode), ToString(framePerSecondDecode)));
  videoStreamEncoder->SetLosslessLink(false);
  videoStreamEncoder->SetRCMode(1); // 1 is VPX_CBR
  for (int i = 1; i<=20; i++) // The original frame bits per second is 256*256*20*8, the compression ratio is set from 0.5% to 8%
  {
    videoStreamEncoder->SetRCTaregetBitRate((int)(Width*Height/100*8*20*i));
    videoStreamEncoder->InitializeEncoder();
    videoStreamEncoder->SetSpeed(8);
    TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder, false);
    framePerSecondEncode = 1e6/((float)totalEncodeTime)*totolFrameNumber;
    framePerSecondDecode = 1e6/((float)totalDecodeTime)*totolFrameNumber;
    std::cerr<<"Total encode and decode frequency for target bitrate="<<i<<": "<<framePerSecondEncode<<",  "<<framePerSecondDecode<<std::endl;
    values.insert(std::pair<std::string, std::string>(ToString(ssim), ToString(compressionRate)));
    times.insert(std::pair<std::string, std::string>(ToString(framePerSecondEncode), ToString(framePerSecondDecode)));
  }
  std::map<std::string, std::string>::const_iterator it2 = times.begin();
  for( std::map<std::string, std::string>::const_iterator it = values.begin(); it != values.end(); ++it, ++it2)
  {
    std::cerr<<it->first<<" "<<it->second<<" "<<it2->first<<" "<<it2->second<<std::endl;
  }
  videoStreamDecoder->~VP9Decoder();
  videoStreamEncoder->~VP9Encoder();
#endif
}

void CodecOptimizationEval()
{
    std::string title;
    pEval = fopen (evalFileName.c_str(), "a");
    title = "H264CodecSpeedAndQualityEval\r\n";
    fwrite(title.c_str(),1, title.size(),pEval);
    fclose(pEval);
    H264CodecSpeedAndQualityEval();
    pEval = fopen (evalFileName.c_str(), "a");
    title = "VP9CodecSpeedAndQualityEval\r\n";
    fwrite(title.c_str(),1, title.size(),pEval);
    fclose(pEval);
    VP9CodecSpeedAndQualityEval();
    pEval = fopen (evalFileName.c_str(), "a");
    title = "H264CodecSpeedAndRateEval\r\n";
    fwrite(title.c_str(),1, title.size(),pEval);
    fclose(pEval);
    H264CodecSpeedAndRateEval();
    pEval = fopen (evalFileName.c_str(), "a");
    title = "VP9CodecSpeedAndRateEval\r\n";
    fwrite(title.c_str(),1, title.size(),pEval);
    fclose(pEval);
    VP9CodecSpeedAndRateEval();
}

void VP9SpeedEvaluation()
{
  for (int speed = 0; speed<=8;speed=speed+2)
  {
#if OpenIGTLink_LINK_VP9
      std::map<std::string, std::string> values, times;
      VP9Encoder* videoStreamEncoder = new VP9Encoder();
      VP9Decoder* videoStreamDecoder = new VP9Decoder();
      videoStreamEncoder->SetPicWidthAndHeight(Width, Height);
      videoStreamEncoder->SetLosslessLink(false);
      videoStreamEncoder->SetRCMode(1); // 1 is VPX_CBR
      float percents[5] ={0.01, 0.02, 0.04, 0.06, 0.09};
      for(int k = 0; k<19; k++)
      {
        startIndex = 2351 + k*100;
        pEval = fopen (evalFileName.c_str(), "a");
        std::string title = "VP9CodecSpeedAndRateEvalWithSpeed-";
        title.append(ToString(speed)).append("\r\n");
        fwrite(title.c_str(),1, title.size(),pEval);
        fclose(pEval);
        for (int j = 0; j<5; j=j+1) // The original frame bits per second is 256*256*20*8, the compression ratio is set from 0.5% to 8%
        {
          videoStreamEncoder->SetRCTaregetBitRate((int)(Width*Height*8*20*percents[j]));
          videoStreamEncoder->InitializeEncoder();
          videoStreamEncoder->SetSpeed(speed);
          
          TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder, false);
          float framePerSecondEncode = 1e6/((float)totalEncodeTime)*totolFrameNumber;
          float framePerSecondDecode = 1e6/((float)totalDecodeTime)*totolFrameNumber;
          std::cerr<<"Total encode and decode frequency for target bitrate="<<j<<": "<<framePerSecondEncode<<",  "<<framePerSecondDecode<<std::endl;
          values.insert(std::pair<std::string, std::string>(ToString(ssim), ToString(compressionRate)));
          times.insert(std::pair<std::string, std::string>(ToString(framePerSecondEncode), ToString(framePerSecondDecode)));
        }
      }
      std::map<std::string, std::string>::const_iterator it2 = times.begin();
      for( std::map<std::string, std::string>::const_iterator it = values.begin(); it != values.end(); ++it, ++it2)
      {
        std::cerr<<it->first<<" "<<it->second<<" "<<it2->first<<" "<<it2->second<<std::endl;
      }
      videoStreamDecoder->~VP9Decoder();
      videoStreamEncoder->~VP9Encoder();
#endif
  }
}


void H265SpeedEvaluation()
{
  for (int speed = 1; speed<=9;speed=speed+2)
  {
#if OpenIGTLink_LINK_X265
    float percents[5] ={0.01, 0.02, 0.04, 0.06, 0.09};
    std::map<std::string, std::string> values, times;
    int BitRateFactor = 7;
    for(int k = 0; k<19; k++)
    {
      startIndex = 2351 + k*100;
      pEval = fopen (evalFileName.c_str(), "a");
      std::string title = "H265CodecSpeedAndRateEvalWithSpeed-";
      title.append(ToString(speed)).append("\r\n");
      fwrite(title.c_str(),1, title.size(),pEval);
      fclose(pEval);
      for (int j = 0; j<5; j=j+1) // The original frame bits per second is 256*256*20*8, the compression ratio is set from 0.5% to 8%
      {
      H265Encoder* videoStreamEncoder = new H265Encoder();
      H265Decoder* videoStreamDecoder = new H265Decoder();
      videoStreamEncoder->SetPicWidthAndHeight(Width, Height);
      videoStreamEncoder->SetLosslessLink(false);
      videoStreamEncoder->SetRCMode(1); // 1 is VPX_CBR
      videoStreamEncoder->SetRCTaregetBitRate((int)(Width*Height*8*20*percents[j])*BitRateFactor);
      videoStreamEncoder->InitializeEncoder();
      videoStreamEncoder->SetSpeed(speed);
      TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder, false);
      float framePerSecondEncode = 1e6/((float)totalEncodeTime)*totolFrameNumber;
      float framePerSecondDecode = 1e6/((float)totalDecodeTime)*totolFrameNumber;
      std::cerr<<"Total encode and decode frequency for target bitrate="<<j<<": "<<framePerSecondEncode<<",  "<<framePerSecondDecode<<std::endl;
      values.insert(std::pair<std::string, std::string>(ToString(ssim), ToString(compressionRate)));
      times.insert(std::pair<std::string, std::string>(ToString(framePerSecondEncode), ToString(framePerSecondDecode)));
      videoStreamDecoder->~H265Decoder();
      videoStreamEncoder->~H265Encoder();
      }
    }
    std::map<std::string, std::string>::const_iterator it2 = times.begin();
    for( std::map<std::string, std::string>::const_iterator it = values.begin(); it != values.end(); ++it, ++it2)
    {
      std::cerr<<it->first<<" "<<it->second<<" "<<it2->first<<" "<<it2->second<<std::endl;
    }
    
#endif
  }
}


void H264SpeedEvaluation()
{
  for (int speed = 0; speed<3;speed=speed+1)
  {
    for (int i = 0; i<22; i=i+2)
    {
#if OpenIGTLink_LINK_H264
      startIndex = i*100;
      pEval = fopen (evalFileName.c_str(), "a");
      std::string title = "H264CodecSpeedAndRateEvalWithSpeed-";
      title.append(ToString(speed)).append("\r\n");
      fwrite(title.c_str(),1, title.size(),pEval);
      fclose(pEval);
      std::map<std::string, std::string> values, times;
      H264Encoder* videoStreamEncoder = new H264Encoder();
      H264Decoder* videoStreamDecoder = new H264Decoder();
      videoStreamEncoder->SetPicWidthAndHeight(Width, Height);
      videoStreamEncoder->SetLosslessLink(false);
      videoStreamEncoder->SetSpeed(speed);
      videoStreamEncoder->SetRCMode(1); // 1 is VPX_CBR
      int BitRateFactor = 3;
      for (int j = 1; j<=20; j=j+4) // The original frame bits per second is 255*255*20*8, the compression ratio is set from 0.5% to 8%
      {
        videoStreamEncoder->SetRCTaregetBitRate((int)(Width*Height/100*8*20*j)*BitRateFactor);
        videoStreamEncoder->InitializeEncoder();
        TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder, false);
        float framePerSecondEncode = 1e6/((float)totalEncodeTime)*totolFrameNumber;
        float framePerSecondDecode = 1e6/((float)totalDecodeTime)*totolFrameNumber;
        std::cerr<<"Total encode and decode frequency for target bitrate="<<j<<": "<<framePerSecondEncode<<",  "<<framePerSecondDecode<<std::endl;
        values.insert(std::pair<std::string, std::string>(ToString(ssim), ToString(compressionRate)));
        times.insert(std::pair<std::string, std::string>(ToString(framePerSecondEncode), ToString(framePerSecondDecode)));
      }
      std::map<std::string, std::string>::const_iterator it2 = times.begin();
      for( std::map<std::string, std::string>::const_iterator it = values.begin(); it != values.end(); ++it, ++it2)
      {
        std::cerr<<it->first<<" "<<it->second<<" "<<it2->first<<" "<<it2->second<<std::endl;
      }
      videoStreamDecoder->~H264Decoder();
      videoStreamEncoder->~H264Encoder();
#endif
    }
  }
}


int main(int argc, char **argv)
{
  //H264SpeedEvaluation();
  //VP9SpeedEvaluation();
  H265SpeedEvaluation();
  //CodecOptimizationEval();
  return 0;
}

