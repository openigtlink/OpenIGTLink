/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlImageMetaMessage.h"

#include "igtl_header.h"
#include "igtl_imgmeta.h"
#include "igtl_image.h"

// Disable warning C4996 (strncpy() may be unsafe) in Windows. 
#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <vector>

namespace igtl {


//----------------------------------------------------------------------
// igtl::ImageMetaElement class

ImageMetaElement::ImageMetaElement() : Object()
{
  this->m_TimeStamp  = 0;
  this->m_Size[0]    = 0;
  this->m_Size[1]    = 0;
  this->m_Size[2]    = 0;
  this->m_ScalarType = 0;
}


ImageMetaElement::~ImageMetaElement()
{
}


int ImageMetaElement::SetName(const char* name)
{
  if (strlen(name) <= IGTL_IMGMETA_LEN_NAME)
    {
    this->m_Name = name;
    return 1;
    }
  else
    {
    return 0;
    }
}


int ImageMetaElement::SetDeviceName(const char* devname)
{
  if (strlen(devname) <= IGTL_IMGMETA_LEN_DEVICE_NAME)
    {
    this->m_DeviceName = devname;
    return 1;
    }
  else
    {
    return 0;
    }
}


int ImageMetaElement::SetModality(const char* modality)
{
  if (strlen(modality) <= IGTL_IMGMETA_LEN_MODALITY)
    {
    this->m_Modality = modality;
    return 1;
    }
  else
    {
    return 0;
    }
}


int ImageMetaElement::SetPatientName(const char* patname)
{
  if (strlen(patname) <= IGTL_IMGMETA_LEN_PATIENT_NAME)
    {
    this->m_PatientName = patname;
    return 1;
    }
  else
    {
    return 0;
    }
}


int ImageMetaElement::SetPatientID(const char* patid)
{
  if (strlen(patid) <= IGTL_IMGMETA_LEN_PATIENT_ID)
    {
    this->m_PatientID = patid;
    return 1;
    }
  else
    {
    return 0;
    }
}


void ImageMetaElement::SetTimeStamp(igtl::TimeStamp::Pointer& time)
{
  this->m_TimeStamp = time;
}


void ImageMetaElement::GetTimeStamp(igtl::TimeStamp::Pointer& time)
{
  time = this->m_TimeStamp;
}


void ImageMetaElement::SetSize(igtlUint16 size[3])
{
  this->m_Size[0] = size[0];
  this->m_Size[1] = size[1];
  this->m_Size[2] = size[2];
}


void ImageMetaElement::SetSize(igtlUint16 si, igtlUint16 sj, igtlUint16 sk)
{
  this->m_Size[0] = si;
  this->m_Size[1] = sj;
  this->m_Size[2] = sk;
}


void ImageMetaElement::GetSize(igtlUint16* size)
{
  size[0] = this->m_Size[0];
  size[1] = this->m_Size[1];
  size[2] = this->m_Size[2];
}


void ImageMetaElement::GetSize(igtlUint16& si, igtlUint16& sj, igtlUint16& sk)
{
  si = this->m_Size[0];
  sj = this->m_Size[1];
  sk = this->m_Size[2];
}


void ImageMetaElement::SetScalarType(igtlUint8 type)
{
  if (type == IGTL_IMAGE_STYPE_TYPE_INT8    ||
      type == IGTL_IMAGE_STYPE_TYPE_UINT8   ||
      type == IGTL_IMAGE_STYPE_TYPE_INT16   ||
      type == IGTL_IMAGE_STYPE_TYPE_UINT16  ||
      type == IGTL_IMAGE_STYPE_TYPE_INT32   ||
      type == IGTL_IMAGE_STYPE_TYPE_UINT32  ||
      type == IGTL_IMAGE_STYPE_TYPE_FLOAT32 ||
      type == IGTL_IMAGE_STYPE_TYPE_FLOAT64)
    {
    this->m_ScalarType = type;
    }
  else
    {
    this->m_ScalarType = 0;
    }
}


igtlUint8 ImageMetaElement::GetScalarType()
{
  return this->m_ScalarType;
}



//----------------------------------------------------------------------
// igtl::ImageMetaMessage class

ImageMetaMessage::ImageMetaMessage()
{
  this->m_SendMessageType = "IMGMETA";
  this->m_ImageMetaList.clear();
}


ImageMetaMessage::~ImageMetaMessage()
{
}


int ImageMetaMessage::AddImageMetaElement(ImageMetaElement::Pointer& elem)
{
  m_IsBodyPacked = false;
  this->m_ImageMetaList.push_back(elem);
  return this->m_ImageMetaList.size();
}


void ImageMetaMessage::ClearImageMetaElement()
{
  this->m_ImageMetaList.clear();
}


int ImageMetaMessage::GetNumberOfImageMetaElement()
{
  return this->m_ImageMetaList.size();
}


void ImageMetaMessage::GetImageMetaElement(int index, ImageMetaElement::Pointer& elem)
{
  if (index >= 0 && index < (int) this->m_ImageMetaList.size())
    {
    elem = this->m_ImageMetaList[index];
    }
}


igtlUint64 ImageMetaMessage::CalculateContentBufferSize()
{
  // The body size sum of the header size and status message size.
  return IGTL_IMGMETA_ELEMENT_SIZE * this->m_ImageMetaList.size();
}


int ImageMetaMessage::PackContent()
{
  // Allocate buffer
  AllocateBuffer();
  
  igtl_imgmeta_element* element = (igtl_imgmeta_element*)this->m_Content;

  std::vector<ImageMetaElement::Pointer>::iterator iter;
  for (iter = this->m_ImageMetaList.begin(); iter != this->m_ImageMetaList.end(); iter ++)
    {
    strncpy((char*)element->name,         (*iter)->GetName(),        IGTL_IMGMETA_LEN_NAME);
    strncpy((char*)element->device_name,  (*iter)->GetDeviceName(),  IGTL_IMGMETA_LEN_DEVICE_NAME);
    strncpy((char*)element->modality,     (*iter)->GetModality(),    IGTL_IMGMETA_LEN_MODALITY);
    strncpy((char*)element->patient_name, (*iter)->GetPatientName(), IGTL_IMGMETA_LEN_PATIENT_NAME);
    strncpy((char*)element->patient_id,   (*iter)->GetPatientID(),   IGTL_IMGMETA_LEN_PATIENT_ID);

    igtl::TimeStamp::Pointer ts;
    (*iter)->GetTimeStamp(ts);
    if (ts.IsNotNull())
      {
      element->timestamp = ts->GetTimeStampUint64();
      }
    else
      {
      element->timestamp = 0;
      }
    igtlUint16 size[3];
    (*iter)->GetSize(size);
    element->size[0] = size[0];
    element->size[1] = size[1];
    element->size[2] = size[2];
    element->scalar_type = (*iter)->GetScalarType();
    element->reserved = 0;
    element ++;
    }

  igtl_imgmeta_convert_byte_order((igtl_imgmeta_element*)this->m_Content, this->m_ImageMetaList.size());

  return 1;
}


int ImageMetaMessage::UnpackContent()
{

  this->m_ImageMetaList.clear();

  igtl_imgmeta_element* element = (igtl_imgmeta_element*) this->m_Content;
  int nElement = igtl_imgmeta_get_data_n(this->m_BodySizeToRead);

  igtl_imgmeta_convert_byte_order(element, nElement);
  
  char strbuf[128];
  for (int i = 0; i < nElement; i ++)
    {
    ImageMetaElement::Pointer elemClass = ImageMetaElement::New();

    // Add '\n' at the end of each string
    // (necessary for a case, where a string reaches the maximum length.)
    strbuf[IGTL_IMGMETA_LEN_NAME] = '\n';
    strncpy(strbuf, (char*)element->name, IGTL_IMGMETA_LEN_NAME);
    elemClass->SetName((const char*)strbuf);
    
    strbuf[IGTL_IMGMETA_LEN_DEVICE_NAME] = '\n';
    strncpy(strbuf, (char*)element->device_name, IGTL_IMGMETA_LEN_DEVICE_NAME);
    elemClass->SetDeviceName(strbuf);

    strbuf[IGTL_IMGMETA_LEN_MODALITY] = '\n';
    strncpy(strbuf, (char*)element->modality, IGTL_IMGMETA_LEN_MODALITY);
    elemClass->SetModality(strbuf);

    strbuf[IGTL_IMGMETA_LEN_PATIENT_NAME] = '\n';
    strncpy(strbuf, (char*)element->patient_name, IGTL_IMGMETA_LEN_PATIENT_NAME);
    elemClass->SetPatientName(strbuf);

    strbuf[IGTL_IMGMETA_LEN_PATIENT_ID] = '\n';
    strncpy(strbuf, (char*)element->patient_id, IGTL_IMGMETA_LEN_PATIENT_ID);
    elemClass->SetPatientID(strbuf);
    
    TimeStamp::Pointer ts = TimeStamp::New();
    ts->SetTime(element->timestamp);
    elemClass->SetTimeStamp(ts);

    elemClass->SetSize(element->size);
    elemClass->SetScalarType(element->scalar_type);

    this->m_ImageMetaList.push_back(elemClass);

    element ++;
    }

  return 1;
}

} // namespace igtl
