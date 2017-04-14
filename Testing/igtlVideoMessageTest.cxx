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
#include "igtlTestConfig.h"
#include "string.h"
#include <cstring>
#include <string>
#if OpenIGTLink_PROTOCOL_VERSION >= 3
  #include "igtlMessageFormat2TestMacro.h"
#endif

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

static double local_ssim_8x8(const uint8_t *s, int sp, const uint8_t *r, int rp) {
  uint32_t sum_s = 0, sum_r = 0, sum_sq_s = 0, sum_sq_r = 0, sum_sxr = 0;
  vpx_ssim_parms_8x8_c(s, sp, r, rp, &sum_s, &sum_r, &sum_sq_s, &sum_sq_r,
                     &sum_sxr);
  return similarity(sum_s, sum_r, sum_sq_s, sum_sq_r, sum_sxr, 64, 8);
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
      double v = local_ssim_8x8(img1 + j, stride_img1, img2 + j, stride_img2);
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
  int Width = 256;
  int Height = 256;
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
  int totalFrame = 0;
  for(int i = 0; i <100; i++)
  {
    igtl_int32 iFrameNumInFile = -1;
    std::string sep = "/";
#if defined(_WIN32) || defined(_WIN64)
    sep = "\\";
#endif
    std::string testFileName(OpenIGTLink_SOURCE_ROOTDIR);
    std::string imageIndexStr = static_cast< std::ostringstream & >(( std::ostringstream() << std::dec << (i%6+1))).str();
    testFileName.append(sep).append("Testing").append(sep).append("img").append(sep).append("igtlTestImage").append(imageIndexStr).append(".raw");
    FILE* pFileYUV = NULL;
    pFileYUV = fopen (testFileName.c_str(), "rb");
    if (pFileYUV != NULL) {
#if defined(_WIN32) || defined(_WIN64)
#if _MSC_VER >= 1400
      if (!_fseeki64 (pFileYUV, 0, SEEK_END)) {
        igtl_int64 i_size = _ftelli64 (pFileYUV);
        _fseeki64 (pFileYUV, 0, SEEK_SET);
        iFrameNumInFile = std::max((igtl_int32) (i_size / kiPicResSize), iFrameNumInFile);
      }
#else
      if (!fseek (pFileYUV, 0, SEEK_END)) {
        igtl_int64 i_size = ftell (pFileYUV);
        fseek (pFileYUV, 0, SEEK_SET);
        iFrameNumInFile = std::max((igtl_int32) (i_size / kiPicResSize), iFrameNumInFile);
      }
#endif
#else
      if (!fseeko (pFileYUV, 0, SEEK_END)) {
        igtl_int64 i_size = ftello (pFileYUV);
        fseeko (pFileYUV, 0, SEEK_SET);
        iFrameNumInFile = std::max((igtl_int32) (i_size / (kiPicResSize)), iFrameNumInFile);
      }
#endif
      
#if defined(_WIN32) || defined(_WIN64)
#if _MSC_VER >= 1400
      _fseeki64(pFileYUV, 0, SEEK_SET);
#else
      fseek(pFileYUV, 0, SEEK_SET);
#endif
#else
      fseeko(pFileYUV, 0, SEEK_SET);
#endif
      while(fread (imagePointer, 1, kiPicResSize, pFileYUV) == kiPicResSize)
      {
        bool isGrayImage = true;
        igtl::VideoMessage::Pointer videoMessageSend = igtl::VideoMessage::New();
        videoMessageSend->SetHeaderVersion(version);
        if(version == IGTL_HEADER_VERSION_2)
          igtlMetaDataAddElementMacro(videoMessageSend);
        startEncodeTime = videoStreamDecoder->getCurrentTime();
        int iEncFrames = videoStreamEncoder->EncodeSingleFrameIntoVideoMSG(pSrcPic, videoMessageSend, isGrayImage);
        endEncodeTime = videoStreamDecoder->getCurrentTime();
        totalEncodeTime += (endEncodeTime - startEncodeTime);
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
          EXPECT_EQ(memcmp(videoMessageReceived->GetPackFragmentPointer(2),videoMessageSend->GetPackFragmentPointer(2),videoMessageReceived->GetBitStreamSize()),0);
          startDecodeTime = videoStreamDecoder->getCurrentTime();
          int iRet= videoStreamDecoder->DecodeVideoMSGIntoSingleFrame(videoMessageReceived.GetPointer(), pDecodedPic);
          if(iRet)
          {
            totalFrame ++;
            endDecodeTime = videoStreamDecoder->getCurrentTime();
            totalDecodeTime += (endDecodeTime - startDecodeTime);
            if(version == IGTL_HEADER_VERSION_2)
            {
              videoMessageReceived->SetMessageID(1); // Message ID is reset by the codec, so the comparison of ID is no longer valid, manually set to 1;
              igtlMetaDataComparisonMacro(videoMessageReceived);
            }
  #if OpenIGTLink_BUILD_VPX
            /*YV12_BUFFER_CONFIG *source = new YV12_BUFFER_CONFIG();
            YV12_BUFFER_CONFIG *dest = new YV12_BUFFER_CONFIG();
            double *weight = new double(); *weight = 1.0;
            source->y_buffer = pSrcPic->data[0];
            dest->y_buffer = pDecodedPic->data[0];
            source->y_stride = pSrcPic->stride[0];
            source->uv_stride =pSrcPic->stride[1];
            dest->y_stride = pDecodedPic->stride[0];
            dest->uv_stride = pDecodedPic->stride[1];
            source->y_crop_width = pSrcPic->picWidth;
            source->y_crop_height = pSrcPic->picHeight;*/
            ssim += local_vpx_ssim2(pSrcPic->data[0], pDecodedPic->data[0],pSrcPic->stride[0], pDecodedPic->stride[0],pSrcPic->picWidth,pSrcPic->picHeight);
  #endif
            if(compareImage)
              EXPECT_EQ(memcmp(pDecodedPic->data[0], pSrcPic->data[0],kiPicResSize*3/2),0);
          }
        }
      }
      igtl::Sleep(5);
      if (pFileYUV)
      {
        fclose(pFileYUV);
        pFileYUV = NULL;
      }
    } else {
      fprintf (stderr, "Unable to open source sequence file, check corresponding path!\n");
    }
  }
  delete pDecodedPic;
  delete pSrcPic;
  pDecodedPic = NULL;
  pSrcPic = NULL;
  ssim/=100.0;
  std::cerr<<"SSIM Value: "<<ssim<<" Total Encoding Time: "<<totalEncodeTime/1e6<<std::endl;
  compressionRate = (float)bitstreamTotalLength/(Width*Height*totalFrame);
  std::cerr<<"Compression Ratio: "<<compressionRate<< "  Total Frame Number: "<< totalFrame<< std::endl;
}

  void H264CodecSpeedAndQualityEval()
  {
    #if OpenIGTLink_BUILD_H264
      H264Encoder* videoStreamEncoder = new H264Encoder();
      H264Decoder* videoStreamDecoder = new H264Decoder();
      int Width = 256;
      int Height = 256;
      videoStreamEncoder->SetPicWidth(Width);
      videoStreamEncoder->SetPicHeight(Height);
      videoStreamEncoder->SetLosslessLink(true);
      videoStreamEncoder->SetQP(0,0);
      videoStreamEncoder->InitializeEncoder();
      TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder,false);
      std::cerr<<"Total encode and decode time for near lossless coding: "<<(float)totalEncodeTime/1e6<<",  "<<(float)totalDecodeTime/1e6<<std::endl;
      std::map<std::string, std::string> values, times;
      values.insert(std::pair<std::string, std::string>(std::to_string(ssim), std::to_string(compressionRate)));
      times.insert(std::pair<std::string, std::string>(std::to_string((float)totalEncodeTime/1e6), std::to_string((float)totalDecodeTime/1e6)));
      for (int i = 1; i<=51; i=i+3)
      {
        videoStreamEncoder->SetQP(i,i);
        videoStreamEncoder->InitializeEncoder();
        TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder, false);
        std::cerr<<"Total encode and decode time for QP="<<i<<": "<<(float)totalEncodeTime/1e6<<",  "<<(float)totalDecodeTime/1e6<<std::endl;
        values.insert(std::pair<std::string, std::string>(std::to_string(ssim), std::to_string(compressionRate)));
        times.insert(std::pair<std::string, std::string>(std::to_string((float)totalEncodeTime/1e6), std::to_string((float)totalDecodeTime/1e6)));
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
    int Width = 256;
    int Height = 256;
    videoStreamEncoder->SetPicWidth(Width);
    videoStreamEncoder->SetPicHeight(Height);
    videoStreamEncoder->SetLosslessLink(false);
    videoStreamEncoder->InitializeEncoder();
    TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder, false);
    std::cerr<<"Total encode and decode time for Variable Rate coding: "<<(float)totalEncodeTime/1e6<<",  "<<(float)totalDecodeTime/1e6<<std::endl;
    std::cerr<<"Total encode and decode time for near lossless coding: "<<(float)totalEncodeTime/1e6<<",  "<<(float)totalDecodeTime/1e6<<std::endl;
    std::map<std::string, std::string> values, times;
    values.insert(std::pair<std::string, std::string>(std::to_string(ssim), std::to_string(compressionRate)));
    times.insert(std::pair<std::string, std::string>(std::to_string((float)totalEncodeTime/1e6), std::to_string((float)totalDecodeTime/1e6)));
    for (int i = 1; i<=63; i=i+3)
    {
      videoStreamEncoder->SetQP(i,i);
      videoStreamEncoder->InitializeEncoder();
      TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder, false);
      std::cerr<<"Total encode and decode time for QP="<<i<<": "<<(float)totalEncodeTime/1e6<<",  "<<(float)totalDecodeTime/1e6<<std::endl;
      values.insert(std::pair<std::string, std::string>(std::to_string(ssim), std::to_string(compressionRate)));
      times.insert(std::pair<std::string, std::string>(std::to_string((float)totalEncodeTime/1e6), std::to_string((float)totalDecodeTime/1e6)));
    }
    videoStreamEncoder->SetLosslessLink(true);
    videoStreamEncoder->InitializeEncoder();
    TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder,true);
    std::cerr<<"Total encode and decode time for lossless coding: "<<(float)totalEncodeTime/1e6<<",  "<<(float)totalDecodeTime/1e6<<std::endl;
    std::map<std::string, std::string>::const_iterator it2 = times.begin();
    for( std::map<std::string, std::string>::const_iterator it = values.begin(); it != values.end(); ++it, ++it2)
    {
      std::cerr<<it->first<<" "<<it->second<<" "<<it2->first<<" "<<it2->second<<std::endl;
    }
  #endif
  }

  void H264CodecSpeedAndRateEval()
  {
  #if OpenIGTLink_BUILD_H264
    H264Encoder* videoStreamEncoder = new H264Encoder();
    H264Decoder* videoStreamDecoder = new H264Decoder();
    int Width = 256;
    int Height = 256;
    videoStreamEncoder->SetPicWidth(Width);
    videoStreamEncoder->SetPicHeight(Height);
    videoStreamEncoder->SetLosslessLink(false);
    videoStreamEncoder->SetRCMode(1); // bitrate mode
    videoStreamEncoder->InitializeEncoder();
    TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder,false);
    std::cerr<<"Total encode and decode time for near lossless coding: "<<(float)totalEncodeTime/1e6<<",  "<<(float)totalDecodeTime/1e6<<std::endl;
    std::map<std::string, std::string> values, times;
    values.insert(std::pair<std::string, std::string>(std::to_string(ssim), std::to_string(compressionRate)));
    times.insert(std::pair<std::string, std::string>(std::to_string((float)totalEncodeTime/1e6), std::to_string((float)totalDecodeTime/1e6)));
    for (int i = 1; i<=20; i++)
    {
      videoStreamEncoder->SetRCTaregetBitRate(655*8*20*i);
      videoStreamEncoder->InitializeEncoder();
      TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder, false);
      std::cerr<<"Total encode and decode time for target rate ="<<i<<": "<<(float)totalEncodeTime/1e6<<",  "<<(float)totalDecodeTime/1e6<<std::endl;
      values.insert(std::pair<std::string, std::string>(std::to_string(ssim), std::to_string(compressionRate)));
      times.insert(std::pair<std::string, std::string>(std::to_string((float)totalEncodeTime/1e6), std::to_string((float)totalDecodeTime/1e6)));
    }
    std::map<std::string, std::string>::const_iterator it2 = times.begin();
    for( std::map<std::string, std::string>::const_iterator it = values.begin(); it != values.end(); ++it, ++it2)
    {
      std::cerr<<it->first<<" "<<it->second<<" "<<it2->first<<" "<<it2->second<<std::endl;
    }
  #endif
  }

  void VP9CodecSpeedAndRateEval()
  {
  #if OpenIGTLink_BUILD_VPX
    VPXEncoder* videoStreamEncoder = new VPXEncoder();
    VPXDecoder* videoStreamDecoder = new VPXDecoder();
    int Width = 256;
    int Height = 256;
    videoStreamEncoder->SetPicWidth(Width);
    videoStreamEncoder->SetPicHeight(Height);
    videoStreamEncoder->SetLosslessLink(true);
    videoStreamEncoder->SetRCMode(1); // 1 is VPX_CBR
    videoStreamEncoder->InitializeEncoder();
    TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder,false);
    std::cerr<<"Total encode and decode time for lossless coding: "<<(float)totalEncodeTime/1e6<<",  "<<(float)totalDecodeTime/1e6<<std::endl;
    std::map<std::string, std::string> values, times;
    values.insert(std::pair<std::string, std::string>(std::to_string(ssim), std::to_string(compressionRate)));
    times.insert(std::pair<std::string, std::string>(std::to_string((float)totalEncodeTime/1e6), std::to_string((float)totalDecodeTime/1e6)));
    videoStreamEncoder->SetLosslessLink(false);
    
    for (int i = 1; i<=20; i++) // The original frame bits per second is 255*255*20*8, the compression ratio is set from 0.5% to 8%
    {
      videoStreamEncoder->SetRCTaregetBitRate((int)(65536/100*8*20*i/1000.0));
      videoStreamEncoder->InitializeEncoder();
      videoStreamEncoder->SetSpeed(8);
      TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder, false);
      std::cerr<<"Total encode and decode time for target bitrate="<<i<<": "<<(float)totalEncodeTime/1e6<<",  "<<(float)totalDecodeTime/1e6<<std::endl;
      values.insert(std::pair<std::string, std::string>(std::to_string(ssim), std::to_string(compressionRate)));
      times.insert(std::pair<std::string, std::string>(std::to_string((float)totalEncodeTime/1e6), std::to_string((float)totalDecodeTime/1e6)));
    }
    std::map<std::string, std::string>::const_iterator it2 = times.begin();
    for( std::map<std::string, std::string>::const_iterator it = values.begin(); it != values.end(); ++it, ++it2)
    {
      std::cerr<<it->first<<" "<<it->second<<" "<<it2->first<<" "<<it2->second<<std::endl;
    }
  #endif
  }

  TEST(VideoMessageTest, CodecEvalFormatVersion1)
  {
    //H264CodecSpeedAndQualityEval();
    //VP9CodecSpeedAndQualityEval();
    //H264CodecSpeedAndRateEval();
    //VP9CodecSpeedAndRateEval();
    EXPECT_EQ(0,0);
  }

  TEST(VideoMessageTest, EncodeAndDecodeFormatVersion1)
  {
    int Width = 256;
    int Height = 256;
  #if OpenIGTLink_BUILD_VPX
    std::cerr<<"--------------------------- "<<std::endl;
    std::cerr<<"Begin of VPX tests "<<std::endl;
    VPXEncoder* VPXStreamEncoder = new VPXEncoder();
    VPXDecoder* VPXStreamDecoder = new VPXDecoder();
    VPXStreamEncoder->SetPicWidth(Width);
    VPXStreamEncoder->SetPicHeight(Height);
    VPXStreamEncoder->InitializeEncoder();
    VPXStreamEncoder->SetLosslessLink(true);
    TestWithVersion(IGTL_HEADER_VERSION_1, VPXStreamEncoder, VPXStreamDecoder,true);
    VPXStreamEncoder->SetSpeed(VPXStreamEncoder->FastestSpeed);
    VPXStreamEncoder->SetLosslessLink(false);
    std::cerr<<"Encoding Time Using Maximum Speed: "<<std::endl;
    TestWithVersion(IGTL_HEADER_VERSION_1, VPXStreamEncoder, VPXStreamDecoder,false);
    std::cerr<<"End of VPX tests "<<std::endl;
    std::cerr<<"--------------------------- "<<std::endl;
  #endif
  #if OpenIGTLink_BUILD_H264
    std::cerr<<"--------------------------- "<<std::endl;
    std::cerr<<"Begin of H264 tests "<<std::endl;
    H264Encoder* H264StreamEncoder = new H264Encoder();
    H264Decoder* H264StreamDecoder = new H264Decoder();
    H264StreamEncoder->SetPicWidth(Width);
    H264StreamEncoder->SetPicHeight(Height);
    H264StreamEncoder->InitializeEncoder();
    TestWithVersion(IGTL_HEADER_VERSION_1, H264StreamEncoder, H264StreamDecoder,false);
    std::cerr<<"End of H264 tests "<<std::endl;
    std::cerr<<"--------------------------- "<<std::endl;
  #endif
  }


