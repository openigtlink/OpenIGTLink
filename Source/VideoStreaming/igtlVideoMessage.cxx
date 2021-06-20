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
  
  igtlUint64 StartVideoMessage::CalculateContentBufferSize()
  {
    return IGTL_STT_VIDEO_SIZE;
  }
  
  int StartVideoMessage::PackContent()
  {
    AllocatePack();
    
    igtl_stt_video* stt_video = (igtl_stt_video*)this->m_Content;
    strncpy(stt_video->codec, this->m_CodecType.c_str(), IGTL_VIDEO_CODEC_NAME_SIZE);
    stt_video->time_interval = this->m_TimeInterval;
    
    igtl_stt_video_convert_byte_order(stt_video);
    
    return 1;
  
  }
  
  
  int StartVideoMessage::UnpackContent()
  {
    igtl_stt_video* stt_video = (igtl_stt_video*)this->m_Content;
    
    igtl_stt_video_convert_byte_order(stt_video);
    
    this->m_TimeInterval = stt_video->time_interval;
    this->m_CodecType = std::string(stt_video->codec);
    return 1;
  }
  
  
  
  VideoMessage::VideoMessage():
  MessageBase()
  {
    endian        = ENDIAN_BIG;
    m_FrameHeader = NULL;
    m_Frame       = NULL;

    for (int i = 0; i < 3; i++)
      {
      dimensions[i] = 0;
      spacing[i] = 0.0;
      subDimensions[i] = 0;
      subOffset[i] = 0;
      }
    dimensions[2] = 1;
    subDimensions[2] = 1;

    m_SendMessageType  = "VIDEO";
    this->m_CodecType = IGTL_VIDEO_CODEC_NAME_VP9;
    this->coordinate = COORDINATE_RAS;

    this->m_Body   = NULL;
    m_Frame = NULL;
    m_FrameHeader = NULL;
  }
  
  
  VideoMessage::~VideoMessage()
  {
  }
  
  int VideoMessage::SetCodecType(const char codecType[])
  {
    if (strcmp(codecType, IGTL_VIDEO_CODEC_NAME_I420) == 0 || strcmp(codecType, IGTL_VIDEO_CODEC_NAME_X265) == 0 || strcmp(codecType, IGTL_VIDEO_CODEC_NAME_VP9) == 0 || strcmp(codecType, IGTL_VIDEO_CODEC_NAME_H264) == 0 || strcmp(codecType, IGTL_VIDEO_CODEC_NAME_OPENHEVC) == 0)
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
  
  
  igtlUint64 VideoMessage::CalculateContentBufferSize()
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
    
    frame_header->header_version    = IGTL_VIDEO_HEADER_VERSION;
    strncpy(frame_header->encoding, this->m_CodecType.c_str(), IGTL_VIDEO_CODEC_NAME_SIZE);
    frame_header->endian            = this->endian;
    frame_header->frameType         = this->videoFrameType;
    frame_header->coord             = this->coordinate;

    float origin[3];
    float norm_i[3];
    float norm_j[3];
    float norm_k[3];

    for (int i = 0; i < 3; i++)
    {
      norm_i[i] = matrix[i][0];
      norm_j[i] = matrix[i][1];
      norm_k[i] = matrix[i][2];
      origin[i] = matrix[i][3];

      frame_header->size[i] = this->dimensions[i];
      frame_header->subvol_size[i] = this->subDimensions[i];
      frame_header->subvol_offset[i] = this->subOffset[i];
    }

    igtl_frame_set_matrix(this->spacing, origin,
      norm_i, norm_j, norm_k,
      frame_header);

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
    
    if (frame_header->header_version == IGTL_VIDEO_HEADER_VERSION)
      {
      // Video format version 1
      this->m_CodecType.clear();
      this->m_CodecType.append(frame_header->encoding, IGTL_VIDEO_CODEC_NAME_SIZE);
      this->endian           = frame_header->endian;
      this->videoFrameType   = frame_header->frameType;

      // Set image orientation
      float rspacing[3];
      float origin[3];
      float norm_i[3];
      float norm_j[3];
      float norm_k[3];

      igtl_frame_get_matrix(rspacing, origin,
        norm_i, norm_j, norm_k,
        frame_header);

      for (int i = 0; i < 3; i++)
      {
        this->spacing[i] = rspacing[i];
        matrix[i][0] = norm_i[i];
        matrix[i][1] = norm_j[i];
        matrix[i][2] = norm_k[i];
        matrix[i][3] = origin[i];

        this->dimensions[i] = frame_header->size[i];
        this->subDimensions[i] = frame_header->subvol_size[i];
        this->subOffset[i] = frame_header->subvol_offset[i];
      }

      matrix[3][0] = 0.0;
      matrix[3][1] = 0.0;
      matrix[3][2] = 0.0;
      matrix[3][3] = 1.0;

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

  void VideoMessage::SetSpacing(float s[3])
  {
    spacing[0] = s[0];
    spacing[1] = s[1];
    spacing[2] = s[2];
  }

  void VideoMessage::SetSpacing(float si, float sj, float sk)
  {
    spacing[0] = si;
    spacing[1] = sj;
    spacing[2] = sk;
  }

  void VideoMessage::GetSpacing(float s[3])
  {
    s[0] = spacing[0];
    s[1] = spacing[1];
    s[2] = spacing[2];
  }

  void VideoMessage::GetSpacing(float &si, float &sj, float &sk)
  {
    si = spacing[0];
    sj = spacing[1];
    sk = spacing[2];
  }

  void VideoMessage::SetOrigin(float p[3])
  {
    matrix[0][3] = p[0];
    matrix[1][3] = p[1];
    matrix[2][3] = p[2];
  }

  void VideoMessage::SetOrigin(float px, float py, float pz)
  {
    matrix[0][3] = px;
    matrix[1][3] = py;
    matrix[2][3] = pz;
  }

  void VideoMessage::GetOrigin(float p[3])
  {
    p[0] = matrix[0][3];
    p[1] = matrix[1][3];
    p[2] = matrix[2][3];
  }

  void VideoMessage::GetOrigin(float &px, float &py, float &pz)
  {
    px = matrix[0][3];
    py = matrix[1][3];
    pz = matrix[2][3];
  }

  void VideoMessage::SetNormals(float o[3][3])
  {
    matrix[0][0] = o[0][0];
    matrix[0][1] = o[0][1];
    matrix[0][2] = o[0][2];
    matrix[1][0] = o[1][0];
    matrix[1][1] = o[1][1];
    matrix[1][2] = o[1][2];
    matrix[2][0] = o[2][0];
    matrix[2][1] = o[2][1];
    matrix[2][2] = o[2][2];
  }

  void VideoMessage::SetNormals(float t[3], float s[3], float n[3])
  {
    matrix[0][0] = t[0];
    matrix[1][0] = t[1];
    matrix[2][0] = t[2];
    matrix[0][1] = s[0];
    matrix[1][1] = s[1];
    matrix[2][1] = s[2];
    matrix[0][2] = n[0];
    matrix[1][2] = n[1];
    matrix[2][2] = n[2];
  }

  void VideoMessage::GetNormals(float o[3][3])
  {
    o[0][0] = matrix[0][0];
    o[0][1] = matrix[0][1];
    o[0][2] = matrix[0][2];
    o[1][0] = matrix[1][0];
    o[1][1] = matrix[1][1];
    o[1][2] = matrix[1][2];
    o[2][0] = matrix[2][0];
    o[2][1] = matrix[2][1];
    o[2][2] = matrix[2][2];
  }

  void VideoMessage::GetNormals(float t[3], float s[3], float n[3])
  {
    t[0] = matrix[0][0];
    t[1] = matrix[1][0];
    t[2] = matrix[2][0];
    s[0] = matrix[0][1];
    s[1] = matrix[1][1];
    s[2] = matrix[2][1];
    n[0] = matrix[0][2];
    n[1] = matrix[1][2];
    n[2] = matrix[2][2];
  }

  void VideoMessage::SetMatrix(Matrix4x4& mat)
  {
    matrix[0][0] = mat[0][0];
    matrix[1][0] = mat[1][0];
    matrix[2][0] = mat[2][0];
    matrix[3][0] = mat[3][0];

    matrix[0][1] = mat[0][1];
    matrix[1][1] = mat[1][1];
    matrix[2][1] = mat[2][1];
    matrix[3][1] = mat[3][1];

    matrix[0][2] = mat[0][2];
    matrix[1][2] = mat[1][2];
    matrix[2][2] = mat[2][2];
    matrix[3][2] = mat[3][2];

    matrix[0][3] = mat[0][3];
    matrix[1][3] = mat[1][3];
    matrix[2][3] = mat[2][3];
    matrix[3][3] = mat[3][3];
  }

  void VideoMessage::GetMatrix(Matrix4x4& mat)
  {
    mat[0][0] = matrix[0][0];
    mat[1][0] = matrix[1][0];
    mat[2][0] = matrix[2][0];
    mat[3][0] = matrix[3][0];

    mat[0][1] = matrix[0][1];
    mat[1][1] = matrix[1][1];
    mat[2][1] = matrix[2][1];
    mat[3][1] = matrix[3][1];

    mat[0][2] = matrix[0][2];
    mat[1][2] = matrix[1][2];
    mat[2][2] = matrix[2][2];
    mat[3][2] = matrix[3][2];

    mat[0][3] = matrix[0][3];
    mat[1][3] = matrix[1][3];
    mat[2][3] = matrix[2][3];
    mat[3][3] = matrix[3][3];
  }

  void VideoMessage::SetDimensions(int s[3])
  {
    for (int i = 0; i < 3; ++i)
      {
      this->dimensions[i] = s[i];
      this->subDimensions[i] = s[i];
      }
  }

  void VideoMessage::SetDimensions(int i, int j, int k)
  {
    dimensions[0] = i;
    subDimensions[0] = i;

    dimensions[1] = i;
    subDimensions[1] = i;

    dimensions[2] = i;
    subDimensions[2] = i;
  }

  void VideoMessage::GetDimensions(int s[3])
  {
    for (int i = 0; i < 3; ++i)
      {
      s[i] = dimensions[i];
      }
  }

  void VideoMessage::GetDimensions(int &i, int &j, int &k)
  {
    i = dimensions[0];
    j = dimensions[1];
    k = dimensions[2];
  }

  int VideoMessage::SetSubVolume(int dim[3], int off[3])
  {
    // make sure that sub-volume fits in the dimensions
    if (off[0] + dim[0] <= dimensions[0] &&
      off[1] + dim[1] <= dimensions[1] &&
      off[2] + dim[2] <= dimensions[2])
    {
      subDimensions[0] = dim[0];
      subDimensions[1] = dim[1];
      subDimensions[2] = dim[2];
      subOffset[0] = off[0];
      subOffset[1] = off[1];
      subOffset[2] = off[2];
      return 1;
    }
    else
    {
      return 0;
    }
  }

  int VideoMessage::SetSubVolume(int dimi, int dimj, int dimk, int offi, int offj, int offk)
  {
    // make sure that sub-volume fits in the dimensions
    if (offi + dimi <= dimensions[0] &&
      offj + dimj <= dimensions[1] &&
      offk + dimk <= dimensions[2])
    {
      subDimensions[0] = dimi;
      subDimensions[1] = dimj;
      subDimensions[2] = dimk;
      subOffset[0] = offi;
      subOffset[1] = offj;
      subOffset[2] = offk;
      return 1;
    }
    else
    {
      return 0;
    }
  }

  void VideoMessage::GetSubVolume(int dim[3], int off[3])
  {
    dim[0] = subDimensions[0];
    dim[1] = subDimensions[1];
    dim[2] = subDimensions[2];
    off[0] = subOffset[0];
    off[1] = subOffset[1];
    off[2] = subOffset[2];
  }

  void VideoMessage::GetSubVolume(int &dimi, int &dimj, int &dimk,
    int &offi, int &offj, int &offk)
  {
    dimi = subDimensions[0];
    dimj = subDimensions[1];
    dimk = subDimensions[2];
    offi = subOffset[0];
    offj = subOffset[1];
    offk = subOffset[2];
  }

} // namespace igtl