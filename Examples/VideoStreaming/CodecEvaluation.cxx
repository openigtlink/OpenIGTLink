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

#if OpenIGTLink_BUILD_H264
#include "H264Encoder.h"
#include "H264Decoder.h"
#endif

#if OpenIGTLink_BUILD_VPX
#include "VPXEncoder.h"
#include "VPXDecoder.h"
#include "../video_reader.h"
#include "./vpx_config.h"
#include "vpx_dsp_rtcd.h"
#include "vpx_dsp/ssim.h"
#endif

int startEncodeTime = 0;
int endEncodeTime = 0;
int startDecodeTime = 0;
int endDecodeTime = 0;
int totalEncodeTime = 0;
int totalDecodeTime =0;
double ssim = 0.0;
float compressionRate = 0.0;

//int Width = 256;
//int Height = 256;
//std::string testFileName(OpenIGTLink_SOURCE_ROOTDIR);
int Width = 1920;
int Height = 1080;
std::string testFileName("/Users/longquanchen/Documents/VideoStreaming/RoboticHysterectomy.yuv");
std::string evalFileName("EvalFile.txt");
FILE* pEval = NULL;
int startIndex = 100;
int  inputFrameNum = 100;

template <typename T>
std::string ToString(T variable)
{
  stringstream stream;
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
    assert(0);
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
  int totalFrame = 0;
  pEval = fopen (evalFileName.c_str(), "a");
  for(int i = 0; i <inputFrameNum; i++)
  {
    igtl_int32 iFrameNumInFile = -1;
    std::string sep = "/";
#if defined(_WIN32) || defined(_WIN64)
    sep = "\\";
#endif
    std::string imageIndexStr = static_cast< std::ostringstream & >(( std::ostringstream() << std::dec << (i%6+1))).str();
    std::string testIndexedFileName = std::string(testFileName);
    //testIndexedFileName.append(sep).append("Testing").append(sep).append("img").append(sep).append("igtlTestImage").append(imageIndexStr).append(".raw");
    FILE* pFileYUV = NULL;
    pFileYUV = fopen (testIndexedFileName.c_str(), "rb");
    if (pFileYUV != NULL) {
      igtl_int64 i_size = 0;
#if defined(_WIN32) || defined(_WIN64)
#if _MSC_VER >= 1400
      if (!_fseeki64 (pFileYUV, 0, SEEK_END)) {
        i_size = _ftelli64 (pFileYUV);
        _fseeki64 (pFileYUV, 0, SEEK_SET);
        iFrameNumInFile = std::max((igtl_int32) (i_size / kiPicResSize), iFrameNumInFile);
      }
#else
      if (!fseek (pFileYUV, 0, SEEK_END)) {
        i_size = ftell (pFileYUV);
        fseek (pFileYUV, 0, SEEK_SET);
        iFrameNumInFile = std::max((igtl_int32) (i_size / kiPicResSize), iFrameNumInFile);
      }
#endif
#else
      if (!fseeko (pFileYUV, 0, SEEK_END)) {
        i_size = ftello (pFileYUV);
        fseek(pFileYUV, 0, SEEK_SET);
        iFrameNumInFile = std::max((igtl_int32) (i_size / (kiPicResSize*3/2)), iFrameNumInFile);
      }
#endif
#if defined(_WIN32) || defined(_WIN64)
#if _MSC_VER >= 1400
      _fseeki64(pFileSeek, startIndex*kiPicResSize*3/2, SEEK_SET); //fix the data range issue
#else
      fseek(pFileSeek, startIndex*kiPicResSize*3/2, SEEK_SET); //fix the data range issue
#endif
#else
      long pos = startIndex/100*kiPicResSize*3/2; // for large file, long type data is in the range of -2,147,483,648	2,147,483,647
      for(int i = 0;i<100;i++)
      {
        fseek(pFileYUV, pos, SEEK_CUR);
      }
#endif
      while(fread (imagePointer, 1, kiPicResSize*3/2, pFileYUV ) == (kiPicResSize*3/2) && totalFrame<inputFrameNum)
      {
        bool isGrayImage = false;
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
          videoMessageReceived->Unpack();
          bitstreamTotalLength += videoMessageReceived->GetBitStreamSize();
          startDecodeTime = videoStreamDecoder->getCurrentTime();
          int iRet= videoStreamDecoder->DecodeVideoMSGIntoSingleFrame(videoMessageReceived.GetPointer(), pDecodedPic);
          if(iRet)
          {
            totalFrame ++;
            //igtl_uint32 dimension[2] = {Width, Height};
            //igtl_uint32 stride[2] = {Width, Width/2};
            //FILE* outFile = fopen("HDVideo.yuv", "a");
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
      if(totalFrame>=inputFrameNum)
      {
        break;
      }
    } else {
      fprintf (stderr, "Unable to open image file, check corresponding path!\n");
    }
  }
  delete pDecodedPic;
  delete pSrcPic;
  pDecodedPic = NULL;
  pSrcPic = NULL;
  float framePerSecondEncode = 1e6/((float)totalEncodeTime)*inputFrameNum;
  float framePerSecondDecode = 1e6/((float)totalDecodeTime)*inputFrameNum;
  ssim/=static_cast<float>(inputFrameNum);
  std::cerr<<"SSIM Value: "<<ssim<<" Total Encoding Time: "<<totalEncodeTime/1e6<<std::endl;
  compressionRate = (float)bitstreamTotalLength/(Width*Height*totalFrame);
  std::cerr<<"Compression Ratio: "<<compressionRate<< "  Total Frame Number: "<< totalFrame<< std::endl;
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
  float framePerSecondEncode = 1e6/((float)totalEncodeTime)*inputFrameNum;
  float framePerSecondDecode = 1e6/((float)totalDecodeTime)*inputFrameNum;
  std::cerr<<"Total encode and decode time for near lossless coding: "<<framePerSecondEncode<<",  "<<framePerSecondDecode<<std::endl;
  std::map<std::string, std::string> values, times;
  values.insert(std::pair<std::string, std::string>(ToString(ssim), ToString(compressionRate)));
  times.insert(std::pair<std::string, std::string>(ToString(framePerSecondEncode), ToString(framePerSecondDecode)));
  for (int i = 1; i<=51; i=i+3)
  {
    videoStreamEncoder->SetQP(i,i);
    videoStreamEncoder->InitializeEncoder();
    TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder, false);
    framePerSecondEncode = 1e6/((float)totalEncodeTime)*inputFrameNum;
    framePerSecondDecode = 1e6/((float)totalDecodeTime)*inputFrameNum;
    std::cerr<<"Total encode and decode frequency for target bitrate="<<i<<": "<<framePerSecondEncode<<",  "<<framePerSecondDecode<<std::endl;
    values.insert(std::pair<std::string, std::string>(ToString(ssim), ToString(compressionRate)));
    times.insert(std::pair<std::string, std::string>(ToString(framePerSecondEncode), ToString(framePerSecondDecode)));
  }
  std::map<std::string, std::string>::const_iterator it2 = times.begin();
  for( std::map<std::string, std::string>::const_iterator it = values.begin(); it != values.end(); ++it, ++it2)
  {
    std::cerr<<it->first<<" "<<it->second<<" "<<it2->first<<" "<<it2->second<<std::endl;
  }
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
    float framePerSecondEncode = 1e6/((float)totalEncodeTime)*inputFrameNum;
    float framePerSecondDecode = 1e6/((float)totalDecodeTime)*inputFrameNum;
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
    videoStreamEncoder->SetRCTaregetBitRate(2073600/100*8*20*i);
    videoStreamEncoder->InitializeEncoder();
    TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder, false);
    float framePerSecondEncode = 1e6/((float)totalEncodeTime)*inputFrameNum;
    float framePerSecondDecode = 1e6/((float)totalDecodeTime)*inputFrameNum;
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
#if OpenIGTLink_BUILD_VPX
  VPXEncoder* videoStreamEncoder = new VPXEncoder();
  VPXDecoder* videoStreamDecoder = new VPXDecoder();
  videoStreamEncoder->SetPicWidthAndHeight(Width, Height);
  videoStreamEncoder->SetLosslessLink(true);
  videoStreamEncoder->SetRCMode(1); // 1 is VPX_CBR
  videoStreamEncoder->InitializeEncoder();
  std::cerr<<"initialized"<<std::endl;
  //TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder,false);
  float framePerSecondEncode = 1e6/((float)totalEncodeTime)*inputFrameNum;
  float framePerSecondDecode = 1e6/((float)totalDecodeTime)*inputFrameNum;
  std::cerr<<"Total encode and decode frequency for lossless coding: "<<framePerSecondEncode<<",  "<<framePerSecondDecode<<std::endl;
  std::map<std::string, std::string> values, times;
  values.insert(std::pair<std::string, std::string>(ToString(ssim), ToString(compressionRate)));
  times.insert(std::pair<std::string, std::string>(ToString(framePerSecondEncode), ToString(framePerSecondDecode)));
  videoStreamEncoder->SetLosslessLink(false);
  videoStreamEncoder->SetRCMode(1); // 1 is VPX_CBR
  for (int i = 1; i<=20; i++) // The original frame bits per second is 255*255*20*8, the compression ratio is set from 0.5% to 8%
  {
    videoStreamEncoder->SetRCTaregetBitRate((int)(2073600/100*8*20*i));
    videoStreamEncoder->InitializeEncoder();
    videoStreamEncoder->SetSpeed(8);
    TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder, false);
    framePerSecondEncode = 1e6/((float)totalEncodeTime)*inputFrameNum;
    framePerSecondDecode = 1e6/((float)totalDecodeTime)*inputFrameNum;
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

void CodecOptimizationEval()
{
  for (int i = 0; i<22; i++)
  {
    startIndex = i*100;
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
    title = "H264CodecSpeedAndRateEval-";
    title.append(ToString(i)).append("\r\n");
    fwrite(title.c_str(),1, title.size(),pEval);
    fclose(pEval);
    H264CodecSpeedAndRateEval();
    pEval = fopen (evalFileName.c_str(), "a");
    title = "VP9CodecSpeedAndRateEval-";
    title.append(ToString(i)).append("\r\n");
    fwrite(title.c_str(),1, title.size(),pEval);
    fclose(pEval);
    VP9CodecSpeedAndRateEval();
  }
}

void VP9SpeedEvaluation()
{
  for (int speed = 0; speed<=8;speed=speed+2)
  {
    for (int i = 0; i<22; i=i+2)
    {
      startIndex = i*100;
      pEval = fopen (evalFileName.c_str(), "a");
      std::string title = "VP9CodecSpeedAndRateEvalWithSpeed-";
      title.append(ToString(speed)).append("\r\n");
      fwrite(title.c_str(),1, title.size(),pEval);
      fclose(pEval);
      std::map<std::string, std::string> values, times;
      VPXEncoder* videoStreamEncoder = new VPXEncoder();
      VPXDecoder* videoStreamDecoder = new VPXDecoder();
      videoStreamEncoder->SetPicWidthAndHeight(Width, Height);
      videoStreamEncoder->SetLosslessLink(false);
      videoStreamEncoder->SetRCMode(1); // 1 is VPX_CBR
      for (int i = 1; i<=20; i=i+4) // The original frame bits per second is 255*255*20*8, the compression ratio is set from 0.5% to 8%
      {
        videoStreamEncoder->SetRCTaregetBitRate((int)(2073600/100*8*20*i));
        videoStreamEncoder->InitializeEncoder();
        videoStreamEncoder->SetSpeed(speed);
        TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder, false);
        float framePerSecondEncode = 1e6/((float)totalEncodeTime)*inputFrameNum;
        float framePerSecondDecode = 1e6/((float)totalDecodeTime)*inputFrameNum;
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
    }
  }
}

int main(int argc, char **argv)
{
  VP9SpeedEvaluation();
  CodecOptimizationEval();
  return 0;
}

