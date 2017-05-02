/*=========================================================================
 
 Program:   VPXEncoder
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/


#include "VPXEncoder.h"
#include "igtlVideoMessage.h"

void usage_exit(void) {exit(EXIT_FAILURE);};

VPXEncoder::VPXEncoder(char *configFile):GenericEncoder()
{
  this->encoder = get_vpx_encoder_by_name("vp9");
  codec = new vpx_codec_ctx_t();
  encodedBuf = new vpx_fixed_buf_t();
  inputImage = new vpx_image_t();
  deadlineMode = VPX_DL_REALTIME;
  isLossLessLink = true;
  codecSpeed = 0;
  FillSpecificParameters ();
}

VPXEncoder::~VPXEncoder()
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

int VPXEncoder::FillSpecificParameters() {
  if (vpx_codec_enc_config_default(encoder->codec_interface(), &cfg, 0))
  {
    die_codec(codec, "Failed to get default codec config.");
    return -1;
  }
  cfg.g_error_resilient = true;
  vpx_codec_enc_init(codec, encoder->codec_interface(), &cfg, 0);
  if(this->SetSpeed(FastestSpeed)!=0)
  {
    die_codec(codec, "Failed to set the speed to be the fastest.");
    return -1;
  }
  return 0;
}

int VPXEncoder::SetRCMode(int value)
{
  this->cfg.rc_end_usage = (vpx_rc_mode) value;
  if(vpx_codec_enc_config_set(codec, &this->cfg))
  {
    die_codec(codec, "Failed to set RC mode");
    return -1;
  }
  return 0;
}

int VPXEncoder::SetKeyFrameDistance(int frameNum)
{
  this->cfg.kf_max_dist = frameNum;
  this->cfg.kf_min_dist = frameNum;
  if(vpx_codec_enc_config_set(codec, &this->cfg))
  {
    die_codec(codec, "Failed to key frame distance");
    return -1;
  }
  return 0;
}


int VPXEncoder::SetRCTaregetBitRate(unsigned int bitRate)
{
  // The bit rate in VPX is in Kilo
  int bitRateInKilo = bitRate/1000;
  this->cfg.rc_target_bitrate = bitRateInKilo;
  this->cfg.layer_target_bitrate[0] = bitRateInKilo;
  for (int i = 0; i < this->cfg.ss_number_layers; i++)
  {
    this->cfg.ss_target_bitrate[i] = bitRateInKilo/this->cfg.ss_number_layers;
  }
  if (vpx_codec_enc_config_set(codec, &this->cfg))
  {
    die_codec(codec, "Failed to set target bit rate");
    return -1;
  }
  this->initializationDone = true;
  return 0;
}

int VPXEncoder::SetQP(int maxQP, int minQP)
{
  this->cfg.rc_max_quantizer = maxQP<63?maxQP:63;
  this->cfg.rc_min_quantizer = minQP>0?minQP:0;
  this->cfg.rc_end_usage = VPX_Q;
  if (vpx_codec_enc_config_set(codec, &this->cfg))
  {
    die_codec(codec, "Failed to set QP");
    return -1;
  }
  this->initializationDone = true;
  return 0;
}

int VPXEncoder::SetLosslessLink(bool linkMethod)
{
  this->isLossLessLink = linkMethod;
  if (vpx_codec_control_(codec, VP9E_SET_LOSSLESS, linkMethod))
  {
    die_codec(codec, "Failed to set lossless mode");
    return -1;
  }
  else
  {
    this->cfg.rc_end_usage = VPX_VBR;
    return 0;
  }
}

int VPXEncoder::SetSpeed(int speed)
{
  this->codecSpeed = speed;
  if (speed>=SlowestSpeed && speed<=FastestSpeed)
  {
    vpx_codec_control(codec, VP8E_SET_CPUUSED, speed);
    return 0;
  }
  return -1;
}

int VPXEncoder::InitializeEncoder()
{
  cfg.g_lag_in_frames = 0;
  cfg.g_w = this->GetPicWidth();
  cfg.g_h = this->GetPicHeight();
  vpx_img_alloc(inputImage, VPX_IMG_FMT_I420, cfg.g_w,
                cfg.g_h, 1);
  if (vpx_codec_enc_init(codec, encoder->codec_interface(), &cfg, 0))
  {
    die_codec(codec, "Failed to initialize encoder");
    return -1;
  }
  
  if (vpx_codec_control_(codec, VP9E_SET_LOSSLESS, this->GetLosslessLink()))
  {
    die_codec(codec, "Failed to set lossless mode");
    return -1;
  }

  if (codecSpeed >= SlowestSpeed && codecSpeed <= FastestSpeed)
  {
    if (vpx_codec_control(codec, VP8E_SET_CPUUSED, codecSpeed))
    {
      die_codec(codec, "Failed to set Speed");
      return -1;
    }
  }
  this->initializationDone = true;
  return 0;
}

int VPXEncoder::SetPicWidthAndHeight(unsigned int width, unsigned int height)
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

int VPXEncoder::SetDeadlineMode(unsigned long mode)
{
  this->deadlineMode = mode;
  return 0;
}

int VPXEncoder::ConvertToLocalImageFormat(SourcePicture* pSrcPic)
{
  if (pSrcPic->picWidth != this->cfg.g_w || pSrcPic->picHeight != this->cfg.g_h)
  {
    this->SetPicWidthAndHeight(pSrcPic->picWidth,pSrcPic->picHeight);
    this->InitializeEncoder();
  }
  int plane;
  for (plane = 0; plane < 3; ++plane) {
    unsigned char *buf = inputImage->planes[plane];
    const int w = vpx_img_plane_width(inputImage, plane) *
    ((inputImage->fmt & VPX_IMG_FMT_HIGHBITDEPTH) ? 2 : 1);
    const int h = vpx_img_plane_height(inputImage, plane);
    memcpy(buf, pSrcPic->data[plane], w*h);
  }
  
  return 1;
}

int VPXEncoder::EncodeSingleFrameIntoVideoMSG(SourcePicture* pSrcPic, igtl::VideoMessage* videoMessage, bool isGrayImage)
{
  int iSourceWidth = pSrcPic->picWidth;
  int iSourceHeight = pSrcPic->picHeight;
  if (iSourceWidth != this->cfg.g_w || iSourceHeight != this->cfg.g_h)
  {
    this->SetPicWidthAndHeight(iSourceWidth,iSourceHeight);
  }
  if (this->initializationDone == true)
  {
    int kiPicResSize = iSourceWidth * iSourceHeight * 3 >> 1;
    if(this->useCompress)
    {
      static igtl_uint32 messageID = 6;
      messageID ++;
      this->ConvertToLocalImageFormat(pSrcPic);
      const vpx_codec_err_t res2 = vpx_codec_encode(codec, inputImage, messageID, 1, 0, this->deadlineMode);
      if (res2 != VPX_CODEC_OK)
      {
        die_codec(codec, "Failed to encode frame");
        return -1;
      }
      iter = NULL;
      while((pkt = vpx_codec_get_cx_data(codec, &iter)) != NULL) {
        if (pkt->kind == VPX_CODEC_CX_FRAME_PKT) {
          encodedFrameType = FrameTypeKey;
          videoMessage->SetBitStreamSize(pkt->data.frame.sz);
          videoMessage->AllocateScalars();
          videoMessage->SetScalarType(videoMessage->TYPE_UINT8);
          videoMessage->SetEndian(igtl_is_little_endian()==true?2:1); //little endian is 2 big endian is 1
          videoMessage->SetWidth(pSrcPic->picWidth);
          videoMessage->SetHeight(pSrcPic->picHeight);
          if (isGrayImage)
          {
            encodedFrameType = encodedFrameType<<8;
          }
          videoMessage->SetFrameType(encodedFrameType);
          videoMessage->SetMessageID(messageID);
          memcpy(videoMessage->GetPackFragmentPointer(2), pkt->data.frame.buf, pkt->data.frame.sz);
          videoMessage->Pack();
        }
      }
      if(videoMessage->GetBitStreamSize())
      {
        return 0;
      }
      return -1;
    }
    else
    {
      static igtl_uint32 messageID = -1;
      videoMessage->SetBitStreamSize(kiPicResSize);
      videoMessage->AllocateScalars();
      videoMessage->SetScalarType(videoMessage->TYPE_UINT8);
      videoMessage->SetEndian(igtl_is_little_endian()==true?IGTL_VIDEO_ENDIAN_LITTLE:IGTL_VIDEO_ENDIAN_BIG); //little endian is 2 big endian is 1
      videoMessage->SetWidth(pSrcPic->picWidth);
      videoMessage->SetHeight(pSrcPic->picHeight);
      encodedFrameType = FrameTypeKey;
      if (isGrayImage)
      {
        encodedFrameType = FrameTypeKey<<8;
      }
      videoMessage->SetFrameType(encodedFrameType);
      messageID ++;
      videoMessage->SetMessageID(messageID);
      memcpy(videoMessage->GetPackFragmentPointer(2), pSrcPic->data[0], kiPicResSize);
      videoMessage->Pack();
    }
  }
  return -1;
}
