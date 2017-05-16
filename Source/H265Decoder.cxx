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
    this->nb_pthreads = 4;
    openHevcHandle = libOpenHevcInit(this->nb_pthreads, FF_THREAD_SLICE/*, pFormatCtx*/);
    libOpenHevcSetCheckMD5(openHevcHandle, false);
    //av_register_all();
    //pFormatCtx = avformat_alloc_context();
    libOpenHevcSetDebugMode(openHevcHandle, 1);
    libOpenHevcStartDecoder(openHevcHandle);
    //avformat_open_input(&pFormatCtx, filename, NULL, NULL)!=0)
    //av_read_frame(pFormatCtx, &packet)<0)
    openHevcFrameCpy.pvY = NULL;
    openHevcFrameCpy.pvU = NULL;
    openHevcFrameCpy.pvV = NULL;
  }

  H265Decoder::~H265Decoder()
  {
    
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

  int H265Decoder::DecodeBitStreamIntoFrame(unsigned char* kpH265BitStream,igtl_uint8* outputFrame, igtl_uint32 dimensions[2], igtl_uint64& iStreamSize)
  {
    /*packet->data = NULL;
    packet->size = 0;
    av_init_packet(packet);
    packet->data = kpH265BitStream;
    packet->size = iStreamSize;*/
    static int timePts = 0;
    timePts++;
    int got_picture = libOpenHevcDecode(openHevcHandle, kpH265BitStream, iStreamSize, timePts);
    if (got_picture > 0) {
      libOpenHevcGetOutput(openHevcHandle, 1, &openHevcFrame);
      FILE *fout  = fopen("HDVideo.yuv", "ab");
      if (fout) {
        
        int format = openHevcFrame.frameInfo.chromat_format == YUV420 ? 1 : 0;
        libOpenHevcGetPictureInfo(openHevcHandle, &openHevcFrameCpy.frameInfo);
        
        if(openHevcFrameCpy.pvY==NULL) {
          openHevcFrameCpy.pvY = calloc (openHevcFrameCpy.frameInfo.nYPitch * openHevcFrameCpy.frameInfo.nHeight, sizeof(unsigned char));
          openHevcFrameCpy.pvU = calloc (openHevcFrameCpy.frameInfo.nUPitch * openHevcFrameCpy.frameInfo.nHeight >> format, sizeof(unsigned char));
          openHevcFrameCpy.pvV = calloc (openHevcFrameCpy.frameInfo.nVPitch * openHevcFrameCpy.frameInfo.nHeight >> format, sizeof(unsigned char));
        }
        format = openHevcFrameCpy.frameInfo.chromat_format == YUV420 ? 1 : 0;
        libOpenHevcGetOutputCpy(openHevcHandle, 1, &openHevcFrameCpy);
        fwrite( openHevcFrameCpy.pvY , sizeof(uint8_t) , openHevcFrameCpy.frameInfo.nYPitch * openHevcFrameCpy.frameInfo.nHeight, fout);
        fwrite( openHevcFrameCpy.pvU , sizeof(uint8_t) , openHevcFrameCpy.frameInfo.nUPitch * openHevcFrameCpy.frameInfo.nHeight >> format, fout);
        fwrite( openHevcFrameCpy.pvV , sizeof(uint8_t) , openHevcFrameCpy.frameInfo.nVPitch * openHevcFrameCpy.frameInfo.nHeight >> format, fout);
        fclose(fout);
        /*if(openHevcFrame.pvY) {
          free(openHevcFrame.pvY);
          free(openHevcFrame.pvU);
          free(openHevcFrame.pvV);
        }
        *(openHevcFrame.pvY) = calloc (openHevcFrame.frameInfo.nYPitch * openHevcFrame.frameInfo.nHeight, sizeof(unsigned char));
        *(openHevcFrame.pvU) = calloc (openHevcFrame.frameInfo.nUPitch * openHevcFrame.frameInfo.nHeight >> format, sizeof(unsigned char));
        *(openHevcFrame.pvV) = calloc (openHevcFrame.frameInfo.nVPitch * openHevcFrame.frameInfo.nHeight >> format, sizeof(unsigned char));*/
      }
      
      //ReconstructFrame(&openHevcFrame, outputFrame);
      return 1;
    }
    return -1;
  }

int H265Decoder::ReconstructFrame(OpenHevc_Frame *openHevcFrame,igtl_uint8* outputFrame)
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
  
  for (y = 0; y < height; y++) {
    memcpy(outputFrame+y_offset2, ((char*)openHevcFrame->pvY)+y_offset, dst_stride);
    y_offset  += src_stride;
    y_offset2 += dst_stride;
  }
  
  y_offset = y_offset2 = 0;
  
  for (y = 0; y < height >> format; y++) {
    memcpy(outputFrame+y_offset2+width*height, ((char*)openHevcFrame->pvU)+y_offset, dst_stride_c);
    memcpy(outputFrame+y_offset2+width*height*5/4, ((char*)openHevcFrame->pvV)+y_offset, dst_stride_c);
    y_offset  += src_stride_c;
    y_offset2 += dst_stride_c;
  }
  return 1;
}

int H265Decoder::find_start_code (unsigned char *Buf, int zeros_in_startcode)
  {
    int i;
    for (i = 0; i < zeros_in_startcode; i++)
      if(Buf[i] != 0)
        return 0;
    return Buf[i];
  }

  int H265Decoder::get_next_nal(FILE* inpf, unsigned char* Buf)
  {
    int pos = 0;
    int StartCodeFound = 0;
    int info2 = 0;
    int info3 = 0;
    while(!feof(inpf)&&(/*Buf[pos++]=*/fgetc(inpf))==0);
    
    while (pos < 3) Buf[pos++] = fgetc (inpf);
    while (!StartCodeFound)
    {
      if (feof (inpf))
      {
        //            return -1;
        return pos-1;
      }
      Buf[pos++] = fgetc (inpf);
      info3 = find_start_code(&Buf[pos-4], 3);
      if(info3 != 1)
        info2 = find_start_code(&Buf[pos-3], 2);
      StartCodeFound = (info2 == 1 || info3 == 1);
    }
    fseek (inpf, - 4 + info2, SEEK_CUR);
    return pos - 4 + info2;
  }

