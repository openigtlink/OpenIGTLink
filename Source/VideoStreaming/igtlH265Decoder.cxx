/*=========================================================================
 
 Program:   H265Encoder
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlH265Decoder.h"

namespace igtl {


H265Decoder::H265Decoder()
{
  this->nb_pthreads = 4;
  openHevcHandle = libOpenHevcInit(this->nb_pthreads, MULTITHREAD_SLICE/*, pFormatCtx*/);
  libOpenHevcSetCheckMD5(openHevcHandle, false);
  libOpenHevcSetDebugMode(openHevcHandle, 0);
  libOpenHevcStartDecoder(openHevcHandle);
  openHevcFrame.pvY = NULL;
  openHevcFrame.pvU = NULL;
  openHevcFrame.pvV = NULL;
}

H265Decoder::~H265Decoder()
{
  libOpenHevcClose(openHevcHandle);
  if(openHevcFrame.pvY) {
    free(openHevcFrame.pvY);
    free(openHevcFrame.pvU);
    free(openHevcFrame.pvV);
  }
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
    igtl_uint32 dimensions[2] = {static_cast<igtl_uint32>(iWidth), static_cast<igtl_uint32>(iHeight)};
    return this->DecodeBitStreamIntoFrame(videoMessage->GetPackFragmentPointer(2), pDecodedPic->data[0], dimensions, iStreamSize);
    }
  return -1;
}

int H265Decoder::DecodeBitStreamIntoFrame(unsigned char* kpH265BitStream,igtl_uint8* outputFrame, igtl_uint32 dimensions[2], igtl_uint64& iStreamSize)
{
  static int timePts = 0;
  timePts++;
  int got_picture = libOpenHevcDecode(openHevcHandle, kpH265BitStream, iStreamSize, timePts);
  if (got_picture > 0) {
    libOpenHevcGetPictureInfo(openHevcHandle, &openHevcFrame.frameInfo);
    if ((dimensions[0] != openHevcFrame.frameInfo.nWidth) || (dimensions[1] != openHevcFrame.frameInfo.nHeight) || openHevcFrame.pvY==NULL) {
      openHevcFrame.frameInfo.nWidth = dimensions[0];
      openHevcFrame.frameInfo.nHeight = dimensions[1];
      if(openHevcFrame.pvY) {
        free(openHevcFrame.pvY);
        free(openHevcFrame.pvU);
        free(openHevcFrame.pvV);
      }
      int format = openHevcFrame.frameInfo.chromat_format == YUV420 ? 1 : 0;
      openHevcFrame.pvY = calloc (openHevcFrame.frameInfo.nYPitch * openHevcFrame.frameInfo.nHeight, sizeof(unsigned char));
      openHevcFrame.pvU = calloc (openHevcFrame.frameInfo.nUPitch * openHevcFrame.frameInfo.nHeight >> format, sizeof(unsigned char));
      openHevcFrame.pvV = calloc (openHevcFrame.frameInfo.nVPitch * openHevcFrame.frameInfo.nHeight >> format, sizeof(unsigned char));
    }
    libOpenHevcGetOutputCpy(openHevcHandle, 1, &openHevcFrame);
    ReconstructFrame(&openHevcFrame, outputFrame);
    return 1;
  }
  return -1;
}

int H265Decoder::ReconstructFrame(OpenHevc_Frame_cpy *openHevcFrame,igtl_uint8* outputFrame)
{
  int y;
  int y_offset, y_offset2;
  int height, format, width;
  int src_stride;
  int dst_stride;
  int src_stride_c;
  int dst_stride_c;
  
  format = openHevcFrame->frameInfo.chromat_format == YUV420 ? 1 : 0;
  src_stride = openHevcFrame->frameInfo.nYPitch;
  src_stride_c = openHevcFrame->frameInfo.nUPitch;
  height = openHevcFrame->frameInfo.nHeight;
  width = openHevcFrame->frameInfo.nWidth;;
  dst_stride = openHevcFrame->frameInfo.nWidth;
  dst_stride_c = openHevcFrame->frameInfo.nWidth>>1;
  y_offset = y_offset2 = 0;
  
  for (y = 0; y < height; y++)
    {
    memcpy(outputFrame+y_offset2, ((char*)openHevcFrame->pvY)+y_offset, dst_stride);
    y_offset  += src_stride;
    y_offset2 += dst_stride;
    }
  
  y_offset = y_offset2 = 0;
  
  for (y = 0; y < height >> format; y++)
    {
    memcpy(outputFrame+y_offset2+width*height, ((char*)openHevcFrame->pvU)+y_offset, dst_stride_c);
    memcpy(outputFrame+y_offset2+width*height*5/4, ((char*)openHevcFrame->pvV)+y_offset, dst_stride_c);
    y_offset  += src_stride_c;
    y_offset2 += dst_stride_c;
    }
  return 1;
}

}// namespace igtl