#if OpenIGTLink_PROTOCOL_VERSION >= 3

  TEST(VideoMessageTest, EncodeAndDecodeFormatVersion2)
  {
    int Width = 256;
    int Height = 256;
  #if OpenIGTLink_BUILD_VPX
    std::cerr<<"--------------------------- "<<std::endl;
    std::cerr<<"Begin of VPX tests "<<std::endl;
    VPXEncoder* VPXStreamEncoder = new VPXEncoder();
    VPXDecoder* VPXStreamDecoder = new VPXDecoder();
    VPXStreamEncoder->SetPicWidth(Width);
    VPXStreamEncoder->SetPicHeight(Height);
    VPXStreamEncoder->InitializeEncoder();
    VPXStreamEncoder->SetLosslessLink(true);
    TestWithVersion(IGTL_HEADER_VERSION_2, VPXStreamEncoder, VPXStreamDecoder,true);
    VPXStreamEncoder->SetSpeed(VPXStreamEncoder->FastestSpeed);
    VPXStreamEncoder->SetLosslessLink(false);
    std::cerr<<"Encoding Time Using Maximum Speed: "<<std::endl;
    TestWithVersion(IGTL_HEADER_VERSION_2, VPXStreamEncoder, VPXStreamDecoder,false);
    std::cerr<<"End of VPX tests "<<std::endl;
    std::cerr<<"--------------------------- "<<std::endl;
  #endif
  #if OpenIGTLink_BUILD_H264
    std::cerr<<"--------------------------- "<<std::endl;
    std::cerr<<"Begin of H264 tests "<<std::endl;
    H264Encoder* H264StreamEncoder = new H264Encoder();
    H264Decoder* H264StreamDecoder = new H264Decoder();
    H264StreamEncoder->SetPicWidth(Width);
    H264StreamEncoder->SetPicHeight(Height);
    H264StreamEncoder->InitializeEncoder();
    TestWithVersion(IGTL_HEADER_VERSION_2, H264StreamEncoder, H264StreamDecoder,false);
    std::cerr<<"End of H264 tests "<<std::endl;
    std::cerr<<"--------------------------- "<<std::endl;
  #endif
  }
#endif

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

