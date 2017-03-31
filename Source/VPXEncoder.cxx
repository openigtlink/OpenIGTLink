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


/* Ctrl-C handler */
static int     g_iCtrlC = 0;
static void    SigIntHandler (int a) {
  g_iCtrlC = 1;
}

VPXEncoder::VPXEncoder(char *configFile):GenericEncoder()
{
  this->encoder = get_vpx_encoder_by_name("vp9");;
  FillSpecificParameters ();
}

VPXEncoder::~VPXEncoder()
{
  vpx_codec_encode(&codec, NULL, -1, 1, 0, VPX_DL_GOOD_QUALITY); //Flush the codec
  vpx_codec_destroy(&codec);
  this->encoder = NULL;
}

int VPXEncoder::FillSpecificParameters() {
  if (vpx_codec_enc_config_default(encoder->codec_interface(), &cfg, 0))
  {
    die_codec(&codec, "Failed to get default codec config.");
    return -1;
  }
  // Lossless link is by default true;
  return this->SetLosslessLink(true);
}

int VPXEncoder::ParseConfig() {
  return 0;
}

int VPXEncoder::SetLosslessLink(bool linkMethod)
{
  if (vpx_codec_control_(&codec, VP9E_SET_LOSSLESS, 1))
  {
    die_codec(&codec, "Failed to use lossless mode");
    this->isLossLessLink = false;
    return -1;
  }
  else
  {
    this->isLossLessLink = true;
    return 0;
  }
}

int VPXEncoder::InitializeEncoder()
{
  if (vpx_codec_enc_init(&codec, encoder->codec_interface(), &cfg, 0))
  {
    die_codec(&codec, "Failed to initialize encoder");
    this->initializationDone = false;
    return -1;
  }
  this->initializationDone = true;
  return 0;
}


void VPXEncoder::SetPicWidth(unsigned int width)
{
  this->picWidth = width;
  this->cfg.g_w = width;
}

void VPXEncoder::SetPicHeight(unsigned int height)
{
  this->picHeight = height;
  this->cfg.g_h = height;
}

int VPXEncoder::EncodeSingleFrameIntoVideoMSG(SourcePicture* pSrcPic, igtl::VideoMessage* videoMessage, bool isGrayImage)
{
  int iSourceWidth = pSrcPic->picWidth;
  int iSourceHeight = pSrcPic->picHeight;
  if (iSourceWidth != this->cfg.g_w || iSourceHeight != this->cfg.g_h)
  {
    this->SetPicWidth(iSourceWidth);
    this->SetPicHeight(iSourceHeight);
    this->InitializeEncoder();
  }
  if (this->initializationDone == true)
  {
    int kiPicResSize = iSourceWidth * iSourceHeight * 3 >> 1;
    if(this->useCompress)
    {
      static igtl_uint32 messageID = -1;
      messageID ++;
      int frameSize = 0;
      int iLayer = 0;
      vpx_img_alloc(&inputImage, VPX_IMG_FMT_I420, iSourceWidth,
                    iSourceHeight, 1);
      const vpx_codec_err_t res = vpx_codec_set_cx_data_buf(&codec,encodedBuf,0,0);
      if (res != VPX_CODEC_OK)
      {
        die_codec(&codec, "Failed to set output buffer");
        return -1;
      }
      vpx_codec_encode(&codec, &inputImage, messageID, 1, 0, VPX_DL_GOOD_QUALITY);
      if (res != VPX_CODEC_OK)
      {
        die_codec(&codec, "Failed to encode frame");
        return -1;
      }
      /*iter = NULL;
      pkt = NULL;
      int totalStreamLength = 0;
      while ((pkt = vpx_codec_get_cx_data(&codec, &iter)) != NULL) {
        if (pkt->kind == VPX_CODEC_CX_FRAME_PKT) {
          encodedFrameType = pkt->data.frame.flags;
          totalStreamLength += pkt->data.frame.sz;
        }
      }*/
      videoMessage->SetBitStreamSize(encodedBuf->sz);
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
      memcpy(videoMessage->GetPackFragmentPointer(2), encodedBuf->buf, encodedBuf->sz);
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
      encodedFrameType = FrameTypeIDR;
      if (isGrayImage)
      {
        encodedFrameType = FrameTypeIDR<<8;
      }
      videoMessage->SetFrameType(encodedFrameType);
      messageID ++;
      videoMessage->SetMessageID(messageID);
      memcpy(videoMessage->GetPackFragmentPointer(2), pSrcPic->data[0], kiPicResSize);
    }
    videoMessage->Pack();
    
  }
  return 0;
}
