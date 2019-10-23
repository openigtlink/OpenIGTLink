/*=========================================================================
 
 Program:   The OpenIGTLink Library
 Language:  C++
 Web page:  http://openigtlink.org/
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlTransformMessage.h"

#include "igtl_header.h"
#include "igtl_transform.h"

#include <string.h>

namespace igtl {


TransformMessage::TransformMessage()
  : MessageBase()
{
  m_Transform = m_Content;
    
  matrix[0][0] = 1.0;
  matrix[1][0] = 0.0;
  matrix[2][0] = 0.0;
  matrix[3][0] = 0.0;
    
  matrix[0][1] = 0.0;
  matrix[1][1] = 1.0;
  matrix[2][1] = 0.0;
  matrix[3][1] = 0.0;
    
  matrix[0][2] = 0.0;
  matrix[1][2] = 0.0;
  matrix[2][2] = 1.0;
  matrix[3][2] = 0.0;
    
  matrix[0][3] = 0.0;
  matrix[1][3] = 0.0;
  matrix[2][3] = 0.0;
  matrix[3][3] = 1.0;
    
  m_SendMessageType  = "TRANSFORM";
    
}
  
TransformMessage::~TransformMessage()
{
}
  
void TransformMessage::SetPosition(float p[3])
{
  m_IsBodyPacked = false;
  matrix[0][3] = p[0];
  matrix[1][3] = p[1];
  matrix[2][3] = p[2];
}
  
void TransformMessage::GetPosition(float p[3])
{
  p[0] = matrix[0][3];
  p[1] = matrix[1][3];
  p[2] = matrix[2][3];
}
  
  
void TransformMessage::SetPosition(float px, float py, float pz)
{
  m_IsBodyPacked = false;
  matrix[0][3] = px;
  matrix[1][3] = py;
  matrix[2][3] = pz;
}
  
void TransformMessage::GetPosition(float* px, float* py, float* pz)
{
  *px = matrix[0][3];
  *py = matrix[1][3];
  *pz = matrix[2][3];
}
  
void TransformMessage::SetNormals(float o[3][3])
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
  
void TransformMessage::GetNormals(float o[3][3])
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
  
void TransformMessage::SetNormals(float t[3], float s[3], float n[3])
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
  
void TransformMessage::GetNormals(float t[3], float s[3], float n[3])
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
  
void TransformMessage::SetMatrix(Matrix4x4& mat)
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
  
void TransformMessage::GetMatrix(Matrix4x4& mat)
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
  
int TransformMessage::CalculateContentBufferSize()
{
  return IGTL_TRANSFORM_SIZE;
}
  
int TransformMessage::PackContent()
{
  AllocateBuffer();

  // The m_Transform is not aligned with m_Body, when allocatePack() is called.
  // This is to realign the two pointers.
#if OpenIGTLink_HEADER_VERSION >= 2
  this->m_Transform = this->m_Content;
#else
  this->m_Transform = m_Body;
#endif
  //igtl_float32* transform = (igtl_float32*)this->m_Transform; // doesn't work on Solaris
  igtl_float32 transform[12];
    
  for (int i = 0; i < 3; i ++) {
    transform[i]   = matrix[i][0];
    transform[i+3] = matrix[i][1];
    transform[i+6] = matrix[i][2];
    transform[i+9] = matrix[i][3];
  }
    
  igtl_transform_convert_byte_order(transform);
#if OpenIGTLink_HEADER_VERSION >= 2
  if (m_HeaderVersion == IGTL_HEADER_VERSION_2)
    {
      memcpy((void*)(this->m_Transform), (void*)transform, sizeof(igtl_float32)*12);
    }
  else
    {
      memcpy((void*)this->m_Transform, (void*)transform, sizeof(igtl_float32)*12);
    }
#elif OpenIGTLink_PROTOCOL_VERSION <= 2
  memcpy((void*)this->m_Transform, (void*)transform, sizeof(igtl_float32)*12);
#endif
    
  return 1;
}
  
int TransformMessage::UnpackContent()
{
  this->m_Transform = this->m_Content;
    
  //igtl_float32* transform = (igtl_float32*)this->m_Transform;  // doesn't work on Solaris
  igtl_float32 transform[12];
#if OpenIGTLink_HEADER_VERSION >= 2
  if (m_HeaderVersion == IGTL_HEADER_VERSION_2)
    {
      memcpy((void*)transform, (void*)(this->m_Transform), sizeof(igtl_float32)*12);
    }
  else
    {
      memcpy((void*)transform, (void*)this->m_Transform, sizeof(igtl_float32)*12);
    }
#elif OpenIGTLink_PROTOCOL_VERSION <= 2
  memcpy((void*)transform, (void*)this->m_Transform, sizeof(igtl_float32)*12);
#endif
    
  igtl_transform_convert_byte_order(transform);
    
  for (int i = 0; i < 3; i ++) {
    matrix[i][0] = transform[i]  ;
    matrix[i][1] = transform[i+3];
    matrix[i][2] = transform[i+6];
    matrix[i][3] = transform[i+9];
  }
    
  matrix[3][0] = 0.0;
  matrix[3][1] = 0.0;
  matrix[3][2] = 0.0;
  matrix[3][3] = 1.0;
    
  return 1;
}
  
} // namespace igtl
