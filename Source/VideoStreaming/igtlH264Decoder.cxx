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

#include "igtlH264Decoder.h"

namespace igtl {

H264Decoder::H264Decoder()
{
  WelsCreateDecoder (&this->pDecoder);
  memset (&this->decParam, 0, sizeof (SDecodingParam));
  this->decParam.uiTargetDqLayer = UCHAR_MAX;
  this->decParam.eEcActiveIdc = ERROR_CON_SLICE_COPY;
  this->decParam.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_DEFAULT;
  this->pDecoder->Initialize (&decParam);
  this->deviceName = "";
}

H264Decoder::~H264Decoder()
{
  WelsDestroyDecoder(pDecoder);
  pDecoder = NULL;
}


int H264Decoder::Process (void* pDst[3], SBufferInfo* pInfo, FILE* pFp) {
  
  int iRet = 0;
  
  if (pDst[0] && pDst[1] && pDst[2] && pInfo) {
    igtl_uint32 iStride[2];
    int iWidth = pInfo->UsrData.sSystemBuffer.iWidth;
    int iHeight = pInfo->UsrData.sSystemBuffer.iHeight;
    iStride[0] = pInfo->UsrData.sSystemBuffer.iStride[0];
    iStride[1] = pInfo->UsrData.sSystemBuffer.iStride[1];
    if(pFp)
      {
      igtl_uint32 dimensions[2] = {static_cast<igtl_uint32>(iWidth), static_cast<igtl_uint32>(iHeight)};
      Write2File (pFp, (unsigned char**)pDst, dimensions, iStride);
      }
  }
  return iRet;
}

igtl_int32 iFrameCountTotal = 0;


void H264Decoder::ComposeByteSteam(igtl_uint8** inputData, int dimension[2], int iStride[2], igtl_uint8 *outputFrame)
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


int H264Decoder::DecodeVideoMSGIntoSingleFrame(igtl::VideoMessage* videoMessage, SourcePicture* pDecodedPic)
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

int H264Decoder::DecodeBitStreamIntoFrame(unsigned char* kpH264BitStream,igtl_uint8* outputFrame, igtl_uint32 dimensions[2], igtl_uint64& iStreamSize) {
  
  unsigned long long uiTimeStamp = 0;
  igtl_int64 iStart = 0, iEnd = 0, iTotal = 0;
  igtl_int32 iSliceSize;
  igtl_int32 iSliceIndex = 0;
  igtl_uint32 iWidth = dimensions[0];
  igtl_uint32 iHeight = dimensions[1];
  unsigned char* pBuf = NULL;
  unsigned char uiStartCode[4] = {0, 0, 0, 1};
  
  unsigned char* pData[3] = {NULL};
  SBufferInfo sDstBufInfo;
  
  igtl_int32 iBufPos = 0;
  igtl_int32 i = 0;
  igtl_int32 iFrameCount = 0;
  igtl_int32 iEndOfStreamFlag = 0;
  //for coverage test purpose
  igtl_int32 iErrorConMethod = (igtl_int32) ERROR_CON_SLICE_MV_COPY_CROSS_IDR_FREEZE_RES_CHANGE;
  pDecoder->SetOption (DECODER_OPTION_ERROR_CON_IDC, &iErrorConMethod);
  //~end for
  double dElapsed = 0;
  
  if (iStreamSize <= 0)
    {
    //fprintf (stderr, "Current Bit Stream File is too small, read error!!!!\n");
    goto label_exit;
    }
  pBuf = new unsigned char[iStreamSize + 5];
  if (pBuf == NULL)
    {
    //fprintf (stderr, "new buffer failed!\n");
    goto label_exit;
    }
  memcpy (pBuf, kpH264BitStream, iStreamSize);
  memcpy (pBuf + iStreamSize, &uiStartCode[0], 4); //confirmed_safe_unsafe_usage
  
  while (true)
    {
    if (iBufPos >= iStreamSize)
      {
      iEndOfStreamFlag = true;
      if (iEndOfStreamFlag)
        pDecoder->SetOption (DECODER_OPTION_END_OF_STREAM, (void*)&iEndOfStreamFlag);
      break;
      }
    for (i = 0; i < iStreamSize; i++)
      {
      if ((pBuf[iBufPos + i] == 0 && pBuf[iBufPos + i + 1] == 0 && pBuf[iBufPos + i + 2] == 0 && pBuf[iBufPos + i + 3] == 1
           && i > 0) || (pBuf[iBufPos + i] == 0 && pBuf[iBufPos + i + 1] == 0 && pBuf[iBufPos + i + 2] == 1 && i > 0))
        {
        break;
        }
      }
    iSliceSize = i;
    if (iSliceSize < 4)
      { //too small size, no effective data, ignore
        iBufPos += iSliceSize;
        continue;
      }
    
    iStart = getCurrentTime();
    pData[0] = NULL;
    pData[1] = NULL;
    pData[2] = NULL;
    uiTimeStamp ++;
    memset (&sDstBufInfo, 0, sizeof (SBufferInfo));
    sDstBufInfo.uiInBsTimeStamp = uiTimeStamp;
    sDstBufInfo.UsrData.sSystemBuffer.iWidth =
#ifndef NO_DELAY_DECODING
    pDecoder->DecodeFrameNoDelay (pBuf + iBufPos, iSliceSize, pData, &sDstBufInfo);
#else
    pDecoder->DecodeFrame2 (pBuf + iBufPos, iSliceSize, pData, &sDstBufInfo);
#endif
    
    iEnd    = getCurrentTime();
    iTotal  = iEnd - iStart;
    if (sDstBufInfo.iBufferStatus == 1)
      {
      Process ((void**)pData, &sDstBufInfo, NULL);
      iWidth  = sDstBufInfo.UsrData.sSystemBuffer.iWidth;
      iHeight = sDstBufInfo.UsrData.sSystemBuffer.iHeight;
      ++ iFrameCount;
      }
    
#ifdef NO_DELAY_DECODING
    iStart = getCurrentTime();
    pData[0] = NULL;
    pData[1] = NULL;
    pData[2] = NULL;
    memset (&sDstBufInfo, 0, sizeof (SBufferInfo));
    sDstBufInfo.uiInBsTimeStamp = uiTimeStamp;
    pDecoder->DecodeFrame2 (NULL, 0, pData, &sDstBufInfo);
    iEnd    = getCurrentTime();
    iTotal = iEnd - iStart;
    if (sDstBufInfo.iBufferStatus == 1)
      {
      Process ((void**)pData, &sDstBufInfo, NULL);
      int dimension[2] = {static_cast<int>(iWidth), static_cast<int>(iHeight)};
      int stride[2] = {sDstBufInfo.UsrData.sSystemBuffer.iStride[0],sDstBufInfo.UsrData.sSystemBuffer.iStride[1]};
      ComposeByteSteam(pData, dimension, stride, outputFrame);
      iWidth  = sDstBufInfo.UsrData.sSystemBuffer.iWidth;
      iHeight = sDstBufInfo.UsrData.sSystemBuffer.iHeight;
      ++ iFrameCount;
      }
#endif
    if (iFrameCount)
      {
      dElapsed = iTotal / 1e6;
      }
    iBufPos += iSliceSize;
    ++ iSliceIndex;
    }
  // coverity scan uninitial
label_exit:
  if (pBuf) {
    delete[] pBuf;
    pBuf = NULL;
  }
  if (iFrameCount)
    {
    return 2;
    }
  return -1;
}

}// Namespace igtl
