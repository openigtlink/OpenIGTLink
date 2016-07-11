/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlQuaternionTrackingDataMessage.h"
#include "igtlMath.h"

#include "igtl_header.h"
#include "igtl_qtdata.h"

// Disable warning C4996 (strncpy() may be unsafe) in Windows. 
#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <vector>

namespace igtl {


//----------------------------------------------------------------------
// igtl::QuaternionTrackingDataElement class

QuaternionTrackingDataElement::QuaternionTrackingDataElement() : Object()
{
  this->m_Name = "";
  this->m_Type = TYPE_TRACKER;

  this->m_position[0] = 0;
  this->m_position[1] = 0;
  this->m_position[2] = 0;

  this->m_quaternion[0] = 0;
  this->m_quaternion[1] = 0;
  this->m_quaternion[2] = 0;
  this->m_quaternion[3] = 1;
}


QuaternionTrackingDataElement::~QuaternionTrackingDataElement()
{
}


int QuaternionTrackingDataElement::SetName(const char* name)
{
  if (strlen(name) <= IGTL_QTDATA_LEN_NAME)
    {
    this->m_Name = name;
    return 1;
    }
  else
    {
    return 0;
    }
}


int QuaternionTrackingDataElement::SetType(igtlUint8 type)
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


void QuaternionTrackingDataElement::SetPosition(float p[3])
{
  this->m_position[0] = p[0];
  this->m_position[1] = p[1];
  this->m_position[2] = p[2];
}


void QuaternionTrackingDataElement::GetPosition(float p[3])
{
  p[0] = this->m_position[0];
  p[1] = this->m_position[1];
  p[2] = this->m_position[2];
}


void QuaternionTrackingDataElement::SetPosition(float px, float py, float pz)
{
  this->m_position[0] = px;
  this->m_position[1] = py;
  this->m_position[2] = pz;
}


void QuaternionTrackingDataElement::GetPosition(float* px, float* py, float* pz)
{
  *px = this->m_position[0];
  *py = this->m_position[1];
  *pz = this->m_position[2];
}


void QuaternionTrackingDataElement::SetQuaternion(float q[4])
{
  this->m_quaternion[0] = q[0];
  this->m_quaternion[1] = q[1];
  this->m_quaternion[2] = q[2];
  this->m_quaternion[3] = q[3];
}


void QuaternionTrackingDataElement::GetQuaternion(float q[4])
{
  q[0] = this->m_quaternion[0];
  q[1] = this->m_quaternion[1];
  q[2] = this->m_quaternion[2];
  q[3] = this->m_quaternion[3];
}


void QuaternionTrackingDataElement::SetQuaternion(float qx, float qy, float qz, float w)
{
  this->m_quaternion[0] = qx;
  this->m_quaternion[1] = qy;
  this->m_quaternion[2] = qz;
  this->m_quaternion[3] = w;
}


void QuaternionTrackingDataElement::GetQuaternion(float* qx, float* qy, float* qz, float* w)
{
  *qx = this->m_quaternion[0];
  *qy = this->m_quaternion[1];
  *qz = this->m_quaternion[2];
  *w  = this->m_quaternion[3];
}


//----------------------------------------------------------------------
// igtl::StartQuaternionTrackingDataMessage class

StartQuaternionTrackingDataMessage::StartQuaternionTrackingDataMessage()
{
  this->m_SendMessageType = "STT_QTDATA";
  this->m_Resolution      = 0;
  this->m_CoordinateName  = "";
}


StartQuaternionTrackingDataMessage::~StartQuaternionTrackingDataMessage()
{
}


int StartQuaternionTrackingDataMessage::SetCoordinateName(const char* name)
{
  if (strlen(name) <= IGTL_STT_QTDATA_LEN_COORDNAME)
    {
    this->m_CoordinateName = name;
    return 1;
    }
  else
    {
    return 0;
    }
}


int StartQuaternionTrackingDataMessage::CalculateContentBufferSize()
{
  return IGTL_STT_QTDATA_SIZE;
}


int StartQuaternionTrackingDataMessage::PackContent()
{
  AllocateBuffer();

  igtl_stt_qtdata* stt_qtdata = (igtl_stt_qtdata*)this->m_Content;

  stt_qtdata->resolution = this->m_Resolution;
  strncpy(stt_qtdata->coord_name, this->m_CoordinateName.c_str(), IGTL_STT_QTDATA_LEN_COORDNAME);

  igtl_stt_qtdata_convert_byte_order(stt_qtdata);

  return 1;
  
}


int StartQuaternionTrackingDataMessage::UnpackContent()
{
  igtl_stt_qtdata* stt_qtdata = (igtl_stt_qtdata*)this->m_Content;
  
  igtl_stt_qtdata_convert_byte_order(stt_qtdata);

  this->m_Resolution = stt_qtdata->resolution;

  char strbuf[IGTL_STT_QTDATA_LEN_COORDNAME+1];
  strbuf[IGTL_STT_QTDATA_LEN_COORDNAME] = '\n';
  strncpy(strbuf, stt_qtdata->coord_name, IGTL_STT_QTDATA_LEN_COORDNAME);

  this->SetCoordinateName(strbuf);

  return 1;

}


RTSQuaternionTrackingDataMessage::RTSQuaternionTrackingDataMessage() 
  : m_Status(0)
{
  this->m_SendMessageType  = "RTS_QTDATA";
}

//----------------------------------------------------------------------
// igtl::RTSQuaternionTrackingDataMessage class

int  RTSQuaternionTrackingDataMessage::CalculateContentBufferSize()
{ 
  return IGTL_RTS_QTDATA_SIZE; 
}

int  RTSQuaternionTrackingDataMessage::PackContent()
{
  AllocateBuffer(); 

  igtl_rts_qtdata* rts_qtdata = (igtl_rts_qtdata*)this->m_Content;

  rts_qtdata->status = this->m_Status;

  igtl_rts_qtdata_convert_byte_order(rts_qtdata);

  return 1; 
}


int  RTSQuaternionTrackingDataMessage::UnpackContent()
{ 
  igtl_rts_qtdata* rts_qtdata = (igtl_rts_qtdata*)this->m_Content;
  
  igtl_rts_qtdata_convert_byte_order(rts_qtdata);

  this->m_Status= rts_qtdata->status;

  return 1; 
}



//----------------------------------------------------------------------
// igtl::QuaternionTrackingDataMessage class

QuaternionTrackingDataMessage::QuaternionTrackingDataMessage()
{
  this->m_SendMessageType = "QTDATA";
  this->m_QuaternionTrackingDataList.clear();
}


QuaternionTrackingDataMessage::~QuaternionTrackingDataMessage()
{
}


int QuaternionTrackingDataMessage::AddQuaternionTrackingDataElement(QuaternionTrackingDataElement::Pointer& elem)
{
  this->m_QuaternionTrackingDataList.push_back(elem);
  return this->m_QuaternionTrackingDataList.size();
}


void QuaternionTrackingDataMessage::ClearQuaternionTrackingDataElements()
{
  this->m_QuaternionTrackingDataList.clear();
}


int QuaternionTrackingDataMessage::GetNumberOfQuaternionTrackingDataElements()
{
  return this->m_QuaternionTrackingDataList.size();
}


void QuaternionTrackingDataMessage::GetQuaternionTrackingDataElement(int index, QuaternionTrackingDataElement::Pointer& elem)
{
  if (index >= 0 && index < (int)this->m_QuaternionTrackingDataList.size())
    {
    elem = this->m_QuaternionTrackingDataList[index];
    }
}


int QuaternionTrackingDataMessage::CalculateContentBufferSize()
{
  return IGTL_QTDATA_ELEMENT_SIZE * this->m_QuaternionTrackingDataList.size();
}


int QuaternionTrackingDataMessage::PackContent()
{
  // Allocate buffer
  AllocateBuffer();
  
  igtl_qtdata_element* element = NULL;
  element = (igtl_qtdata_element*)(this->m_Content);
  
  igtl_qtdata_element * elementHolder = element;
  std::vector<QuaternionTrackingDataElement::Pointer>::iterator iter;
  for (iter = this->m_QuaternionTrackingDataList.begin(); iter != this->m_QuaternionTrackingDataList.end(); iter ++)
    {
    strncpy((char*)element->name, (*iter)->GetName(), IGTL_QTDATA_LEN_NAME);
    element->type = (*iter)->GetType();
    element->reserved = 0;
    float p[3];
    (*iter)->GetPosition(p);
    float q[4];
    (*iter)->GetQuaternion(q);

    for (int i = 0; i < 3; i ++)
      {
      element->position[i] = p[i];
      }
    for (int j = 0; j < 4; j ++)
      {
      element->quaternion[j] = q[j];
      }

    element ++;
    }
  
  igtl_qtdata_convert_byte_order(elementHolder, this->m_QuaternionTrackingDataList.size());
  
  return 1;
}


int QuaternionTrackingDataMessage::UnpackContent()
{
  this->m_QuaternionTrackingDataList.clear();

  igtl_qtdata_element* element = NULL;
  int nElement = 0;
#if OpenIGTLink_HEADER_VERSION >= 2
  element = (igtl_qtdata_element*) (this->m_Content);
  nElement = igtl_qtdata_get_data_n(CalculateReceiveContentSize());
#elif OpenIGTLink_PROTOCOL_VERSION <=2
  element = (igtl_qtdata_element*) this->m_Body;
  nElement = igtl_qtdata_get_data_n(this->m_BodySizeToRead);
#endif
  igtl_qtdata_convert_byte_order(element, nElement);
  
  char strbuf[128];
  for (int i = 0; i < nElement; i ++)
    {
    QuaternionTrackingDataElement::Pointer elemClass = QuaternionTrackingDataElement::New();

    // Add '\n' at the end of each string
    // (necessary for a case, where a string reaches the maximum length.)
    strbuf[IGTL_QTDATA_LEN_NAME] = '\n';
    strncpy(strbuf, (char*)element->name, IGTL_QTDATA_LEN_NAME);
    elemClass->SetName((const char*)strbuf);
    elemClass->SetType(element->type);

    float p[3] = {0, 0, 0};
    float q[4] = {0, 0, 0, 1};

    for (int i = 0; i < 3; i ++)
      {
      p[i] = element->position[i];
      }
    for (int j = 0; j < 4; j ++)
      {
      q[j] = element->quaternion[j];
      }

    elemClass->SetPosition(p);
    elemClass->SetQuaternion(q);

    this->m_QuaternionTrackingDataList.push_back(elemClass);

    element ++;
    }

  return 1;
}

StopQuaternionTrackingDataMessage::StopQuaternionTrackingDataMessage()
{
  this->m_SendMessageType  = "STP_QTDATA";
}

} // namespace igtl