/*=========================================================================
 
 Program:   AOMEncoder
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlAV1Decoder.h"

// AV1 includes
#include "aomdx.h"
namespace igtl {

static const AomInterfaceDecoder AV1StaticDecoder[] = { { &aom_codec_av1_dx } };

int igtlAV1Decoder::aom_img_plane_width(const aom_image_t *img, int plane) {
  if (plane > 0 && img->x_chroma_shift > 0)
    return (img->d_w + 1) >> img->x_chroma_shift;
  else
    return img->d_w;
}

int igtlAV1Decoder::aom_img_plane_height(const aom_image_t *img, int plane) {
  if (plane > 0 && img->y_chroma_shift > 0)
    return (img->d_h + 1) >> img->y_chroma_shift;
  else
    return img->d_h;
}


igtlAV1Decoder::igtlAV1Decoder()
{
  decoder = &AV1StaticDecoder[0];
  aom_codec_dec_init(&codec, decoder->codec_interface(), NULL, 0);
  this->deviceName = "";
}

igtlAV1Decoder::~igtlAV1Decoder()
{
  aom_codec_destroy(&codec);
  decoder = NULL;
}

int igtlAV1Decoder::DecodeVideoMSGIntoSingleFrame(igtl::VideoMessage* videoMessage, SourcePicture* pDecodedPic)
{
  if(videoMessage->GetBitStreamSize())
    {
    igtl_int32 iWidth = videoMessage->GetWidth();
    igtl_int32 iHeight = videoMessage->GetHeight();
    igtl_uint64 iStreamSize = videoMessage->GetBitStreamSize();
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

void igtlAV1Decoder::ComposeByteSteam(igtl_uint8** inputData, int dimension[2], int iStride[3], igtl_uint8 *outputFrame)
{
  int plane;
  int dimensionW [3] = {dimension[0],dimension[0]/2,dimension[0]/2};
  int dimensionH [3] = {dimension[1],dimension[1]/2,dimension[1]/2};
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


int igtlAV1Decoder::DecodeBitStreamIntoFrame(unsigned char* bitstream, igtl_uint8* outputFrame, igtl_uint32 dimensions[2], igtl_uint64& iStreamSize)
{
  if (!aom_codec_decode(&codec, bitstream, (unsigned int)iStreamSize, NULL))
    {
    iter = NULL;
    if ((outputImage = aom_codec_get_frame(&codec, &iter)) != NULL)
      {
      int stride[3] = { outputImage->stride[0], outputImage->stride[1], outputImage->stride[2] };
      int convertedDimensions[2] = { aom_img_plane_width(outputImage, 0) *
        ((outputImage->fmt & AOM_IMG_FMT_HIGHBITDEPTH) ? 2 : 1), aom_img_plane_height(outputImage, 0) };
      ComposeByteSteam(outputImage->planes, convertedDimensions, stride, outputFrame);
      return 2;
      }
    }
  else
    {
    aom_codec_dec_init(&codec, decoder->codec_interface(), NULL, 0);
    std::cerr << "decode failed" << std::endl;
    }
  return -1;
}

} // namespace igtl
