/*=========================================================================
 
 Program:   H265Encoder
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlH265Encoder.h"
#include "igtlVideoMessage.h"
#include <sstream>

namespace igtl {

template <typename T>
std::string ToString(T variable)
{
  stringstream stream;
  stream << variable;
  return stream.str();
}

H265Encoder::H265Encoder(char *configFile):GenericEncoder()
{
  this->sSvcParam=x265_param_alloc();
  FillSpecificParameters();
  pSVCEncoder=x265_encoder_open(this->sSvcParam);
  H265SrcPicture = x265_picture_alloc();
  x265_picture_init(sSvcParam,H265SrcPicture);
  pNals = NULL;
  this->codecSpeed = 9;
}

H265Encoder::~H265Encoder()
{
  x265_encoder_close(this->pSVCEncoder);
  x265_picture_free(this->H265SrcPicture);
  x265_param_free(this->sSvcParam);
  this->pNals = NULL;
  this->pSVCEncoder = NULL;
  this->H265SrcPicture = NULL;
  this->sSvcParam = NULL;
}

int H265Encoder::FillSpecificParameters() {
  /* Test for temporal, spatial, SNR scalability */
  
  x265_param_default_preset(this->sSvcParam,ToString(9-this->codecSpeed).c_str(),"zerolatency");// second parameter is the speed.
  this->sSvcParam->internalCsp=X265_CSP_I420;
  this->sSvcParam->bRepeatHeaders=1;//write sps,pps before keyframe
  this->sSvcParam->fpsNum=200;
  this->sSvcParam->fpsDenom=1;
  this->sSvcParam->sourceWidth=1920;
  this->sSvcParam->sourceHeight=1080;
  this->sSvcParam->bEnableTemporalSubLayers = false;
  this->sSvcParam->maxNumReferences = 1;
  this->sSvcParam->maxNumMergeCand = 1;
  this->sSvcParam->bLossless = false;
  return 0;
}

int H265Encoder::SetRCTaregetBitRate(unsigned int bitRate)
{
  this->sSvcParam->rc.aqMode = X265_AQ_VARIANCE;
  this->sSvcParam->rc.rateControlMode = X265_RC_ABR;
  this->sSvcParam->rc.bitrate = bitRate/1000; // bit rate in h265 is represented in kbps
  /*for (int i = 0; i < this->sSvcParam.iSpatialLayerNum; i++)
   {
   this->sSvcParam.sSpatialLayers[i].iSpatialBitrate = bitRate/this->sSvcParam.iSpatialLayerNum;
   }*/
  x265_encoder_close(this->pSVCEncoder);
  pSVCEncoder=x265_encoder_open(this->sSvcParam);
  H265SrcPicture = x265_picture_alloc();
  x265_picture_init(sSvcParam,H265SrcPicture);
  /*if (x265_encoder_reconfig(this->pSVCEncoder, sSvcParam)<0) {
   fprintf (stderr, "Set target bit rate failed. \n");
   return -1;
   }*/
  this->initializationDone = true;
  return 0;
}

int H265Encoder::SetRCMode(int value)
{
  this->sSvcParam->rc.aqMode = X265_AQ_VARIANCE;
  this->sSvcParam->rc.rateControlMode = value; //X265_RC_ABR;
  if (x265_encoder_reconfig(this->pSVCEncoder, sSvcParam)<0) {
    fprintf (stderr, "Set RC mode failed. \n");
    return -1;
  }
  this->initializationDone = true;
  return 0;
}

int H265Encoder::SetQP(int maxQP, int minQP)
{
  sSvcParam->rc.qpMax= maxQP<51?maxQP:51;
  sSvcParam->rc.qpMin = minQP>0?minQP:0;
  if (x265_encoder_reconfig(this->pSVCEncoder, sSvcParam)<0)
    {
    fprintf (stderr, "Set QP value failed.\n");
    return -1;
    }
  this->initializationDone = true;
  return 0;
}

void H265Encoder::CopySettingToAnother(x265_param* srcSetting, x265_param* dstSetting)
{
  dstSetting->rc.qpMax = srcSetting->rc.qpMax;
  dstSetting->rc.qpMin = srcSetting->rc.qpMin;
  dstSetting->rc.aqMode = srcSetting->rc.aqMode;
  dstSetting->rc.rateControlMode = srcSetting->rc.rateControlMode;
  dstSetting->rc.bitrate = srcSetting->rc.bitrate;
  dstSetting->sourceWidth = srcSetting->sourceWidth;
  dstSetting->sourceHeight = srcSetting->sourceHeight;
  dstSetting->internalCsp = srcSetting->internalCsp;
  dstSetting->bRepeatHeaders = srcSetting->bRepeatHeaders;//write sps,pps before keyframe
  dstSetting->fpsNum = srcSetting->fpsNum;
  dstSetting->fpsDenom = srcSetting->fpsDenom;
  dstSetting->bEnableTemporalSubLayers = srcSetting->bEnableTemporalSubLayers;
  dstSetting->maxNumReferences = srcSetting->maxNumReferences;
  dstSetting->maxNumMergeCand = srcSetting->maxNumMergeCand;
  dstSetting->bLossless = srcSetting->bLossless;
}

