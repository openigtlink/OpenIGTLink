/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlImageMessage2.h"

#include <cstring>

#include "igtl_header.h"
#include "igtl_image.h"

namespace igtl {


ImageMessage2::ImageMessage2():
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

#ifdef FRAGMENTED_PACK
  this->m_Header  = new unsigned char [IGTL_HEADER_SIZE];
  this->m_Content = NULL;

  this->m_SelfAllocatedImageHeader = 0;
  this->m_ImageSize = 0;
  this->m_SelfAllocatedImage = 0;

  this->m_SinglePack = NULL;
  this->m_SinglePackSize = 0; 

  // Size of the pack. Only header can be used after initialization
  this->m_MessageSize = IGTL_HEADER_SIZE;
#endif

}


ImageMessage2::~ImageMessage2()
{
#ifdef FRAGMENTED_PACK
  //if (this->m_Header)
  //  {
  //  delete [] this->m_Header;
  //  }
  if (this->m_ImageHeader && this->m_ImageHeader!= this->m_Content && this->m_SelfAllocatedImageHeader)
    {
    delete [] this->m_ImageHeader;
    }
  if (this->m_Image && this->m_SelfAllocatedImage)
    {
    delete [] this->m_Image;
    this->m_SelfAllocatedImage = 0;
    }
#endif
}

void ImageMessage2::SetDimensions(int s[3])
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

void ImageMessage2::SetDimensions(int i, int j, int k)
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

void ImageMessage2::GetDimensions(int s[3])
{
  s[0] = dimensions[0];
  s[1] = dimensions[1];
  s[2] = dimensions[2];
}

void ImageMessage2::GetDimensions(int &i, int &j, int &k)
{
  i = dimensions[0];
  j = dimensions[1];
  k = dimensions[2];
}

int ImageMessage2::SetSubVolume(int dim[3], int off[3])
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

int ImageMessage2::SetSubVolume(int dimi, int dimj, int dimk, int offi, int offj, int offk)
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

void ImageMessage2::GetSubVolume(int dim[3], int off[3])
{
  dim[0] = subDimensions[0];
  dim[1] = subDimensions[1];
  dim[2] = subDimensions[2];
  off[0] = subOffset[0];
  off[1] = subOffset[1];
  off[2] = subOffset[2];
}

void ImageMessage2::GetSubVolume(int &dimi, int &dimj, int &dimk,
                                 int &offi, int &offj, int &offk)
{
  dimi = subDimensions[0];
  dimj = subDimensions[1];
  dimk = subDimensions[2];
  offi = subOffset[0];
  offj = subOffset[1];
  offk = subOffset[2];
}


void ImageMessage2::SetSpacing(float s[3])
{
  m_IsBodyPacked = false;
  spacing[0] = s[0];
  spacing[1] = s[1];
  spacing[2] = s[2];
}

void ImageMessage2::SetSpacing(float si, float sj, float sk)
{
  m_IsBodyPacked = false;
  spacing[0] = si;
  spacing[1] = sj;
  spacing[2] = sk;
}

void ImageMessage2::GetSpacing(float s[3])
{
  s[0] = spacing[0];
  s[1] = spacing[1];
  s[2] = spacing[2];
}

void ImageMessage2::GetSpacing(float &si, float &sj, float &sk)
{
  si = spacing[0];
  sj = spacing[1];
  sk = spacing[2];
}
  
void ImageMessage2::SetOrigin(float p[3])
{
  m_IsBodyPacked = false;
  matrix[0][3] = p[0];
  matrix[1][3] = p[1];
  matrix[2][3] = p[2];
}

void ImageMessage2::SetOrigin(float px, float py, float pz)
{
  m_IsBodyPacked = false;
  matrix[0][3] = px;
  matrix[1][3] = py;
  matrix[2][3] = pz;
}

void ImageMessage2::GetOrigin(float p[3])
{
  p[0] = matrix[0][3];
  p[1] = matrix[1][3];
  p[2] = matrix[2][3];
}

void ImageMessage2::GetOrigin(float &px, float &py, float &pz)
{
  px = matrix[0][3];
  py = matrix[1][3];
  pz = matrix[2][3];
}

void ImageMessage2::SetNormals(float o[3][3])
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

void ImageMessage2::SetNormals(float t[3], float s[3], float n[3])
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

