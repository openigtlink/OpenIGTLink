/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlImageMessage.h"

#include "igtl_header.h"
#include "igtl_image.h"

namespace igtl {


ImageMessage::ImageMessage():
  MessageBase()
{
  for (int i = 0; i < 3; i ++)
    {
    dimensions[i] = 0;
    spacing[i]    = 0.0;
    subDimensions[i] = 0;
    subOffset[i] = 0;
    }
  for (int i = 0; i < 4; i ++)
    {
    for (int j = 0; j < 4; j ++)
      {
      matrix[i][j] = (i == j ? 1.0 : 0.0);
      }
    }

  endian        = ENDIAN_BIG;
  coordinate    = COORDINATE_RAS;
  scalarType    = TYPE_UINT8;
  m_ImageHeader = NULL;
  m_Image       = NULL;
  numComponents = 1;

  m_SendMessageType  = "IMAGE";

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
  ScalarSizeTable[10]= sizeof(igtlFloat32); // TYPE_FLOAT32
  ScalarSizeTable[11]= sizeof(igtlFloat64); // TYPE_FLOAT64
}

ImageMessage::~ImageMessage()
{
}

void ImageMessage::SetDimensions(int s[3])
{
  m_IsBodyPacked = false;
  dimensions[0] = s[0];
  dimensions[1] = s[1];
  dimensions[2] = s[2];

  // initialize sub-volume
  subDimensions[0] = dimensions[0];
  subDimensions[1] = dimensions[1];
  subDimensions[2] = dimensions[2];
  subOffset[0] = 0;
  subOffset[1] = 0;
  subOffset[2] = 0;
}

void ImageMessage::SetDimensions(int i, int j, int k)
{
  m_IsBodyPacked = false;
  dimensions[0] = i;
  dimensions[1] = j;
  dimensions[2] = k;

  // initialize sub-volume
  subDimensions[0] = dimensions[0];
  subDimensions[1] = dimensions[1];
  subDimensions[2] = dimensions[2];
  subOffset[0] = 0;
  subOffset[1] = 0;
  subOffset[2] = 0;
}

void ImageMessage::GetDimensions(int s[3])
{
  s[0] = dimensions[0];
  s[1] = dimensions[1];
  s[2] = dimensions[2];
}

void ImageMessage::GetDimensions(int &i, int &j, int &k)
{
  i = dimensions[0];
  j = dimensions[1];
  k = dimensions[2];
}

int ImageMessage::SetSubVolume(int dim[3], int off[3])
{
  // make sure that sub-volume fits in the dimensions
  if (off[0] + dim[0] <= dimensions[0] &&
      off[1] + dim[1] <= dimensions[1] &&
      off[2] + dim[2] <= dimensions[2])
    {
    m_IsBodyPacked = false;
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

int ImageMessage::SetSubVolume(int dimi, int dimj, int dimk, int offi, int offj, int offk)
{
  // make sure that sub-volume fits in the dimensions
  if (offi + dimi <= dimensions[0] &&
      offj + dimj <= dimensions[1] &&
      offk + dimk <= dimensions[2])
    {
    m_IsBodyPacked = false;
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

void ImageMessage::GetSubVolume(int dim[3], int off[3])
{
  dim[0] = subDimensions[0];
  dim[1] = subDimensions[1];
  dim[2] = subDimensions[2];
  off[0] = subOffset[0];
  off[1] = subOffset[1];
  off[2] = subOffset[2];
}

void ImageMessage::GetSubVolume(int &dimi, int &dimj, int &dimk,
                                int &offi, int &offj, int &offk)
{
  dimi = subDimensions[0];
  dimj = subDimensions[1];
  dimk = subDimensions[2];
  offi = subOffset[0];
  offj = subOffset[1];
  offk = subOffset[2];
}


void ImageMessage::SetSpacing(float s[3])
{
  m_IsBodyPacked = false;
  spacing[0] = s[0];
  spacing[1] = s[1];
  spacing[2] = s[2];
}

void ImageMessage::SetSpacing(float si, float sj, float sk)
{
  m_IsBodyPacked = false;
  spacing[0] = si;
  spacing[1] = sj;
  spacing[2] = sk;
}

void ImageMessage::GetSpacing(float s[3])
{
  s[0] = spacing[0];
  s[1] = spacing[1];
  s[2] = spacing[2];
}

void ImageMessage::GetSpacing(float &si, float &sj, float &sk)
{
  si = spacing[0];
  sj = spacing[1];
  sk = spacing[2];
}
  
void ImageMessage::SetOrigin(float p[3])
{
  m_IsBodyPacked = false;
  matrix[0][3] = p[0];
  matrix[1][3] = p[1];
  matrix[2][3] = p[2];
}

void ImageMessage::SetOrigin(float px, float py, float pz)
{
  m_IsBodyPacked = false;
  matrix[0][3] = px;
  matrix[1][3] = py;
  matrix[2][3] = pz;
}

void ImageMessage::GetOrigin(float p[3])
{
  p[0] = matrix[0][3];
  p[1] = matrix[1][3];
  p[2] = matrix[2][3];
}

void ImageMessage::GetOrigin(float &px, float &py, float &pz)
{
  px = matrix[0][3];
  py = matrix[1][3];
  pz = matrix[2][3];
}

void ImageMessage::SetNormals(float o[3][3])
{
  m_IsBodyPacked = false;
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

void ImageMessage::SetNormals(float t[3], float s[3], float n[3])
{
  m_IsBodyPacked = false;
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

void ImageMessage::GetNormals(float o[3][3])
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

void ImageMessage::GetNormals(float t[3], float s[3], float n[3])
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

void ImageMessage::SetMatrix(Matrix4x4& mat)
{
  m_IsBodyPacked = false;
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

void ImageMessage::GetMatrix(Matrix4x4& mat)
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

void ImageMessage::AllocateScalars()
{
  // Memory area to store image scalar is allocated with
  // message and image header, by using AllocatePack() implemented
  // in the parent class.
  AllocateBuffer();
#if OpenIGTLink_HEADER_VERSION >= 2
  m_ImageHeader = m_Content;
  m_Image  = &m_ImageHeader[IGTL_IMAGE_HEADER_SIZE];
#else
  m_ImageHeader = m_Body;
  m_Image  = &m_ImageHeader[IGTL_IMAGE_HEADER_SIZE];
#endif
}

void* ImageMessage::GetScalarPointer()
{
  return (void*)m_Image;
}

int ImageMessage::CalculateContentBufferSize()
{
  return GetSubVolumeImageSize() + IGTL_IMAGE_HEADER_SIZE;
}

int ImageMessage::PackContent()
{
  AllocateScalars();
  
  igtl_image_header* image_header = (igtl_image_header*)m_ImageHeader;

  image_header->header_version           = IGTL_IMAGE_HEADER_VERSION;
  image_header->num_components    = this->numComponents;
  image_header->scalar_type       = this->scalarType;
  image_header->endian            = this->endian;
  image_header->coord             = this->coordinate;
  image_header->size[0]           = this->dimensions[0];
  image_header->size[1]           = this->dimensions[1];
  image_header->size[2]           = this->dimensions[2];
  image_header->subvol_offset[0]  = this->subOffset[0];
  image_header->subvol_offset[1]  = this->subOffset[1];
  image_header->subvol_offset[2]  = this->subOffset[2];
  image_header->subvol_size[0]    = this->subDimensions[0];
  image_header->subvol_size[1]    = this->subDimensions[1];
  image_header->subvol_size[2]    = this->subDimensions[2];

  float origin[3];
  float norm_i[3];
  float norm_j[3];
  float norm_k[3];

  for (int i = 0; i < 3; i ++)
    {
    norm_i[i] = matrix[i][0];
    norm_j[i] = matrix[i][1];
    norm_k[i] = matrix[i][2];
    origin[i] = matrix[i][3];
    }

  igtl_image_set_matrix(this->spacing, origin,
                        norm_i, norm_j, norm_k,
                        image_header);

  igtl_image_convert_byte_order(image_header);
  
  return 1;

}


int ImageMessage::UnpackContent()
{
#if OpenIGTLink_HEADER_VERSION >= 2
  m_ImageHeader = m_Content;
#elif OpenIGTLink_PROTOCOL_VERSION <=2
  m_ImageHeader = m_Body;
#endif
  igtl_image_header* image_header = (igtl_image_header*)m_ImageHeader;
  igtl_image_convert_byte_order(image_header);

  if (image_header->header_version == IGTL_IMAGE_HEADER_VERSION)
    {
    // Image format version 1
    this->scalarType       = image_header->scalar_type;
    this->numComponents    = image_header->num_components;
    this->endian           = image_header->endian;
    this->coordinate       = image_header->coord;
    this->dimensions[0]    = image_header->size[0];
    this->dimensions[1]    = image_header->size[1];
    this->dimensions[2]    = image_header->size[2];
    this->subOffset[0]     = image_header->subvol_offset[0];
    this->subOffset[1]     = image_header->subvol_offset[1];
    this->subOffset[2]     = image_header->subvol_offset[2];
    this->subDimensions[0] = image_header->subvol_size[0];
    this->subDimensions[1] = image_header->subvol_size[1];
    this->subDimensions[2] = image_header->subvol_size[2];

    // Set image orientation
    float rspacing[3];
    float origin[3];
    float norm_i[3];
    float norm_j[3];
    float norm_k[3];

    igtl_image_get_matrix(rspacing, origin,
                          norm_i, norm_j, norm_k,
                          image_header);

    for (int i = 0; i < 3; i ++)
      {
      this->spacing[i] = rspacing[i];
      matrix[i][0] = norm_i[i];
      matrix[i][1] = norm_j[i];
      matrix[i][2] = norm_k[i];
      matrix[i][3] = origin[i];
      }

    matrix[3][0] = 0.0;
    matrix[3][1] = 0.0;
    matrix[3][2] = 0.0;
    matrix[3][3] = 1.0;
      
    m_Image  = &m_ImageHeader[IGTL_IMAGE_HEADER_SIZE];

    return 1;
    }
  else
    {
    // Incompatible version. 
    return 0;
    }
}


void ImageMessage::SetNumComponents(int num)
{
  if (num > 0 && num < 255)
    {
    numComponents = num;
    }
}

int ImageMessage::GetNumComponents()
{
  return numComponents;
}


} // namespace igtl
