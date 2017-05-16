/*=========================================================================
 
 Program:   H265Encoder
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "H265Decoder.h"
  H265Decoder::H265Decoder()
  {
    err =DE265_OK;
    ctx = de265_new_decoder();
    de265_set_parameter_bool(ctx, DE265_DECODER_PARAM_BOOL_SEI_CHECK_HASH, false);
    de265_set_parameter_bool(ctx, DE265_DECODER_PARAM_SUPPRESS_FAULTY_PICTURES, false);
    
    de265_set_parameter_bool(ctx, DE265_DECODER_PARAM_DISABLE_DEBLOCKING, 0);
    de265_set_parameter_bool(ctx, DE265_DECODER_PARAM_DISABLE_SAO, 0);
    de265_set_limit_TID(ctx, 100);
    currentBitPos = 0;
  }

  H265Decoder::~H265Decoder()
  {
    de265_free_decoder(ctx);
  }

  void H265Decoder::ComposeByteSteam(igtl_uint8** inputData, int dimension[2], int iStride[2], igtl_uint8 *outputFrame)
  {
    int iWidth = dimension[0];
    int iHeight = dimension[1];
  #pragma omp parallel for default(none) shared(outputByteStream,inputData, iStride, iHeight, iWidth)
    for (int i = 0; i < iHeight; i++)
    {
      igtl_uint8* pPtr = inputData[0]+i*iStride[0];
      for (int j = 0; j < iWidth; j++)
      {
        outputFrame[i*iWidth + j] = pPtr[j];
      }
    }
  #pragma omp parallel for default(none) shared(outputByteStream,inputData, iStride, iHeight, iWidth)
    for (int i = 0; i < iHeight/2; i++)
    {
      igtl_uint8* pPtr = inputData[1]+i*iStride[1];
      for (int j = 0; j < iWidth/2; j++)
      {
        outputFrame[i*iWidth/2 + j + iHeight*iWidth] = pPtr[j];
      }
    }
  #pragma omp parallel for default(none) shared(outputByteStream, inputData, iStride, iHeight, iWidth)
    for (int i = 0; i < iHeight/2; i++)
    {
      igtl_uint8* pPtr = inputData[2]+i*iStride[1];
      for (int j = 0; j < iWidth/2; j++)
      {
        outputFrame[i*iWidth/2 + j + iHeight*iWidth*5/4] = pPtr[j];
      }
    }
  }


  int H265Decoder::DecodeVideoMSGIntoSingleFrame(igtl::VideoMessage* videoMessage, SourcePicture* pDecodedPic)
  {
    if(videoMessage->GetBitStreamSize())
    {
      igtl_int32 iWidth = videoMessage->GetWidth();
      igtl_int32 iHeight = videoMessage->GetHeight();
      igtl_uint64 iStreamSize = videoMessage->GetBitStreamSize();
      igtl_uint16 frameType = videoMessage->GetFrameType();
      isGrayImage = false;
      if (frameType>0X00FF)
      {
        frameType= frameType>>8;
        isGrayImage = true;
      }
      pDecodedPic->picWidth = iWidth;
      pDecodedPic->picHeight = iHeight;
      pDecodedPic->data[1]= pDecodedPic->data[0] + iWidth*iHeight;
      pDecodedPic->data[2]= pDecodedPic->data[1] + iWidth*iHeight/4;
      pDecodedPic->stride[0] = iWidth;
      pDecodedPic->stride[1] = pDecodedPic->stride[2] = iWidth>>1;
      pDecodedPic->stride[3] = 0;
      igtl_uint32 dimensions[2] = {iWidth, iHeight};
      int iRet = this->DecodeBitStreamIntoFrame(videoMessage->GetPackFragmentPointer(2), pDecodedPic->data[0], dimensions, iStreamSize);
      return iRet;
    }
    return -1;
  }

  int H265Decoder::DecodeBitStreamIntoFrame(unsigned char* kpH265BitStream,igtl_uint8* outputFrame, igtl_uint32 dimensions[2], igtl_uint64& iStreamSize) {
    err = de265_push_data(ctx, kpH265BitStream, iStreamSize, currentBitPos, (void*)2);
    //currentBitPos += iStreamSize;
    if (err == DE265_OK) {
      int more = 1;
      err = de265_decode(ctx, &more);
      if (err != DE265_OK) {
        return -1;
      }
      de265_push_end_of_frame(ctx);
      const de265_image* img = de265_get_next_picture(ctx);
      if(img)
      {
        int pos = 0;
        for (int c=0;c<3;c++) {
          int stride;
          const uint8_t* p = de265_get_image_plane(img, c, &stride);
          memcpy(outputFrame+pos, p, de265_get_image_height(img,c)*de265_get_image_width(img,c));
          pos += de265_get_image_height(img,c)*de265_get_image_width(img,c);
        }
      }
    }
    return -1;
  }
