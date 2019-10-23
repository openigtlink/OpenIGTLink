/*=========================================================================
 Program:   The OpenIGTLink Library
 Language:  C++
 Web page:  http://openigtlink.org/
 Copyright (c) Insight Software Consortium. All rights reserved.
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 =========================================================================*/

#include "igtlTrackingDataMessage.h"
#include "igtlMath.h"

#include "igtl_header.h"
#include "igtl_tdata.h"

// Disable warning C4996 (strncpy() may be unsafe) in Windows.
#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <vector>

namespace igtl {
  
  
//----------------------------------------------------------------------
// igtl::TrackingDataElement class

TrackingDataElement::TrackingDataElement() : Object()
{
  this->m_Name = "";
  this->m_Type = TYPE_TRACKER;
  IdentityMatrix(this->m_Matrix);
}
  
  
TrackingDataElement::~TrackingDataElement()
{
}
  
  
int TrackingDataElement::SetName(const char* name)
{
  if (strlen(name) <= IGTL_TDATA_LEN_NAME)
    {
    this->m_Name = name;
    return 1;
    }
  else
    {
    return 0;
    }
}
  
  
int TrackingDataElement::SetType(igtlUint8 type)
{
  if(type == TYPE_TRACKER ||
     type == TYPE_6D ||
     type == TYPE_3D ||
     type == TYPE_5D)
    {
    this->m_Type = type;
    return type;
    }
  else
    {
    return 0;
    }
}
  
  
void TrackingDataElement::SetPosition(float p[3])
{
  this->m_Matrix[0][3] = p[0];
  this->m_Matrix[1][3] = p[1];
  this->m_Matrix[2][3] = p[2];
}
  
  
void TrackingDataElement::GetPosition(float p[3])
{
  p[0] = this->m_Matrix[0][3];
  p[1] = this->m_Matrix[1][3];
  p[2] = this->m_Matrix[2][3];
}
  
  
void TrackingDataElement::SetPosition(float px, float py, float pz)
{
  this->m_Matrix[0][3] = px;
  this->m_Matrix[1][3] = py;
  this->m_Matrix[2][3] = pz;
}
  
  
void TrackingDataElement::GetPosition(float* px, float* py, float* pz)
{
  *px = this->m_Matrix[0][3];
  *py = this->m_Matrix[1][3];
  *pz = this->m_Matrix[2][3];
}
  
  
void TrackingDataElement::SetMatrix(Matrix4x4& mat)
{
  m_Matrix[0][0] = mat[0][0];
  m_Matrix[1][0] = mat[1][0];
  m_Matrix[2][0] = mat[2][0];
  m_Matrix[3][0] = mat[3][0];
    
  m_Matrix[0][1] = mat[0][1];
  m_Matrix[1][1] = mat[1][1];
  m_Matrix[2][1] = mat[2][1];
  m_Matrix[3][1] = mat[3][1];
    
  m_Matrix[0][2] = mat[0][2];
  m_Matrix[1][2] = mat[1][2];
  m_Matrix[2][2] = mat[2][2];
  m_Matrix[3][2] = mat[3][2];
    
  m_Matrix[0][3] = mat[0][3];
  m_Matrix[1][3] = mat[1][3];
  m_Matrix[2][3] = mat[2][3];
  m_Matrix[3][3] = mat[3][3];
}
  
  
void TrackingDataElement::GetMatrix(Matrix4x4& mat)
{
  mat[0][0] = m_Matrix[0][0];
  mat[1][0] = m_Matrix[1][0];
  mat[2][0] = m_Matrix[2][0];
  mat[3][0] = m_Matrix[3][0];
    
  mat[0][1] = m_Matrix[0][1];
  mat[1][1] = m_Matrix[1][1];
  mat[2][1] = m_Matrix[2][1];
  mat[3][1] = m_Matrix[3][1];
    
  mat[0][2] = m_Matrix[0][2];
  mat[1][2] = m_Matrix[1][2];
  mat[2][2] = m_Matrix[2][2];
  mat[3][2] = m_Matrix[3][2];
    
  mat[0][3] = m_Matrix[0][3];
  mat[1][3] = m_Matrix[1][3];
  mat[2][3] = m_Matrix[2][3];
  mat[3][3] = m_Matrix[3][3];
}
  
  
//----------------------------------------------------------------------
// igtl::StartTrackingDataMessage class
  
StartTrackingDataMessage::StartTrackingDataMessage()
{
  this->m_SendMessageType = "STT_TDATA";
  this->m_Resolution      = 0;
  this->m_CoordinateName  = "";
}
  
  
StartTrackingDataMessage::~StartTrackingDataMessage()
{
}
  
  
int StartTrackingDataMessage::SetCoordinateName(const char* name)
{
  if (strlen(name) <= IGTL_STT_TDATA_LEN_COORDNAME)
    {
    this->m_CoordinateName = name;
    return 1;
    }
  else
    {
    return 0;
    }
}
  
  
int StartTrackingDataMessage::CalculateContentBufferSize()
{
  return IGTL_STT_TDATA_SIZE;
}
  
  
int StartTrackingDataMessage::PackContent()
{
  AllocateBuffer();
  igtl_stt_tdata* stt_tdata = NULL;
  stt_tdata = (igtl_stt_tdata*)(this->m_Content);
    
  stt_tdata->resolution = this->m_Resolution;
  strncpy(stt_tdata->coord_name, this->m_CoordinateName.c_str(), IGTL_STT_TDATA_LEN_COORDNAME);
    
  igtl_stt_tdata_convert_byte_order(stt_tdata);
    
  return 1;
    
}
  
  
int StartTrackingDataMessage::UnpackContent()
{
  igtl_stt_tdata* stt_tdata = NULL;
  stt_tdata = (igtl_stt_tdata*)(this->m_Content);
    
  igtl_stt_tdata_convert_byte_order(stt_tdata);
    
  this->m_Resolution = stt_tdata->resolution;
    
  char strbuf[IGTL_STT_TDATA_LEN_COORDNAME+1];
  strbuf[IGTL_STT_TDATA_LEN_COORDNAME] = '\n';
  strncpy(strbuf, stt_tdata->coord_name, IGTL_STT_TDATA_LEN_COORDNAME);
    
  this->SetCoordinateName(strbuf);
    
  return 1;
    
}
  
  
//----------------------------------------------------------------------
// igtl::RTSTrackingDataMessage class
  
int  RTSTrackingDataMessage::CalculateContentBufferSize()
{
  return IGTL_RTS_TDATA_SIZE;
}
  
int  RTSTrackingDataMessage::PackContent()
{
  AllocateBuffer();
  igtl_rts_tdata* rts_tdata = NULL;
  rts_tdata = (igtl_rts_tdata*)this->m_Content;
    
  rts_tdata->status = this->m_Status;
    
  igtl_rts_tdata_convert_byte_order(rts_tdata);
    
  return 1;
}
  
  
int  RTSTrackingDataMessage::UnpackContent()
{
  igtl_rts_tdata* rts_tdata = NULL;
  rts_tdata = (igtl_rts_tdata*)this->m_Content;
    
  igtl_rts_tdata_convert_byte_order(rts_tdata);
    
  this->m_Status= rts_tdata->status;
    
  return 1;
}
  
//----------------------------------------------------------------------
// igtl::TrackingDataMessage class
  
TrackingDataMessage::TrackingDataMessage()
{
  this->m_SendMessageType = "TDATA";
  this->m_TrackingDataList.clear();
}
  
  
TrackingDataMessage::~TrackingDataMessage()
{
}
  
  
int TrackingDataMessage::AddTrackingDataElement(TrackingDataElement::Pointer& elem)
{
  m_IsBodyPacked = false;
  this->m_TrackingDataList.push_back(elem);
  return this->m_TrackingDataList.size();
}
  
  
void TrackingDataMessage::ClearTrackingDataElements()
{
  this->m_TrackingDataList.clear();
}
  
  
int TrackingDataMessage::GetNumberOfTrackingDataElements()
{
  return this->m_TrackingDataList.size();
}
  
  
void TrackingDataMessage::GetTrackingDataElement(int index, TrackingDataElement::Pointer& elem)
{
  if (index >= 0 && index < (int)this->m_TrackingDataList.size())
    {
    elem = this->m_TrackingDataList[index];
    }
}
  
  
int TrackingDataMessage::CalculateContentBufferSize()
{
  // The body size sum of the header size and status message size.
  return IGTL_TDATA_ELEMENT_SIZE * this->m_TrackingDataList.size();
}
  
  
int TrackingDataMessage::PackContent()
{
  // Allocate buffer
  AllocateBuffer();
  igtl_tdata_element* element = NULL;
  element = (igtl_tdata_element*)(this->m_Content);

  igtl_tdata_element * elementHolder = element;
  std::vector<TrackingDataElement::Pointer>::iterator iter;
    
  for (iter = this->m_TrackingDataList.begin(); iter != this->m_TrackingDataList.end(); iter ++)
    {
    strncpy((char*)element->name, (*iter)->GetName(), IGTL_TDATA_LEN_NAME);
    element->type = (*iter)->GetType();
    element->reserved = 0;
    Matrix4x4 matrix;
    (*iter)->GetMatrix(matrix);
    for (int i = 0; i < 3; i ++)
      {
      element->transform[i]   = matrix[i][0];
      element->transform[i+3] = matrix[i][1];
      element->transform[i+6] = matrix[i][2];
      element->transform[i+9] = matrix[i][3];
      }
    element ++;
    }
    
  igtl_tdata_convert_byte_order(elementHolder, this->m_TrackingDataList.size());
    
  return 1;
}
  
  
int TrackingDataMessage::UnpackContent()
{
    
  this->m_TrackingDataList.clear();
    
  igtl_tdata_element* element = NULL;
  int nElement = 0;

  element = (igtl_tdata_element*)(this->m_Content);
  nElement = igtl_tdata_get_data_n(CalculateReceiveContentSize());
    
  igtl_tdata_convert_byte_order(element, nElement);
    
  char strbuf[128];
  for (int i = 0; i < nElement; i ++)
    {
    TrackingDataElement::Pointer elemClass = TrackingDataElement::New();
      
    // Add '\n' at the end of each string
    // (neccesary for a case, where a string reaches the maximum length.)
    strbuf[IGTL_TDATA_LEN_NAME] = '\n';
    strncpy(strbuf, (char*)element->name, IGTL_TDATA_LEN_NAME);
    elemClass->SetName((const char*)strbuf);
    elemClass->SetType(element->type);
      
    Matrix4x4 matrix;
    IdentityMatrix(matrix);
    for (int j = 0; j < 3; j ++)
      {
      matrix[j][0] = element->transform[j];
      matrix[j][1] = element->transform[j+3];
      matrix[j][2] = element->transform[j+6];
      matrix[j][3] = element->transform[j+9];
      }
    elemClass->SetMatrix(matrix);
      
    this->m_TrackingDataList.push_back(elemClass);
      
    element ++;
    }
    
  return 1;
}
  
} // namespace igtl
