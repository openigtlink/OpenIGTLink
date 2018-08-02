/*=========================================================================

Program:   OpenIGTLink
Language:  C++

Copyright (c) Insight Software Consortium. All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlI420Decoder.h"

namespace igtl {

  I420Decoder::I420Decoder()
  {
    this->deviceName = "";
  }

  I420Decoder::~I420Decoder()
  {
  }

  int I420Decoder::DecodeVideoMSGIntoSingleFrame(igtl::VideoMessage* videoMessage, SourcePicture* pDecodedPic)
  {
    return this->UnpackUncompressedData(videoMessage, pDecodedPic);
  }

}// namespace igtl
