/*=========================================================================
 
 Program:   OpenIGTLink
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/


#include "igtlI420Encoder.h"
#include "igtlVideoMessage.h"

namespace igtl {

I420Encoder::I420Encoder(char *configFile):GenericEncoder()
{
}

I420Encoder::~I420Encoder()
{
}

int I420Encoder::EncodeSingleFrameIntoVideoMSG(SourcePicture* pSrcPic, igtl::VideoMessage* videoMessage, bool isGrayImage)
{
  videoMessage->SetCodecType(IGTL_VIDEO_CODEC_NAME_I420);
  return this->PackUncompressedData(pSrcPic, videoMessage, isGrayImage);
}

}// namespace igtl
