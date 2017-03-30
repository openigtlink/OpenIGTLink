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
#include "H264Encoder.h"
#include "H264Decoder.h"
#include "igtlMessageDebugFunction.h"
#include "igtl_video.h"
#include "igtl_types.h"
#include "igtl_header.h"
#include "igtl_util.h"
#include "igtlTestConfig.h"
#include "string.h"
#include "macros.h"
#if OpenIGTLink_PROTOCOL_VERSION >= 3
  #include "igtlMessageFormat2TestMacro.h"
#endif

void TestWithVersion(int version)
{
  // Get thread information
  int Width = 256;
  int Height = 256;
  H264Encoder* videoStreamEncoder = new H264Encoder();
  H264Decoder* videoStreamDecoder = new H264Decoder();
  videoStreamEncoder->SetPicWidth(Width);
  videoStreamEncoder->SetPicHeight(Height);
  videoStreamEncoder->InitializeEncoder();
  int kiPicResSize = Width*Height;
  igtl_uint8* imagePointer = new igtl_uint8[kiPicResSize*3/2];
  memset(imagePointer, 0, Width * Height * 3 / 2);
  //memcpy(imagePointer, frameImage->GetScalarPointer(), imageSizePixels[0] * imageSizePixels[1]);
  SourcePicture* pSrcPic = new SourcePicture();
  pSrcPic->colorFormat = videoFormatI420;
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
        iFrameNumInFile = WELS_MAX ((igtl_int32) (i_size / kiPicResSize), iFrameNumInFile);
      }
#else
      if (!fseek (pFileYUV, 0, SEEK_END)) {
        igtl_int64 i_size = ftell (pFileYUV);
        fseek (pFileYUV, 0, SEEK_SET);
        iFrameNumInFile = WELS_MAX ((igtl_int32) (i_size / kiPicResSize), iFrameNumInFile);
      }
#endif
#else
      if (!fseeko (pFileYUV, 0, SEEK_END)) {
        igtl_int64 i_size = ftello (pFileYUV);
        fseeko (pFileYUV, 0, SEEK_SET);
        iFrameNumInFile = WELS_MAX ((igtl_int32) (i_size / kiPicResSize), iFrameNumInFile);
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
      if(fread (imagePointer, 1, kiPicResSize, pFileYUV) == kiPicResSize)
      {
        bool isGrayImage = true;
        igtl::VideoMessage::Pointer videoMessageSend = igtl::VideoMessage::New();
        videoMessageSend->SetHeaderVersion(version);
        if(version == IGTL_HEADER_VERSION_2)
          igtlMetaDataAddElementMacro(videoMessageSend);
        int iEncFrames = videoStreamEncoder->EncodeSingleFrameIntoVideoMSG(pSrcPic, videoMessageSend, isGrayImage);
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
          EXPECT_EQ(memcmp(videoMessageReceived->GetPackFragmentPointer(2),videoMessageSend->GetPackFragmentPointer(2),videoMessageReceived->GetBitStreamSize()),0);
          videoStreamDecoder->DecodeVideoMSGIntoSingleFrame(videoMessageReceived.GetPointer(), pDecodedPic);
          if(version == IGTL_HEADER_VERSION_2)
          {
            videoMessageReceived->SetMessageID(1); // Message ID is reset by the codec, so the comparison of ID is no longer valid, manually set to 1;
            igtlMetaDataComparisonMacro(videoMessageReceived);
          }
          //EXPECT_EQ(memcmp(pDecodedPic->pData[0], pSrcPic->pData[0],kiPicResSize),0);
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
}

TEST(VideoMessageTest, EncodeAndDecodeFormatVersion1)
{
  TestWithVersion(IGTL_HEADER_VERSION_1);
}

#if OpenIGTLink_PROTOCOL_VERSION >= 3

  TEST(VideoMessageTest, EncodeAndDecodeFormatVersion2)
  {
    TestWithVersion(IGTL_HEADER_VERSION_2);
  }

#endif

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

