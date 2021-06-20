/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlVideoMetaMessage.h"

#include "igtl_header.h"
#include "igtl_videometa.h"
#include "igtl_video.h"

// Disable warning C4996 (strncpy() may be unsafe) in Windows. 
#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <vector>

namespace igtl {


//----------------------------------------------------------------------
// igtl::VideoMetaElement class

VideoMetaElement::VideoMetaElement() : Object()
{
  for (int i = 0; i < 3; i ++)
    {
    spacing[i]    = 1.0;
    }
  for (int i = 0; i < 4; i ++)
    {
    for (int j = 0; j < 4; j ++)
      {
      matrix[i][j] = (i == j ? 1.0 : 0.0);
      }
    }
  this->m_Size[0]    = 0;
  this->m_Size[1]    = 0;
  this->m_Size[2]    = 0;
  this->m_ZoomLevel  = 1;
  this->m_FocalLength = 100.0;
  this->m_ScalarType = 0;
}


VideoMetaElement::~VideoMetaElement()
{
}


int VideoMetaElement::SetName(const char* name)
{
  if (strlen(name) <= IGTL_VIDEOMETA_LEN_NAME)
    {
    this->m_Name = name;
    return 1;
    }
  else
    {
    return 0;
    }
}


int VideoMetaElement::SetDeviceName(const char* devname)
{
  if (strlen(devname) <= IGTL_VIDEOMETA_LEN_DEVICE_NAME)
    {
    this->m_DeviceName = devname;
    return 1;
    }
  else
    {
    return 0;
    }
}

int VideoMetaElement::SetPatientName(const char* patname)
{
  if (strlen(patname) <= IGTL_VIDEOMETA_LEN_PATIENT_NAME)
    {
    this->m_PatientName = patname;
    return 1;
    }
  else
    {
    return 0;
    }
}


int VideoMetaElement::SetPatientID(const char* patid)
{
  if (strlen(patid) <= IGTL_VIDEOMETA_LEN_PATIENT_ID)
    {
    this->m_PatientID = patid;
    return 1;
    }
  else
    {
    return 0;
    }
}


void VideoMetaElement::SetSize(igtlUint16 si, igtlUint16 sj, igtlUint16 sk)
{
  this->m_Size[0] = si;
  this->m_Size[1] = sj;
  this->m_Size[2] = sk;
}

void VideoMetaElement::GetSize(igtlUint16& si, igtlUint16& sj, igtlUint16& sk)
{
  si = this->m_Size[0];
  sj = this->m_Size[1];
  sk = this->m_Size[2];
}

  
void VideoMetaElement::SetSpacing(float si, float sj, float sk)
{
  spacing[0] = si;
  spacing[1] = sj;
  spacing[2] = sk;
}

void VideoMetaElement::GetSpacing(float &si, float &sj, float &sk)
{
  si = spacing[0];
  sj = spacing[1];
  sk = spacing[2];
}

void VideoMetaElement::SetOrigin(float px, float py, float pz)
{
  matrix[0][3] = px;
  matrix[1][3] = py;
  matrix[2][3] = pz;
}

void VideoMetaElement::GetOrigin(float &px, float &py, float &pz)
{
  px = matrix[0][3];
  py = matrix[1][3];
  pz = matrix[2][3];
}

void VideoMetaElement::SetNormals(float t[3], float s[3], float n[3])
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

void VideoMetaElement::GetNormals(float t[3], float s[3], float n[3])
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

void VideoMetaElement::SetMatrix(Matrix4x4& mat)
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

void VideoMetaElement::GetMatrix(Matrix4x4& mat)
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

void VideoMetaElement::SetScalarType(igtlUint8 type)
{
  if (type == IGTL_VIDEO_STYPE_TYPE_INT8    ||
      type == IGTL_VIDEO_STYPE_TYPE_UINT8   ||
      type == IGTL_VIDEO_STYPE_TYPE_INT16   ||
      type == IGTL_VIDEO_STYPE_TYPE_UINT16  ||
      type == IGTL_VIDEO_STYPE_TYPE_INT32   ||
      type == IGTL_VIDEO_STYPE_TYPE_UINT32)
    {
    this->m_ScalarType = type;
    }
  else
    {
    this->m_ScalarType = 0;
    }
}


igtlUint8 VideoMetaElement::GetScalarType()
{
  return this->m_ScalarType;
}



//----------------------------------------------------------------------
// igtl::VideoMetaMessage class

VideoMetaMessage::VideoMetaMessage()
{
  this->m_SendMessageType = "VIDEOMETA";
  this->m_VideoMetaList.clear();
}


VideoMetaMessage::~VideoMetaMessage()
{
}


int VideoMetaMessage::AddVideoMetaElement(VideoMetaElement::Pointer& elem)
{
  this->m_VideoMetaList.push_back(elem);
  return this->m_VideoMetaList.size();
}


void VideoMetaMessage::ClearVideoMetaElement()
{
  this->m_VideoMetaList.clear();
}


int VideoMetaMessage::GetNumberOfVideoMetaElement()
{
  return this->m_VideoMetaList.size();
}


void VideoMetaMessage::GetVideoMetaElement(int index, VideoMetaElement::Pointer& elem)
{
  if (index >= 0 && index < (int) this->m_VideoMetaList.size())
    {
    elem = this->m_VideoMetaList[index];
    }
}


igtlUint64 VideoMetaMessage::CalculateContentBufferSize()
{
  // The body size sum of the header size and status message size.
  return IGTL_VIDEOMETA_ELEMENT_SIZE * this->m_VideoMetaList.size();
}


int VideoMetaMessage::PackContent()
{
  // Allocate buffer
  AllocateBuffer();
  
  igtl_videometa_element* element = (igtl_videometa_element*)this->m_Content;

  std::vector<VideoMetaElement::Pointer>::iterator iter;
  for (iter = this->m_VideoMetaList.begin(); iter != this->m_VideoMetaList.end(); iter ++)
    {
    strncpy((char*)element->name,         (*iter)->GetName(),        IGTL_VIDEOMETA_LEN_NAME);
    strncpy((char*)element->device_name,  (*iter)->GetDeviceName(),  IGTL_VIDEOMETA_LEN_DEVICE_NAME);
    strncpy((char*)element->patient_name, (*iter)->GetPatientName(), IGTL_VIDEOMETA_LEN_PATIENT_NAME);
    strncpy((char*)element->patient_id,   (*iter)->GetPatientID(),   IGTL_VIDEOMETA_LEN_PATIENT_ID);
    
    element->zoom_level = (*iter)->GetZoomLevel();
    element->focal_length = (*iter)->GetFocalLength();
    
    igtlUint16 size[3];
    (*iter)->GetSize(size[0], size[1], size[2]);
    element->size[0] = size[0];
    element->size[1] = size[1];
    element->size[2] = size[2];
    
    float origin[3];
    float norm_i[3];
    float norm_j[3];
    float norm_k[3];
    float spacing[3];
    Matrix4x4 mat;
    (*iter)->GetMatrix(mat);
    (*iter)->GetSpacing(spacing[0], spacing[1], spacing[2]);
    for (int i = 0; i < 3; i ++)
      {
      norm_i[i] = mat[i][0];
      norm_j[i] = mat[i][1];
      norm_k[i] = mat[i][2];
      origin[i] = mat[i][3];
      }
    
    igtl_videometa_set_matrix(spacing, origin,
                          norm_i, norm_j, norm_k,
                          element);
    
    element->scalar_type = (*iter)->GetScalarType();
    element->reserved = 0;
    element ++;
    }

  igtl_videometa_convert_byte_order((igtl_videometa_element*)this->m_Content, this->m_VideoMetaList.size());

  return 1;
}


int VideoMetaMessage::UnpackContent()
{

  this->m_VideoMetaList.clear();

  igtl_videometa_element* element = (igtl_videometa_element*) this->m_Content;
  int elementsDataLength = this->m_BodySizeToRead;
#if OpenIGTLink_HEADER_VERSION >= 2
  if (m_HeaderVersion == IGTL_HEADER_VERSION_2)
    {
    elementsDataLength = this->m_BodySizeToRead-IGTL_EXTENDED_HEADER_SIZE-this->GetMetaDataHeaderSize()-this->GetMetaDataSize();
    }
#endif
  int nElement = igtl_videometa_get_data_n(elementsDataLength);

  igtl_videometa_convert_byte_order(element, nElement);
  
  char strbuf[128];
  for (int i = 0; i < nElement; i ++)
    {
    VideoMetaElement::Pointer elemClass = VideoMetaElement::New();

    // Add '\n' at the end of each string
    // (necessary for a case, where a string reaches the maximum length.)
    strbuf[IGTL_VIDEOMETA_LEN_NAME] = '\n';
    strncpy(strbuf, (char*)element->name, IGTL_VIDEOMETA_LEN_NAME);
    elemClass->SetName((const char*)strbuf);
    
    strbuf[IGTL_VIDEOMETA_LEN_DEVICE_NAME] = '\n';
    strncpy(strbuf, (char*)element->device_name, IGTL_VIDEOMETA_LEN_DEVICE_NAME);
    elemClass->SetDeviceName(strbuf);

    strbuf[IGTL_VIDEOMETA_LEN_PATIENT_NAME] = '\n';
    strncpy(strbuf, (char*)element->patient_name, IGTL_VIDEOMETA_LEN_PATIENT_NAME);
    elemClass->SetPatientName(strbuf);

    strbuf[IGTL_VIDEOMETA_LEN_PATIENT_ID] = '\n';
    strncpy(strbuf, (char*)element->patient_id, IGTL_VIDEOMETA_LEN_PATIENT_ID);
    elemClass->SetPatientID(strbuf);
    
    elemClass->SetZoomLevel(element->zoom_level);
    elemClass->SetFocalLength(element->focal_length);
    elemClass->SetSize(element->size[0],element->size[1],element->size[2]);
    
    // Set image orientation
    float rspacing[3];
    float origin[3];
    float norm_i[3];
    float norm_j[3];
    float norm_k[3];
    
    igtl_videometa_get_matrix(rspacing, origin,
                          norm_i, norm_j, norm_k,
                          element);
    
    elemClass->SetSpacing(rspacing[0], rspacing[1], rspacing[2]);
    elemClass->SetNormals(norm_i, norm_j, norm_k);
    elemClass->SetOrigin(origin[0], origin[1], origin[2]);
    
    Matrix4x4 tmpMatrix;
    for (int i = 0; i < 3; i ++)
      {
      tmpMatrix[i][0] = norm_i[i];
      tmpMatrix[i][1] = norm_j[i];
      tmpMatrix[i][2] = norm_k[i];
      tmpMatrix[i][3] = origin[i];
      }
    tmpMatrix[3][0] = 0.0;
    tmpMatrix[3][1] = 0.0;
    tmpMatrix[3][2] = 0.0;
    tmpMatrix[3][3] = 1.0;
    elemClass->SetMatrix(tmpMatrix);
    
    elemClass->SetScalarType(element->scalar_type);

    this->m_VideoMetaList.push_back(elemClass);

    element ++;
    }

  return 1;
}

} // namespace igtl