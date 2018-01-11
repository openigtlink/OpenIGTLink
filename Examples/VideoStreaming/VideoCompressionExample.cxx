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
#include <stdlib.h>
#include "igtlVideoMessage.h"
#include "igtlMessageDebugFunction.h"
#include "igtl_video.h"
#include "igtl_types.h"
#include "igtl_header.h"
#include "igtl_util.h"
#include "string.h"
#include <cstring>
#include <string>

#if defined(OpenIGTLink_USE_H264)
  #include "H264Encoder.h"
  #include "H264Decoder.h"
#endif

#if defined(OpenIGTLink_USE_VP9)
  #include "VP9Encoder.h"
  #include "VP9Decoder.h"
#endif

#if defined(OpenIGTLink_USE_AV1)
  #include "AV1Encoder.h"
  #include "AV1Decoder.h"
#endif

int Width = 256;
int Height = 256;
std::string testFileName(OpenIGTLink_SOURCE_ROOTDIR);
int startIndex = 0;
int  inputFrameNum = 100;

int main(int argc, char* argv[])
  {
    GenericEncoder * encoder = NULL;
    GenericDecoder * decoder = NULL;
  #if defined(OpenIGTLink_USE_VP9)
    std::cerr<<"--------------------------- "<<std::endl;
    std::cerr<<"Begin of VP9 tests "<<std::endl;
    VP9Encoder* VP9StreamEncoder = new VP9Encoder();
    VP9Decoder* VP9StreamDecoder = new VP9Decoder();
    VP9StreamEncoder->SetPicWidthAndHeight(Width,Height);
    VP9StreamEncoder->InitializeEncoder();
    VP9StreamEncoder->SetLosslessLink(false);
    VP9StreamEncoder->SetSpeed(VP9StreamEncoder->FastestSpeed);
    encoder = VP9StreamEncoder;
    decoder = VP9StreamDecoder;
  #endif
  #if defined(OpenIGTLink_USE_H264)
    H264Encoder* H264StreamEncoder = new H264Encoder();
    H264Decoder* H264StreamDecoder = new H264Decoder();
    H264StreamEncoder->SetPicWidthAndHeight(Width,Height);
    H264StreamEncoder->InitializeEncoder();
    encoder = H264StreamEncoder;
    decoder = H264StreamDecoder;
  #endif
#if defined(OpenIGTLink_USE_AV1)
    igtlAV1Encoder* AV1StreamEncoder = new AV1Encoder();
    igtlAV1Decoder* AV1StreamDecoder = new AV1Decoder();
    AV1StreamEncoder->SetPicWidthAndHeight(Width, Height);
    AV1StreamEncoder->InitializeEncoder();
    AV1StreamEncoder->SetLosslessLink(false);
    AV1StreamEncoder->SetSpeed(AV1StreamEncoder->FastestSpeed);
    encoder = AV1StreamEncoder;
    decoder = AV1StreamDecoder;
#endif
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
    int totalFrame = 0;
    for(int i = 0; i <inputFrameNum; i++)
    {
      igtl_int32 iFrameNumInFile = -1;
      std::string sep = "/";
#if defined(_WIN32) || defined(_WIN64)
      sep = "\\";
#endif
      stringstream stream;
      stream << (i%6+1);
      std::string imageIndexStr = stream.str();
      std::string testIndexedFileName = std::string(testFileName);
      testIndexedFileName.append(sep).append("Testing").append(sep).append("img").append(sep).append("igtlTestImage").append(imageIndexStr).append(".raw");
      FILE* pFileYUV = NULL;
      pFileYUV = fopen (testIndexedFileName.c_str(), "rb");
      if (pFileYUV != NULL) {
        igtl_int64 i_size = 0;
#if defined(_WIN32) || defined(_WIN64)
#if _MSC_VER >= 1400
        if (!_fseeki64 (pFileYUV, 0, SEEK_END)) {
          i_size = _ftelli64 (pFileYUV);
          _fseeki64 (pFileYUV, 0, SEEK_SET);
          iFrameNumInFile =std::fmax((igtl_int32) (i_size / kiPicResSize), iFrameNumInFile);
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
          iFrameNumInFile = std::max((igtl_int32) (i_size / (kiPicResSize)), iFrameNumInFile);
        }
#endif
#if defined(_WIN32) || defined(_WIN64)
#if _MSC_VER >= 1400
        _fseeki64(pFileYUV, startIndex*kiPicResSize, SEEK_SET); //fix the data range issue
#else
        fseek(pFileYUV, startIndex*kiPicResSize, SEEK_SET); //fix the data range issue
#endif
#else
        fseek(pFileYUV, startIndex*kiPicResSize, SEEK_SET);
#endif
        while(fread (imagePointer, 1, kiPicResSize, pFileYUV ) == (kiPicResSize) && totalFrame<inputFrameNum)
        {
          bool isGrayImage = true;
          igtl::VideoMessage::Pointer videoMessageSend = igtl::VideoMessage::New();
          videoMessageSend->SetHeaderVersion(IGTL_HEADER_VERSION_2);
          int iEncFrames = encoder->EncodeSingleFrameIntoVideoMSG(pSrcPic, videoMessageSend, isGrayImage);
          if(iEncFrames == 0)
          {
            std::cerr<<"Encoding Frame index: " << i << std::endl;
            std::cerr<<"Packet size: " << videoMessageSend->GetPackSize() << std::endl;
            igtl::VideoMessage::Pointer videoMessageReceived = igtl::VideoMessage::New();
            igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
            headerMsg->AllocatePack();
            memcpy(headerMsg->GetPackPointer(), videoMessageSend->GetPackPointer(), IGTL_HEADER_SIZE);
            headerMsg->Unpack();
            videoMessageReceived->SetMessageHeader(headerMsg);
            videoMessageReceived->AllocatePack();
            memcpy(videoMessageReceived->GetPackBodyPointer(), videoMessageSend->GetPackBodyPointer(), videoMessageSend->GetPackBodySize());
            videoMessageReceived->Unpack();
            if (memcmp(videoMessageReceived->GetPackFragmentPointer(2), videoMessageSend->GetPackFragmentPointer(2), videoMessageReceived->GetBitStreamSize()) != 0)
            {
              return -1;
            }
            int iRet= decoder->DecodeVideoMSGIntoSingleFrame(videoMessageReceived.GetPointer(), pDecodedPic);
            if(iRet)
            {
              std::cerr<<"Decoding Frame Index: " << totalFrame << std::endl;
              totalFrame ++;
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
    }
    delete pDecodedPic;
    delete pSrcPic;
    pDecodedPic = NULL;
    pSrcPic = NULL;
    return 0;
  }