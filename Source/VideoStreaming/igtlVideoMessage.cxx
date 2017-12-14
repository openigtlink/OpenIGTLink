/*=========================================================================
 
 Program:   The OpenIGTLink Library
 Language:  C++
 Web page:  http://openigtlink.org/
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlVideoMessage.h"

#include <string.h>

namespace igtl {
  
  
  StartVideoMessage::~StartVideoMessage()
  {
  }
  
  int StartVideoMessage::GetBodyPackSize()
  {
    return IGTL_STT_VIDEO_SIZE;
  }
  
  int StartVideoMessage::CalculateContentBufferSize()
  {
    return IGTL_STT_VIDEO_SIZE;
  }
  
  int StartVideoMessage::PackContent()
  {
    AllocatePack();
    
    igtl_stt_video* stt_video = (igtl_stt_video*)this->m_Content;
    strncpy(stt_video->codec, this->m_CodecType.c_str(), IGTL_VIDEO_CODEC_NAME_SIZE);
    stt_video->time_interval = this->m_TimeInterval;
    stt_video->useCompress = this->m_UseCompress;
    
    igtl_stt_video_convert_byte_order(stt_video);
    
    return 1;
  
  }
  
  
  int StartVideoMessage::UnpackContent()
  {
    igtl_stt_video* stt_video = (igtl_stt_video*)this->m_Content;
    
    igtl_stt_video_convert_byte_order(stt_video);
    
    this->m_TimeInterval = stt_video->time_interval;
    this->m_UseCompress = stt_video->useCompress;
    this->m_CodecType = std::string(stt_video->codec);
    return 1;
  }
  
  
  
  VideoMessage::VideoMessage():
  MessageBase()
  {
    endian        = ENDIAN_BIG;
    scalarType    = TYPE_UINT8;
    m_FrameHeader = NULL;
    m_Frame       = NULL;
    
    m_SendMessageType  = "VIDEO";
    this->m_CodecType = IGTL_VIDEO_CODEC_NAME_VP9;
    this->useCompress = true;
    this->additionalZDimension = 1;
    ScalarSizeTable[0] = 0;
    ScalarSizeTable[1] = 0;
    ScalarSizeTable[2] = sizeof(igtlInt8);    // TYPE_INT8
    ScalarSizeTable[3] = sizeof(igtlUint8);   // TYPE_UINT8
    ScalarSizeTable[4] = sizeof(igtlInt16);   // TYPE_INT16
    ScalarSizeTable[5] = sizeof(igtlUint16);  // TYPE_UINT16
    ScalarSizeTable[6] = sizeof(igtlInt32);   // TYPE_INT32
    ScalarSizeTable[7] = sizeof(igtlUint32);  // TYPE_UINT32
    ScalarSizeTable[8] = 0;                   // not defined
    ScalarSizeTable[9] = 0;                   // not defined
    
    this->m_Body   = NULL;
    m_Frame = NULL;
    m_FrameHeader = NULL;
  }
  
  
  VideoMessage::~VideoMessage()
  {
  }
  
  int VideoMessage::SetCodecType(const char codecType[])
  {
    if (strcmp(codecType, IGTL_VIDEO_CODEC_NAME_X265)==0 || strcmp(codecType, IGTL_VIDEO_CODEC_NAME_VP9)==0 || strcmp(codecType, IGTL_VIDEO_CODEC_NAME_H264)==0 || strcmp(codecType, IGTL_VIDEO_CODEC_NAME_OPENHEVC)==0)
      {
      this->m_CodecType = std::string(codecType);
      return 0;
      }
    else
      {
      return -1;
      }
  }
  
  /// This should only be called when the data is unpacked
  int VideoMessage::GetBitStreamSize()
  {
  #if OpenIGTLink_HEADER_VERSION >= 2
    if (m_HeaderVersion == IGTL_HEADER_VERSION_2)
      {
      return GetPackBodySize()-IGTL_VIDEO_HEADER_SIZE - IGTL_EXTENDED_HEADER_SIZE - GetMetaDataHeaderSize() - GetMetaDataSize();
      }
    else
      {
      return GetPackBodySize()-IGTL_VIDEO_HEADER_SIZE;
      }
  #else
    return GetPackBodySize()-IGTL_VIDEO_HEADER_SIZE;
  #endif
  };
  
  void VideoMessage::SetBitStreamSize(int size)
  {
    bitStreamSize = size;
  #if OpenIGTLink_HEADER_VERSION >= 2
    if (m_HeaderVersion == IGTL_HEADER_VERSION_2)
      {
      m_MessageSize = IGTL_HEADER_SIZE + bitStreamSize + IGTL_EXTENDED_HEADER_SIZE + GetMetaDataHeaderSize() + GetMetaDataSize() + IGTL_VIDEO_HEADER_SIZE;
      }
    else
      {
      m_MessageSize = size + IGTL_VIDEO_HEADER_SIZE + IGTL_HEADER_SIZE;
      
      }
  #else
    m_MessageSize = size + IGTL_HEADER_SIZE;
  #endif
  };
  
  // Get the packed bit stream size
  int VideoMessage::GetPackedBitStreamSize()
  {
    if(m_IsBodyPacked)
      return bitStreamSize;
    return -1;
  }
  
  void VideoMessage::AllocateScalars()
  {
    MessageBase::AllocateBuffer();
  #if OpenIGTLink_HEADER_VERSION >= 2
    if (m_HeaderVersion == IGTL_HEADER_VERSION_2)
      {
      this->m_FrameHeader = &m_Body[IGTL_EXTENDED_HEADER_SIZE];
      }
    else
      {
      this->m_FrameHeader = m_Body;
      }
    this->m_Frame  = &m_FrameHeader[IGTL_VIDEO_HEADER_SIZE];
  #else
    this->m_FrameHeader = m_Body;
    this->m_Frame = m_Body;
  #endif
  }
  
  
  int VideoMessage::CalculateContentBufferSize()
  {
    return  IGTL_VIDEO_HEADER_SIZE + GetBitStreamSize();
  }
  
  unsigned char* VideoMessage::GetPackFragmentPointer(int id)
  {
    if (id == 0)
      {
      return this->m_Header;
      }
    else if (id == 1)
      {
      return this->m_FrameHeader;
      }
    else if (id == 2)
      {
      return this->m_Frame;
      }
    return NULL;
  }
  
  
  int VideoMessage::GetPackFragmentSize(int id)
  {
    if (id == 0)
      {
      return IGTL_HEADER_SIZE;
      }
    else if (id == 1)
      {
      return IGTL_VIDEO_HEADER_SIZE;
      }
    else if (id == 2)
      {
      return GetBitStreamSize();
      }
    return 0;
  }
  
  int VideoMessage::PackContent()
  {
    igtl_frame_header* frame_header = (igtl_frame_header*)this->m_FrameHeader;
    
    frame_header->version           = IGTL_VIDEO_HEADER_VERSION;
    strncpy(frame_header->codec, this->m_CodecType.c_str(), IGTL_VIDEO_CODEC_NAME_SIZE);
    frame_header->scalar_type       = this->scalarType;
    frame_header->endian            = this->endian;
    frame_header->width             = this->width;
    frame_header->height            = this->height;
    frame_header->frameType         = this->videoFrameType;
    frame_header->useCompress       = this->useCompress;
    igtl_frame_convert_byte_order(frame_header);
  #if OpenIGTLink_HEADER_VERSION >= 2
    if (m_HeaderVersion == IGTL_HEADER_VERSION_2)
      {
      m_MetaDataHeader = &m_Body[CalculateContentBufferSize()+IGTL_EXTENDED_HEADER_SIZE];
      }
  #endif
    return 1;
  }
  
  
  int VideoMessage::UnpackContent()
  {
  #if OpenIGTLink_HEADER_VERSION >= 2
    if (m_HeaderVersion == IGTL_HEADER_VERSION_2)
      {
      this->m_FrameHeader = &m_Body[IGTL_EXTENDED_HEADER_SIZE];
      }
    else
      {
      this->m_FrameHeader = m_Body;
      }
    this->m_Frame  = &m_FrameHeader[IGTL_VIDEO_HEADER_SIZE];
  #else
    this->m_FrameHeader = m_Body;
    this->m_Frame = m_Body;
  #endif
    
    igtl_frame_header* frame_header = (igtl_frame_header*)m_FrameHeader;
    igtl_frame_convert_byte_order(frame_header);
    
    if (frame_header->version == IGTL_VIDEO_HEADER_VERSION)
      {
      // Video format version 1
      this->scalarType       = frame_header->scalar_type;
      this->m_CodecType.clear();
      this->m_CodecType.append(frame_header->codec, IGTL_VIDEO_CODEC_NAME_SIZE);
      this->endian           = frame_header->endian;
      this->width            = frame_header->width;
      this->height           = frame_header->height;
      this->videoFrameType   = frame_header->frameType;
      this->useCompress      = frame_header->useCompress;
      this->m_Frame = this->m_FrameHeader;
      this->m_Frame += IGTL_VIDEO_HEADER_SIZE;
      
      return 1;
      }
    else
      {
      // Incompatible version.
      return 0;
      }
  }
} // namespace igtl