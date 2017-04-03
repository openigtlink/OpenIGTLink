/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/
#include <sstream>
#include "igtlVideoMessage.h"
#include "igtlMessageDebugFunction.h"
#include "igtl_video.h"
#include "igtl_types.h"
#include "igtl_header.h"
#include "igtl_util.h"
#include "igtlTestConfig.h"
#include "string.h"
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
#endif

int startEncodeTime = 0;
int endEncodeTime = 0;
int startDecodeTime = 0;
int endDecodeTime = 0;
int totalEncodeTime = 0;
int totalDecodeTime =0;

void TestWithVersion(int version, GenericEncoder* videoStreamEncoder, GenericDecoder* videoStreamDecoder, bool compareImage)
{
  // Get thread information
  int Width = 256;
  int Height = 256;
  int kiPicResSize = Width*Height;
  igtl_uint8* imagePointer = new igtl_uint8[kiPicResSize*3/2];
  memset(imagePointer, 0, Width * Height * 3 / 2);
  //memcpy(imagePointer, frameImage->GetScalarPointer(), imageSizePixels[0] * imageSizePixels[1]);
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
  totalEncodeTime = 0;
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
          videoStreamDecoder->DecodeVideoMSGIntoSingleFrame(videoMessageReceived.GetPointer(), pDecodedPic);
          endDecodeTime = videoStreamDecoder->getCurrentTime();
          totalDecodeTime += (endDecodeTime - startDecodeTime);
          if(version == IGTL_HEADER_VERSION_2)
          {
            videoMessageReceived->SetMessageID(1); // Message ID is reset by the codec, so the comparison of ID is no longer valid, manually set to 1;
            igtlMetaDataComparisonMacro(videoMessageReceived);
          }
          if(compareImage)
            EXPECT_EQ(memcmp(pDecodedPic->data[0], pSrcPic->data[0],kiPicResSize*3/2),0);
        }
      }
      igtl::Sleep(20);
      if (pFileYUV)
      {
        fclose(pFileYUV);
        pFileYUV = NULL;
      }
    } else {
      fprintf (stderr, "Unable to open source sequence file, check corresponding path!\n");
    }
  }
  std::cerr<<"Compression Rate: "<<(float)bitstreamTotalLength/(Width*Height*100)<<std::endl;
}


TEST(VideoMessageTest, VPXCodecSpeedAndQualityTestFormatVersion1)
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
  std::cerr<<"Total encode and decode time for lossy coding: "<<(float)totalEncodeTime/1e6<<",  "<<(float)totalDecodeTime/1e6<<std::endl;
  videoStreamEncoder->SetLosslessLink(true);
  videoStreamEncoder->InitializeEncoder();
  TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder,true);
  std::cerr<<"Total encode and decode time for lossless coding: "<<(float)totalEncodeTime/1e6<<",  "<<(float)totalDecodeTime/1e6<<std::endl;
#endif
}



TEST(VideoMessageTest, H264CodecSpeedAndQualityTestFormatVersion1)
{
#if OpenIGTLink_BUILD_H264
  H264Encoder* videoStreamEncoder = new H264Encoder();
  H264Decoder* videoStreamDecoder = new H264Decoder();
  int Width = 256;
  int Height = 256;
  videoStreamEncoder->SetPicWidth(Width);
  videoStreamEncoder->SetPicHeight(Height);
  videoStreamEncoder->SetLosslessLink(false);
  videoStreamEncoder->InitializeEncoder();
  TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder,false);
  std::cerr<<"Total encode and decode time for near lossless coding: "<<(float)totalEncodeTime/1e6<<",  "<<(float)totalDecodeTime/1e6<<std::endl;
#endif
}


TEST(VideoMessageTest, EncodeAndDecodeFormatVersion1)
{
#if OpenIGTLink_BUILD_VPX
  VPXEncoder* videoStreamEncoder = new VPXEncoder();
  VPXDecoder* videoStreamDecoder = new VPXDecoder();
#elif OpenIGTLink_BUILD_H264
  H264Encoder* videoStreamEncoder = new H264Encoder();
  H264Decoder* videoStreamDecoder = new H264Decoder();
#endif
  int Width = 256;
  int Height = 256;
  videoStreamEncoder->SetPicWidth(Width);
  videoStreamEncoder->SetPicHeight(Height);
  videoStreamEncoder->InitializeEncoder();
  TestWithVersion(IGTL_HEADER_VERSION_1, videoStreamEncoder, videoStreamDecoder,true);
}


#if OpenIGTLink_PROTOCOL_VERSION >= 3

  TEST(VideoMessageTest, EncodeAndDecodeFormatVersion2)
  {
#if OpenIGTLink_BUILD_VPX
    VPXEncoder* videoStreamEncoder = new VPXEncoder();
    VPXDecoder* videoStreamDecoder = new VPXDecoder();
#elif OpenIGTLink_BUILD_H264
    H264Encoder* videoStreamEncoder = new H264Encoder();
    H264Decoder* videoStreamDecoder = new H264Decoder();
#endif
    int Width = 256;
    int Height = 256;
    videoStreamEncoder->SetPicWidth(Width);
    videoStreamEncoder->SetPicHeight(Height);
    videoStreamEncoder->InitializeEncoder();
    TestWithVersion(IGTL_HEADER_VERSION_2, videoStreamEncoder, videoStreamDecoder,true);
  }

#endif

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