void ImageMessage2::GetNormals(float o[3][3])
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

void ImageMessage2::GetNormals(float t[3], float s[3], float n[3])
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

void ImageMessage2::SetMatrix(Matrix4x4& mat)
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

void ImageMessage2::GetMatrix(Matrix4x4& mat)
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

void ImageMessage2::AllocateScalars()
{
  // Memory area to store image scalar is allocated with
  // message and image header, by using AllocatePack() implemented
  // in the parent class.
  
#ifdef FRAGMENTED_PACK
  MessageBase::AllocateBuffer();
  if (!this->m_ImageHeader)
    {
    this->m_ImageHeader = new unsigned char [IGTL_IMAGE_HEADER_SIZE];
    this->m_SelfAllocatedImageHeader = 1;
    }

  int s = GetSubVolumeImageSize();
  if (this->m_SelfAllocatedImage && this->m_Image)
    {
    delete [] this->m_Image;
    }
  this->m_Image = new unsigned char [s];
  this->m_SelfAllocatedImage = 1;
  this->m_ImageSize = s;
  this->m_MessageSize = IGTL_HEADER_SIZE + IGTL_IMAGE_HEADER_SIZE + s;

#else
  AllocateBuffer();
  this->m_ImageHeader = m_Content;
  this->m_Image  = &m_ImageHeader[IGTL_IMAGE_HEADER_SIZE];
#endif
}


void* ImageMessage2::GetScalarPointer()
{
  return (void*)m_Image;
}

#ifdef FRAGMENTED_PACK  
void  ImageMessage2::SetScalarPointer(void * p)
{
  if (this->m_SelfAllocatedImage && this->m_Image)
    {
    this->m_SelfAllocatedImage = 0;
    this->m_ImageSize = 0;

    delete [] this->m_Image;
    }
  this->m_Image = (unsigned char *) p;
  for (int i = 0;i<IGTL_IMAGE_HEADER_SIZE; i++)
    {
    this->m_Content[i] = m_ImageHeader[i];
    }
  for (int i = 0;i<GetImageSize(); i++)
    {
    this->m_Content[i+IGTL_IMAGE_HEADER_SIZE] = m_Image[i];
    }
}


void* ImageMessage2::GetBufferPointer()
{
  // This function is re-implemented for backward compatibility.
  // If fragmented pack is supported, this function may cause
  // memory copy that may considerably slows the program.

  // When the pack only contains header
  if (this->m_MessageSize == IGTL_HEADER_SIZE)
    {
    return this->m_Header;
    }

  // 
  int vs = this->GetSubVolumeImageSize();
  if (this->m_SinglePackSize < IGTL_HEADER_SIZE + IGTL_IMAGE_HEADER_SIZE + vs)
    {
    if (this->m_SinglePack)
      {
      delete [] this->m_SinglePack;
      }
    this->m_SinglePack = new unsigned char [IGTL_HEADER_SIZE + IGTL_IMAGE_HEADER_SIZE + vs];
    this->m_SinglePackSize = IGTL_HEADER_SIZE + IGTL_IMAGE_HEADER_SIZE + vs;
    }
    
  if (this->m_SinglePack == NULL)
    {
    return NULL;
    }

  unsigned char * ptr = this->m_SinglePack;
  memcpy(ptr, this->m_Header, IGTL_HEADER_SIZE);
  ptr += IGTL_HEADER_SIZE;
  memcpy(ptr, this->m_ImageHeader, IGTL_IMAGE_HEADER_SIZE);
  ptr += IGTL_IMAGE_HEADER_SIZE;
  memcpy(ptr, this->m_Image, vs);

  return (void*) this->m_SinglePack;  
}


void* ImageMessage2::GetPackFragmentPointer(int id)
{
  if (id == 0)
    {
    return this->m_Header;
    }
  else if (id == 1)
    {
    return this->m_ImageHeader;
    }
  else if (id == 2)
    {
    return this->m_Image;
    }

  return NULL;

}


int ImageMessage2::GetPackFragmentSize(int id)
{
  if (id == 0)
    {
    return IGTL_HEADER_SIZE;
    }
  else if (id == 1)
    {
    return IGTL_IMAGE_HEADER_SIZE;
    }
  else if (id == 2)
    {
    return GetSubVolumeImageSize();
    }

  return 0;
}

