/*=========================================================================

  Program:   OpenIGTLink
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igtlI420Encoder_h
#define __igtlI420Encoder_h

#include "igtlCodecCommonClasses.h"

namespace igtl {

using namespace std;

class IGTLCommon_EXPORT I420Encoder: public GenericEncoder
{
public:
  igtlTypeMacro(I420Encoder, GenericEncoder);
  igtlNewMacro(I420Encoder);

  I420Encoder(char * configFile = NULL);
  ~I420Encoder();
  
  int InitializeEncoder() override { return 0; };

  /**
   Encode a frame, for performance issue, before encode the frame, make sure the frame pointer is updated with a new frame.
   Otherwize, the old frame will be encoded.
   */
  int EncodeSingleFrameIntoVideoMSG(SourcePicture* pSrcPic, igtl::VideoMessage* videoMessage, bool isGrayImage = false ) override;
};


}// Namespace igtl
#endif
