/*=========================================================================
 
 Program:   igtlAV1Encoder
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlAV1Encoder.h"

// AV1 includes
#include "aomcx.h"

namespace igtl {

static const AomInterfaceEncoder AV1StaticEncoder[] = {{&aom_codec_av1_cx}};

void igtlAV1Encoder::error_output(aom_codec_ctx_t *ctx, const char *s) {
  const char *detail = aom_codec_error_detail(ctx);
  
  printf("%s: %s\n", s, aom_codec_error(ctx));
  if (detail) printf("    %s\n", detail);
  exit(EXIT_FAILURE);
}


// TODO(dkovalev): move this function to aom_image.{c, h}, so it will be part
// of aom_image_t support, this section will be removed when it is moved to aom_image
int igtlAV1Encoder::aom_img_plane_width(const aom_image_t *img, int plane) {
  if (plane > 0 && img->x_chroma_shift > 0)
    return (img->d_w + 1) >> img->x_chroma_shift;
  else
    return img->d_w;
}

int igtlAV1Encoder::aom_img_plane_height(const aom_image_t *img, int plane) {
  if (plane > 0 && img->y_chroma_shift > 0)
    return (img->d_h + 1) >> img->y_chroma_shift;
  else
    return img->d_h;
}


igtlAV1Encoder::igtlAV1Encoder(char *configFile):GenericEncoder()
{
  this->encoder = &AV1StaticEncoder[0];
  codec = new aom_codec_ctx_t();
  encodedBuf = new aom_fixed_buf_t();
  inputImage = new aom_image_t();
  isLossLessLink = true;
  codecSpeed = 0;
  FillSpecificParameters ();
}

igtlAV1Encoder::~igtlAV1Encoder()
{
  aom_codec_encode(codec, NULL, -1, 1, 0); //Flush the codec
  aom_codec_destroy(codec);
  if (inputImage)
    {
    delete inputImage;
    }
  if(this->encoder)
    {
    this->encoder = NULL;
    }
}

int igtlAV1Encoder::FillSpecificParameters() {
  if (aom_codec_enc_config_default(encoder->codec_interface(), &cfg, 0))
    {
    error_output(codec, "Failed to get default codec config.");
    return -1;
    }
  cfg.g_error_resilient = true;
  aom_codec_enc_init(codec, encoder->codec_interface(), &cfg, 0);
  if(this->SetSpeed(FastestSpeed)!=0)
    {
    error_output(codec, "Failed to set the speed to be the fastest.");
    return -1;
    }
  return 0;
}

int igtlAV1Encoder::SetRCMode(int value)
{
  this->cfg.rc_end_usage = (aom_rc_mode) value;
  if(aom_codec_enc_config_set(codec, &this->cfg))
    {
    error_output(codec, "Failed to set RC mode");
    return -1;
    }
  return 0;
}

int igtlAV1Encoder::SetKeyFrameDistance(int frameNum)
{
  this->cfg.kf_max_dist = frameNum;
  this->cfg.kf_min_dist = frameNum;
  if(aom_codec_enc_config_set(codec, &this->cfg))
    {
    error_output(codec, "Failed to key frame distance");
    return -1;
    }
  return 0;
}


int igtlAV1Encoder::SetRCTargetBitRate(unsigned int bitRate)
{
  //// The bit rate in AOM is in Kilo
  int bitRateInKilo = bitRate/1000;
  this->cfg.rc_target_bitrate = bitRateInKilo;
  if (aom_codec_enc_config_set(codec, &this->cfg))
    {
    error_output(codec, "Failed to set target bit rate");
    return -1;
    }
  this->initializationDone = true;
  return 0;
}

int igtlAV1Encoder::SetQP(int maxQP, int minQP)
{
  this->cfg.rc_max_quantizer = maxQP<63?maxQP:63;
  this->cfg.rc_min_quantizer = minQP>0?minQP:0;
  this->cfg.rc_end_usage = AOM_Q;
  if (aom_codec_enc_config_set(codec, &this->cfg))
    {
    error_output(codec, "Failed to set QP");
    return -1;
    }
  this->initializationDone = true;
  return 0;
}

int igtlAV1Encoder::SetLosslessLink(bool linkMethod)
{
  this->isLossLessLink = linkMethod;
  if (aom_codec_control_(codec, AV1E_SET_LOSSLESS, linkMethod))
    {
    error_output(codec, "Failed to set lossless mode");
    return -1;
    }
  else
    {
    this->cfg.rc_end_usage = AOM_VBR;
    return 0;
    }
}

int igtlAV1Encoder::SetSpeed(int speed)
{
  this->codecSpeed = speed;
  if (speed>=SlowestSpeed && speed<=FastestSpeed)
    {  
    aom_codec_control(codec, AOME_SET_CPUUSED, speed);
    return 0;
    }
  return -1;
}

int igtlAV1Encoder::InitializeEncoder()
{
  cfg.g_lag_in_frames = 0;
  cfg.g_w = this->GetPicWidth();
  cfg.g_h = this->GetPicHeight();
  aom_img_alloc(inputImage, AOM_IMG_FMT_I420, cfg.g_w,
                cfg.g_h, 1);
  if (aom_codec_enc_init(codec, encoder->codec_interface(), &cfg, 0))
    {
    error_output(codec, "Failed to initialize encoder");
    return -1;
    }
  
  if (aom_codec_control_(codec, AV1E_SET_LOSSLESS, this->GetLosslessLink()))
    {
    error_output(codec, "Failed to set lossless mode");
    return -1;
    }
  
  if (codecSpeed >= SlowestSpeed && codecSpeed <= FastestSpeed)
    {
    if (aom_codec_control(codec, AOME_SET_CPUUSED, codecSpeed))
      {
      error_output(codec, "Failed to set Speed");
      return -1;
      }
    }
  this->initializationDone = true;
  return 0;
}

int igtlAV1Encoder::SetPicWidthAndHeight(unsigned int width, unsigned int height)
{
  this->picWidth = width;
  this->picHeight = height;
  if(this->picHeight != this->cfg.g_h || this->picWidth != this->cfg.g_w )
    {
    bool iRet = this->InitializeEncoder();
    if(iRet==0)
      {
      return 0;
      }
    }
  return 0;
}

int igtlAV1Encoder::ConvertToLocalImageFormat(SourcePicture* pSrcPic)
{
  if (pSrcPic->picWidth != this->cfg.g_w || pSrcPic->picHeight != this->cfg.g_h)
    {
    this->SetPicWidthAndHeight(pSrcPic->picWidth,pSrcPic->picHeight);
    this->InitializeEncoder();
    }
  int plane;
  for (plane = 0; plane < 3; ++plane)
    {
    unsigned char *buf = inputImage->planes[plane];
    const int w = aom_img_plane_width(inputImage, plane) *
    ((inputImage->fmt & AOM_IMG_FMT_HIGHBITDEPTH) ? 2 : 1);
    const int h = aom_img_plane_height(inputImage, plane);
    memcpy(buf, pSrcPic->data[plane], w*h);
    }
  
  return 1;
}

int igtlAV1Encoder::EncodeSingleFrameIntoVideoMSG(SourcePicture* pSrcPic, igtl::VideoMessage* videoMessage, bool isGrayImage)
{
  int iSourceWidth = pSrcPic->picWidth;
  int iSourceHeight = pSrcPic->picHeight;
  if (iSourceWidth != this->cfg.g_w || iSourceHeight != this->cfg.g_h)
    {
    this->SetPicWidthAndHeight(iSourceWidth,iSourceHeight);
    }
  if (this->initializationDone == true)
    {
    static igtl_uint32 messageID = 6;
    messageID ++;
    this->ConvertToLocalImageFormat(pSrcPic);
    const aom_codec_err_t res2 = aom_codec_encode(codec, inputImage, messageID, 1, 0);
    if (res2 != AOM_CODEC_OK)
      {
      error_output(codec, "Failed to encode frame");
      return -1;
      }
    iter = NULL;
    while ((pkt = aom_codec_get_cx_data(codec, &iter)) != NULL)
      {
      if (pkt->kind == AOM_CODEC_CX_FRAME_PKT)
        {
        if((pkt->data.frame.flags & AOM_FRAME_IS_KEY) != 0)
          {
          encodedFrameType = FrameTypeKey;
          }
        else
          {
          // To do, assign other frame type too.
          encodedFrameType = FrameTypeUnKnown;
          }
        videoMessage->SetBitStreamSize(pkt->data.frame.sz);
        videoMessage->AllocateScalars();
        videoMessage->SetCodecType(IGTL_VIDEO_CODEC_NAME_AV1);
        videoMessage->SetEndian(igtl_is_little_endian() == true ? 2 : 1); //little endian is 2 big endian is 1
        videoMessage->SetWidth(pSrcPic->picWidth);
        videoMessage->SetHeight(pSrcPic->picHeight);
        if (isGrayImage)
          {
          encodedFrameType = encodedFrameType << 8;
          }
        videoMessage->SetFrameType(encodedFrameType);
        memcpy(videoMessage->GetPackFragmentPointer(2), pkt->data.frame.buf, pkt->data.frame.sz);
        videoMessage->Pack();
      }
    }
    if (videoMessage->GetBitStreamSize())
      {
      return 0;
      }
    return -1;
    }
  return -1;
}

} // namespace igtl