#endif // FRAGMENTED_PACK  


int ImageMessage2::CalculateContentBufferSize()
{
  // This function is called by:
  //   MessageBase::Pack()
  //   MessageBase::AllocatePack()
  return GetSubVolumeImageSize() + IGTL_IMAGE_HEADER_SIZE;
}


#ifdef FRAGMENTED_PACK  
int ImageMessage2::Pack()
{
  PackContent();
  m_IsBodyUnpacked   = false;
  
  // pack header
  igtl_header* h = (igtl_header*) m_Header;

  igtl_uint64 crc = crc64(0, 0, 0LL); // initial crc

  h->header_version   = IGTL_HEADER_VERSION_1;

  igtl_uint64 ts  =  m_TimeStampSec & 0xFFFFFFFF;
  ts = (ts << 32) | (m_TimeStampSecFraction & 0xFFFFFFFF);

  h->timestamp = ts;
  h->body_size = CalculateContentBufferSize();

  // Note pack fragment #0 is the OpenIGTLink general header.
  for (int i = 1; i < this->GetNumberOfPackFragments(); i ++)
    {
    crc = crc64((unsigned char*)this->GetPackFragmentPointer(i),
                this->GetPackFragmentSize(i), crc);
    }
  h->crc = crc;

  strncpy(h->name, m_SendMessageType.c_str(), 12);

  strncpy(h->device_name, m_DeviceName.c_str(), 20);

  igtl_header_convert_byte_order(h);

  m_IsHeaderUnpacked = false;

  return 1;
}
#endif // FRAGMENTED_PACK  


int ImageMessage2::PackContent()
{
  igtl_image_header* image_header = (igtl_image_header*)this->m_ImageHeader;

  image_header->header_version    = IGTL_IMAGE_HEADER_VERSION;
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

  for (int i = 0; i < 3; i ++) {
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


int ImageMessage2::UnpackContent()
{
  if (this->m_ImageHeader && this->m_SelfAllocatedImageHeader)
    {
    delete [] this->m_ImageHeader;
    this->m_SelfAllocatedImageHeader = 0;
    }
  if (this->m_Image && this->m_SelfAllocatedImage)
    {
    delete [] this->m_Image;
    this->m_SelfAllocatedImage = 0;
    }

  this->m_ImageHeader = this->m_Content;

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

    this->m_Image = this->m_ImageHeader;
    this->m_Image += IGTL_IMAGE_HEADER_SIZE;
      
    return 1;
    }
  else
    {
    // Incompatible version. 
    return 0;
    }
}

#ifdef FRAGMENTED_PACK  
void ImageMessage2::AllocateBuffer(int contentSize)
{
  MessageBase::AllocateBuffer(contentSize);
  if (contentSize <= 0)
    {
    contentSize = 0;
    this->m_IsBodyUnpacked = false;
    }

  int s = IGTL_HEADER_SIZE + contentSize;

  m_IsHeaderUnpacked = false;
  m_IsBodyUnpacked = false;

  if (contentSize > 0)
    {
    if (this->m_ImageHeader && this->m_SelfAllocatedImageHeader)
      {
      delete [] this->m_ImageHeader;
      this->m_ImageHeader = NULL;
      this->m_SelfAllocatedImageHeader = 0;
      }
    if (this->m_Image && this->m_SelfAllocatedImage)
      {
      delete [] this->m_Image;
      this->m_Image = NULL;
      this->m_SelfAllocatedImage = 0;
      }
    if (this->m_Body)
      {
      if (this->m_MessageSize != s)
        {
        // If the pack area exists but needs to be reallocated
        // m_IsHeaderUnpacked status is not changed in this case.
        unsigned char * old = this->m_Body;
        this->m_Body = new unsigned char [contentSize];
        memcpy(this->m_Body, old, contentSize);
        delete [] old;
        }
      }
    else 
      {
      this->m_Body = new unsigned char [contentSize];
      }
    this->m_MessageSize = s;
    }
}
#endif // FRAGMENTED_PACK  


void ImageMessage2::SetNumComponents(int num)
{
  if (num > 0 && num < 255)
    {
    numComponents = num;
    }
}


int ImageMessage2::GetNumComponents()
{
  return numComponents;
}


} // namespace igtl
