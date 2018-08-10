/*=========================================================================

Program:   OpenIGTLink
Language:  C++

Copyright (c) Insight Software Consortium. All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlI420Decoder_h
#define __igtlI420Decoder_h

#include "igtl_types.h"
#include "igtlVideoMessage.h"
#include "igtlCodecCommonClasses.h"

namespace igtl {

#define NO_DELAY_DECODING
  class IGTLCommon_EXPORT I420Decoder :public GenericDecoder
  {
  public:
    igtlTypeMacro(I420Decoder, GenericDecoder);
    igtlNewMacro(I420Decoder);

    I420Decoder();
    ~I420Decoder();

    virtual int DecodeBitStreamIntoFrame(unsigned char* bitStream, igtl_uint8* outputFrame, igtl_uint32 iDimensions[2], igtl_uint64 &iStreamSize) { return -1; };
    virtual int DecodeVideoMSGIntoSingleFrame(igtl::VideoMessage* videoMessage, SourcePicture* pDecodedPic);

  };

} // namespace igtl

#endif