int H265Encoder::SetSpeed(int speed)
{
  speed = speed>=0?speed:0;
  speed = speed<=9?speed:9;
  this->codecSpeed = speed;
  x265_param* previousSetting = x265_param_alloc();
  
  this->CopySettingToAnother(this->sSvcParam, previousSetting);
  x265_param_default_preset(this->sSvcParam,ToString(9-speed).c_str(),"zerolatency"); // In OpenIGTLink, lower speed value corresponding to slower coding speed. In x265, the speed setting is reversed.
  this->CopySettingToAnother(previousSetting, this->sSvcParam);
  x265_param_free(previousSetting);
  if (x265_encoder_reconfig(this->pSVCEncoder, sSvcParam)<0)
    {
    fprintf (stderr, "Set speed mode failed.\n");
    return -1;
    }
  return 0;
}

int H265Encoder::InitializeEncoder()
{
  //------------------------------------------------------------
  int iRet = 0;
  
  if (this->configFile=="" && this->pSVCEncoder)
    {
    fprintf (stderr, "No configuration file specified. Use Default Parameters\n");
    iRet = x265_encoder_reconfig(this->pSVCEncoder, sSvcParam);
    if (iRet<0)
      {
      fprintf (stderr, "parse svc parameter config file failed.\n");
      return -1;
      }
    this->initializationDone = true;
    return iRet;
    }
  this->initializationDone = false;
  return -1;
}

int H265Encoder::SetPicWidthAndHeight(unsigned int Width, unsigned int Height)
{
  this->sSvcParam->sourceWidth = Width;
  this->sSvcParam->sourceHeight = Height;
  if (x265_encoder_reconfig(this->pSVCEncoder, sSvcParam)<0)
    {
    fprintf (stderr, "parse svc parameter config file failed.\n");
    return -1;
    }
  x265_picture_init(sSvcParam,H265SrcPicture);
  this->initializationDone = true;
  return 0;
}

int H265Encoder::SetLosslessLink(bool linkMethod)
{
  this->sSvcParam->bLossless = linkMethod;
  x265_encoder_close(this->pSVCEncoder);
  pSVCEncoder=x265_encoder_open(this->sSvcParam);
  H265SrcPicture = x265_picture_alloc();
  x265_picture_init(sSvcParam,H265SrcPicture);
  return 0;
}


int H265Encoder::ConvertToLocalImageFormat(SourcePicture* pSrcPic)
{
  if(pSrcPic->colorFormat==FormatI420)
    {
    if((pSrcPic->picWidth*pSrcPic->picHeight*3/2 != this->H265SrcPicture->framesize || pSrcPic->picHeight != this->H265SrcPicture->height))
      {
      this->SetPicWidthAndHeight(pSrcPic->picWidth, pSrcPic->picHeight);
      x265_picture_init(this->sSvcParam, H265SrcPicture);
      }
    for(int i = 0; i < 3; i++)
      {
      H265SrcPicture->stride[i] = pSrcPic->stride[i];
      H265SrcPicture->planes[i] = pSrcPic->data[i];
      }
    return 1;
    }
  return -1;// image format not supported
}


int H265Encoder::EncodeSingleFrameIntoVideoMSG(SourcePicture* pSrcPic, igtl::VideoMessage* videoMessage, bool isGrayImage)
{
  int encodeRet = -1;
  int iSourceWidth = pSrcPic->picWidth;
  int iSourceHeight = pSrcPic->picHeight;
  if (iSourceWidth != this->sSvcParam->sourceWidth || iSourceHeight != this->sSvcParam->sourceHeight)
    {
    this->SetPicWidthAndHeight(iSourceWidth, iSourceHeight);
    this->InitializeEncoder();
    }
  if (this->initializationDone == true)
    {
    pSrcPic->stride[0] = iSourceWidth;
    pSrcPic->stride[1] = pSrcPic->stride[2] = pSrcPic->stride[0] >> 1;
    this->ConvertToLocalImageFormat(pSrcPic);

    igtl_uint32 iNal = 0;
    encodeRet =x265_encoder_encode(this->pSVCEncoder,&pNals,&iNal,this->H265SrcPicture,NULL);
    //encodeRet =x265_encoder_encode(this->pSVCEncoder,&pNals,&iNal,NULL,NULL);
    if (encodeRet>=1)
      {
      igtl_uint64 totalBitStreamSize = 0;
      for(int j=0;j<iNal;j++){
        totalBitStreamSize += pNals[j].sizeBytes;
      }
      if((pNals[0].type & X265_TYPE_IDR) != 0)
        {
        encodedFrameType = FrameTypeKey;
        }
      else
        {
        // To do, assign other frame type too.
        encodedFrameType = FrameTypeUnKnown;
        }
      videoMessage->SetFrameType(encodedFrameType);
      videoMessage->SetBitStreamSize(totalBitStreamSize);
      videoMessage->AllocateScalars();
      videoMessage->SetCodecType(IGTL_VIDEO_CODEC_NAME_X265);
      videoMessage->SetEndian(igtl_is_little_endian()==true?2:1); //little endian is 2 big endian is 1
      videoMessage->SetWidth(pSrcPic->picWidth);
      videoMessage->SetHeight(pSrcPic->picHeight);
      encodedFrameType = pNals[iNal-1].type;
      if (isGrayImage)
        {
        encodedFrameType = pNals[iNal-1].type<<8;
        }
      videoMessage->SetFrameType(encodedFrameType);
      int nalSize = 0;
      FILE* testFile = fopen("Test.265", "a");
      for(int j=0;j<iNal;j++){
        memcpy(&(videoMessage->GetPackFragmentPointer(2)[nalSize]), pNals[j].payload, pNals[j].sizeBytes);
        nalSize += pNals[j].sizeBytes;
        //fwrite(pNals[j].payload, 1 , pNals[j].sizeBytes, testFile);
      }
      fclose(testFile);
      videoMessage->Pack();
      return 0;
      }
    }
  return -1;
}

} // namespace igtl
