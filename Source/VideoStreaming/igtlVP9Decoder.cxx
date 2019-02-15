/*=========================================================================
 
 Program:   VPXEncoder
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlVP9Decoder.h"

namespace igtl {

static const VpxInterfaceDecoder vp9StaticDecoder[] = {{&vpx_codec_vp9_dx}};

// TODO(dkovalev): move this function to vpx_image.{c, h}, so it will be part
// of vpx_image_t support, this section will be removed when it is moved to vpx_image
int VP9Decoder::vpx_img_plane_width(const vpx_image_t *img, int plane) {
  if (plane > 0 && img->x_chroma_shift > 0)
    return (img->d_w + 1) >> img->x_chroma_shift;
  else
    return img->d_w;
}

int VP9Decoder::vpx_img_plane_height(const vpx_image_t *img, int plane) {
  if (plane > 0 && img->y_chroma_shift > 0)
    return (img->d_h + 1) >> img->y_chroma_shift;
  else
    return img->d_h;
}


VP9Decoder::VP9Decoder()
{
  decoder = &vp9StaticDecoder[0];
  vpx_codec_dec_init(&codec, decoder->codec_interface(), NULL, 0);
  this->deviceName = "";
}

VP9Decoder::~VP9Decoder()
{
  vpx_codec_destroy(&codec);
  decoder = NULL;
}

int VP9Decoder::DecodeVideoMSGIntoSingleFrame(igtl::VideoMessage* videoMessage, SourcePicture* pDecodedPic)
{
  if(videoMessage->GetBitStreamSize())
    {
    igtl_int32 iWidth = videoMessage->GetWidth();
    igtl_int32 iHeight = videoMessage->GetHeight();
    igtl_uint64 iStreamSize = videoMessage->GetBitStreamSize();
    pDecodedPic->picWidth = iWidth;
    pDecodedPic->picHeight = iHeight;
    if (!vpx_codec_decode(&codec, videoMessage->GetPackFragmentPointer(2), (unsigned int)iStreamSize, NULL, 0))
      {
      iter = NULL;
      if ((outputImage = vpx_codec_get_frame(&codec, &iter)) != NULL)
        {
        int stride[3] = { outputImage->stride[0], outputImage->stride[1], outputImage->stride[2] };
        int convertedDimensions[2] = { vpx_img_plane_width(outputImage, 0) *
          ((outputImage->fmt & VPX_IMG_FMT_HIGHBITDEPTH) ? 2 : 1), vpx_img_plane_height(outputImage, 0) };
        if (outputImage->fmt == VPX_IMG_FMT_I420)
        {
            pDecodedPic->data[1]= pDecodedPic->data[0] + iWidth*iHeight;
            pDecodedPic->data[2]= pDecodedPic->data[1] + iWidth*iHeight/4;
            pDecodedPic->stride[0] = iWidth;
            pDecodedPic->stride[1] = pDecodedPic->stride[2] = iWidth>>1;
            pDecodedPic->stride[3] = 0;
            ComposeByteSteam(outputImage->planes, convertedDimensions, stride, pDecodedPic->data[0], FormatI420);
        }
        else if (outputImage->fmt == VPX_IMG_FMT_I444)
        {
            pDecodedPic->data[1]= pDecodedPic->data[0] + iWidth*iHeight;
            pDecodedPic->data[2]= pDecodedPic->data[1] + iWidth*iHeight;
            pDecodedPic->stride[0] = iWidth;
            pDecodedPic->stride[1] = pDecodedPic->stride[2] = iWidth;
            pDecodedPic->stride[3] = 0;
            ComposeByteSteam(outputImage->planes, convertedDimensions, stride, pDecodedPic->data[0], FormatI444);
        }
        else
        {
            return -1;
        }
        return 2;
        }
      }
    else
      {
      vpx_codec_dec_init(&codec, decoder->codec_interface(), NULL, 0);
      std::cerr << "decode failed" << std::endl;
      }
    return -1;
  }
  return -1;
}

void VP9Decoder::ComposeByteSteam(igtl_uint8** inputData, int dimension[2], int iStride[3], igtl_uint8 *outputFrame, VideoFormatType format)
{
  int plane;
  int dimensionW [3];
  int dimensionH [3];
  if(format==FormatI420)
  {
     dimensionW [0] = dimension[0];
     dimensionW [1] = dimension[0]/2;
     dimensionW [2] = dimension[0]/2;
     dimensionH [0] = dimension[1];
     dimensionH [1] = dimension[1]/2;
     dimensionH [2] = dimension[1]/2;
  }
  else if(format==FormatI444)
  {
     dimensionW [0] = dimension[0];
     dimensionW [1] = dimension[0];
     dimensionW [2] = dimension[0];
     dimensionH [0] = dimension[0];
     dimensionH [1] = dimension[1];
     dimensionH [2] = dimension[1];
  }
  else
  {
      return;
  }
  int shift = 0;
  for (plane = 0; plane < 3; ++plane)
    {
    const unsigned char *buf = inputData[plane];
    const int stride = iStride[plane];
    int w = dimensionW[plane];
    int h = dimensionH[plane];
    int y;
    for (y = 0; y < h; ++y)
      {
      memcpy(outputFrame+shift + w*y, buf, w);
      buf += stride;
      }
    shift += w*h;
    }
}


int VP9Decoder::DecodeBitStreamIntoFrame(unsigned char* bitstream,igtl_uint8* outputFrame, igtl_uint32 dimensions[2], igtl_uint64& iStreamSize)
{
  if (!vpx_codec_decode(&codec, bitstream, (unsigned int)iStreamSize, NULL, 0))
    {
    iter = NULL;
    if ((outputImage = vpx_codec_get_frame(&codec, &iter)) != NULL)
      {
      int stride[3] = { outputImage->stride[0], outputImage->stride[1], outputImage->stride[2] };
      int convertedDimensions[2] = { vpx_img_plane_width(outputImage, 0) *
        ((outputImage->fmt & VPX_IMG_FMT_HIGHBITDEPTH) ? 2 : 1), vpx_img_plane_height(outputImage, 0) };
      if (outputImage->fmt == VPX_IMG_FMT_I420)
      {
          ComposeByteSteam(outputImage->planes, convertedDimensions, stride, outputFrame, FormatI420);
      }
      else if (outputImage->fmt == VPX_IMG_FMT_I444)
      {
          ComposeByteSteam(outputImage->planes, convertedDimensions, stride, outputFrame, FormatI444);
      }
      else
      {
          return -1;
      }
      return 2;
      }
    }
  else
    {
    vpx_codec_dec_init(&codec, decoder->codec_interface(), NULL, 0);
    std::cerr << "decode failed" << std::endl;
    }
  return -1;
}

}// namespace igtl
