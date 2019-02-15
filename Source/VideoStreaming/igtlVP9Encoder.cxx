/*=========================================================================
 
 Program:   VP9Encoder
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/


#include "igtlVP9Encoder.h"
#include "igtlVideoMessage.h"

namespace igtl {


static const VpxInterfaceEncoder vp9StaticEncoder[] = {{&vpx_codec_vp9_cx}};

void VP9Encoder::error_output(vpx_codec_ctx_t *ctx, const char *s) {
  const char *detail = vpx_codec_error_detail(ctx);
  
  printf("%s: %s\n", s, vpx_codec_error(ctx));
  if (detail) printf("    %s\n", detail);
  exit(EXIT_FAILURE);
}


// TODO(dkovalev): move this function to vpx_image.{c, h}, so it will be part
// of vpx_image_t support, this section will be removed when it is moved to vpx_image
int VP9Encoder::vpx_img_plane_width(const vpx_image_t *img, int plane) {
  if (plane > 0 && img->x_chroma_shift > 0)
    return (img->d_w + 1) >> img->x_chroma_shift;
  else
    return img->d_w;
}

int VP9Encoder::vpx_img_plane_height(const vpx_image_t *img, int plane) {
  if (plane > 0 && img->y_chroma_shift > 0)
    return (img->d_h + 1) >> img->y_chroma_shift;
  else
    return img->d_h;
}


VP9Encoder::VP9Encoder(char *configFile):GenericEncoder()
{
  this->encoder = &vp9StaticEncoder[0];
  codec = new vpx_codec_ctx_t();
  encodedBuf = new vpx_fixed_buf_t();
  inputImage = new vpx_image_t();
  deadlineMode = VPX_DL_REALTIME;
  isLossLessLink = true;
  codecSpeed = 0;
  FillSpecificParameters ();
  SetImageFormat(FormatI420);
}

VP9Encoder::~VP9Encoder()
{
  vpx_codec_encode(codec, NULL, -1, 1, 0, deadlineMode); //Flush the codec
  vpx_codec_destroy(codec);
  if (inputImage)
    {
    delete inputImage;
    }
  if(this->encoder)
    {
    this->encoder = NULL;
    }
}

int VP9Encoder::FillSpecificParameters() {
  if (vpx_codec_enc_config_default(encoder->codec_interface(), &cfg, 0))
    {
    error_output(codec, "Failed to get default codec config.");
    return -1;
    }
  cfg.g_error_resilient = true;
  vpx_codec_enc_init(codec, encoder->codec_interface(), &cfg, 0);
  if(this->SetSpeed(FastestSpeed)!=0)
    {
    error_output(codec, "Failed to set the speed to be the fastest.");
    return -1;
    }
  return 0;
}

int VP9Encoder::SetRCMode(int value)
{
  this->cfg.rc_end_usage = (vpx_rc_mode) value;
  if(vpx_codec_enc_config_set(codec, &this->cfg))
    {
    error_output(codec, "Failed to set RC mode");
    return -1;
    }
  return 0;
}

int VP9Encoder::SetImageFormat(VideoFormatType value)
{
  if (value == FormatI420)
  {
     this->cfg.g_profile = 0;
     inputImage->fmt = VPX_IMG_FMT_I420;
  }
  else if (value == FormatI444)
  {
     this->cfg.g_profile = 1;
     inputImage->fmt = VPX_IMG_FMT_I444;
  }
  else
  {
      error_output(codec, "Not supported image format");
      return -1;
  }
  return 0;
}

int VP9Encoder::SetKeyFrameDistance(int frameNum)
{
  this->cfg.kf_max_dist = frameNum;
  this->cfg.kf_min_dist = frameNum;
  if(vpx_codec_enc_config_set(codec, &this->cfg))
    {
    error_output(codec, "Failed to key frame distance");
    return -1;
    }
  return 0;
}


int VP9Encoder::SetRCTaregetBitRate(unsigned int bitRate)
{
  // The bit rate in VPX is in Kilo
  int bitRateInKilo = bitRate/1000;
  this->cfg.rc_target_bitrate = bitRateInKilo;
  this->cfg.layer_target_bitrate[0] = bitRateInKilo;
  for (unsigned int i = 0; i < this->cfg.ss_number_layers; i++)
    {
    this->cfg.ss_target_bitrate[i] = bitRateInKilo/this->cfg.ss_number_layers;
    }
  if (vpx_codec_enc_config_set(codec, &this->cfg))
    {
    error_output(codec, "Failed to set target bit rate");
    return -1;
    }
  this->initializationDone = true;
  return 0;
}

int VP9Encoder::SetQP(int maxQP, int minQP)
{
  this->cfg.rc_max_quantizer = maxQP<63?maxQP:63;
  this->cfg.rc_min_quantizer = minQP>0?minQP:0;
  this->cfg.rc_end_usage = VPX_Q;
  if (vpx_codec_enc_config_set(codec, &this->cfg))
    {
    error_output(codec, "Failed to set QP");
    return -1;
    }
  this->initializationDone = true;
  return 0;
}

int VP9Encoder::SetLosslessLink(bool linkMethod)
{
  this->SetDeadlineMode(VPX_DL_GOOD_QUALITY);
  this->isLossLessLink = linkMethod;
  if (vpx_codec_control_(codec, VP9E_SET_LOSSLESS, linkMethod))
    {
    error_output(codec, "Failed to set lossless mode");
    return -1;
    }
  else
    {
    this->cfg.rc_end_usage = VPX_VBR;
    return 0;
    }
}

int VP9Encoder::SetSpeed(int speed)
{
  this->SetDeadlineMode(VPX_DL_REALTIME);
  this->codecSpeed = speed;
  if (speed>=SlowestSpeed && speed<=FastestSpeed)
    {
    vpx_codec_control(codec, VP8E_SET_CPUUSED, speed);
    return 0;
    }
  return -1;
}

int VP9Encoder::InitializeEncoder()
{
  cfg.g_lag_in_frames = 0;
  cfg.g_w = this->GetPicWidth();
  cfg.g_h = this->GetPicHeight();
  if (inputImage->fmt == VPX_IMG_FMT_NONE)
  {
      return -1;
  }
  vpx_img_alloc(inputImage, inputImage->fmt, cfg.g_w,
                cfg.g_h, 1);
  if (vpx_codec_enc_init(codec, encoder->codec_interface(), &cfg, 0))
    {
    error_output(codec, "Failed to initialize encoder");
    return -1;
    }
  
  if (vpx_codec_control_(codec, VP9E_SET_LOSSLESS, this->GetLosslessLink()))
    {
    error_output(codec, "Failed to set lossless mode");
    return -1;
    }
  
  if (codecSpeed >= SlowestSpeed && codecSpeed <= FastestSpeed)
    {
    if (vpx_codec_control(codec, VP8E_SET_CPUUSED, codecSpeed))
      {
      error_output(codec, "Failed to set Speed");
      return -1;
      }
    }
  this->initializationDone = true;
  return 0;
}

int VP9Encoder::SetPicWidthAndHeight(unsigned int width, unsigned int height)
{
  this->picWidth = width;
  this->picHeight = height;
  if(this->picHeight != this->cfg.g_h || this->picWidth != this->cfg.g_w )
    {
    bool iRet = (this->InitializeEncoder() == 1);
    if(iRet==0)
      {
      return 0;
      }
    }
  return 0;
}

int VP9Encoder::SetDeadlineMode(unsigned long mode)
{
  this->deadlineMode = mode;
  return 0;
}

int VP9Encoder::ConvertToLocalImageFormat(SourcePicture* pSrcPic)
{
  if (pSrcPic->picWidth != this->cfg.g_w || pSrcPic->picHeight != this->cfg.g_h)
    {
    this->SetPicWidthAndHeight(pSrcPic->picWidth,pSrcPic->picHeight);
    this->InitializeEncoder();
    }
  int plane;
  for (plane = 0; plane < 3; ++plane)
    {
    unsigned char *buf ;
    const int w = vpx_img_plane_width(inputImage, plane) *
    ((inputImage->fmt & VPX_IMG_FMT_HIGHBITDEPTH) ? 2 : 1);
    const int h = vpx_img_plane_height(inputImage, plane);
    buf = inputImage->planes[plane];
    memcpy(buf, pSrcPic->data[plane], w*h);
    }
  
  return 1;
}

int VP9Encoder::EncodeSingleFrameIntoVideoMSG(SourcePicture* pSrcPic, igtl::VideoMessage* videoMessage, bool isGrayImage)
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
    const vpx_codec_err_t res2 = vpx_codec_encode(codec, inputImage, messageID, 1, 0, this->deadlineMode);
    if (res2 != VPX_CODEC_OK)
      {
      error_output(codec, "Failed to encode frame");
      return -1;
      }
    iter = NULL;
    while ((pkt = vpx_codec_get_cx_data(codec, &iter)) != NULL)
      {
      if (pkt->kind == VPX_CODEC_CX_FRAME_PKT)
        {
        if((pkt->data.frame.flags & VPX_FRAME_IS_KEY) != 0)
          {
          encodedFrameType = FrameTypeKey;
          }
        else
          {
          // To do, assign other frame type too.
          encodedFrameType = FrameTypeUnKnown;
          }
        videoMessage->SetFrameType(encodedFrameType);
        videoMessage->SetBitStreamSize(pkt->data.frame.sz);
        videoMessage->AllocateScalars();
        videoMessage->SetCodecType(IGTL_VIDEO_CODEC_NAME_VP9);
        int endian = (igtl_is_little_endian() == 1 ? IGTL_VIDEO_ENDIAN_LITTLE : IGTL_VIDEO_ENDIAN_BIG);
        videoMessage->SetEndian(endian); //little endian is 2 big endian is 1
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
    }
  return -1;
}


}// namespace igtl


