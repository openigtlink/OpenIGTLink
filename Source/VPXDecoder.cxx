/*=========================================================================
 
 Program:   VPXEncoder
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "VPXDecoder.h"

VPXDecoder::VPXDecoder()
{
  decoder = get_vpx_decoder_by_name("vp9");
  vpx_codec_dec_init(&codec, decoder->codec_interface(), NULL, 0);
  this->deviceName = "";
}

VPXDecoder::~VPXDecoder()
{
  vpx_codec_destroy(&codec);
  decoder = NULL;
}

int VPXDecoder::DecodeVideoMSGIntoSingleFrame(igtl::VideoMessage* videoMessage, SourcePicture* pDecodedPic)
{
  if(videoMessage->GetBitStreamSize())
  {
    igtl_int32 iWidth = videoMessage->GetWidth();
    igtl_int32 iHeight = videoMessage->GetHeight();
    igtl_uint64 iStreamSize = videoMessage->GetBitStreamSize();
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

int VPXDecoder::DecodeBitStreamIntoFrame(unsigned char* bitstream,igtl_uint8* outputFrame, igtl_uint32 dimensions[2], igtl_uint64& iStreamSize) {
  
  if (vpx_codec_decode(&codec, bitstream, (unsigned int)iStreamSize, NULL, 0))
    die_codec(&codec, "Failed to decode frame.");
  if((outputImage = vpx_codec_get_frame(&codec, &iter)) != NULL) {
    memcpy(outputFrame, outputImage->fb_priv, outputImage->d_h*outputImage->d_w*3/2);
    return 2;
  }
  return 1;
}
