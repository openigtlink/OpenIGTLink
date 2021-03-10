/*=========================================================================
 
 Program:   H264Encoder
 Language:  C++
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

/*!
 * \copy
 *     Copyright (c)  2013, Cisco Systems
 *     All rights reserved.
 *
 *     Redistribution and use in source and binary forms, with or without
 *     modification, are permitted provided that the following conditions
 *     are met:
 *
 *        * Redistributions of source code must retain the above copyright
 *          notice, this list of conditions and the following disclaimer.
 *
 *        * Redistributions in binary form must reproduce the above copyright
 *          notice, this list of conditions and the following disclaimer in
 *          the documentation and/or other materials provided with the
 *          distribution.
 *
 *     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *     "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *     LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *     FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *     COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *     INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *     BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *     CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *     LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *     ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *     POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef __igtlH264Decoder_h
#define __igtlH264Decoder_h

#include <vector>
#include <limits.h>
#include <string.h>
#include "codec_api.h"
#include "codec_app_def.h"
#include "igtl_types.h"
#include "igtlVideoMessage.h"
#include "igtlCodecCommonClasses.h"

#define NO_DELAY_DECODING

namespace igtl {

class IGTLCommon_EXPORT H264Decoder:public GenericDecoder
{
public:
  igtlTypeMacro(H264Decoder, GenericDecoder);
  igtlNewMacro(H264Decoder);

  H264Decoder();
  ~H264Decoder();
  
  int DecodeBitStreamIntoFrame(unsigned char* bitStream,igtl_uint8* outputFrame, igtl_uint32 iDimensions[2], igtl_uint64 &iStreamSize) override;
  
  int DecodeVideoMSGIntoSingleFrame(igtl::VideoMessage* videoMessage, SourcePicture* pDecodedPic) override;
  
  int Process (void* pDst[3], SBufferInfo* pInfo, FILE* pFp);
  
private:
  
  void ComposeByteSteam(igtl_uint8** inputData, int dimension[2], int iStride[2], igtl_uint8 *outputFrame) override;
  
  ISVCDecoder* pDecoder;
  
  SDecodingParam decParam;
  
};


} // Namespace igtl
#endif