/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlLabelMetaMessage.h"

#include "igtl_header.h"
#include "igtl_imgmeta.h"
#include "igtl_lbmeta.h"
#include "igtl_image.h"

// Disable warning C4996 (strncpy() may be unsafe) in Windows. 
#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <vector>

namespace igtl {


//----------------------------------------------------------------------
// igtl::LabelMetaElement class

LabelMetaElement::LabelMetaElement() : Object()
{
  this->m_Name       = "";
  this->m_DeviceName = "";
  this->m_Label      = 0;
  this->m_RGBA[0]    = 0;
  this->m_RGBA[1]    = 0;
  this->m_RGBA[2]    = 0;
  this->m_RGBA[3]    = 0;
  this->m_Size[0]    = 0;
  this->m_Size[1]    = 0;
  this->m_Size[2]    = 0;
  this->m_Owner      = "";
}


LabelMetaElement::~LabelMetaElement()
{
}


int LabelMetaElement::SetName(const char* name)
{
  if (strlen(name) <= IGTL_LBMETA_LEN_NAME)
    {
    this->m_Name = name;
    return 1;
    }
  else
    {
    return 0;
    }
}


int LabelMetaElement::SetDeviceName(const char* devname)
{
  if (strlen(devname) <= IGTL_LBMETA_LEN_DEVICE_NAME)
    {
    this->m_DeviceName = devname;
    return 1;
    }
  else
    {
    return 0;
    }
}


void LabelMetaElement::SetRGBA(igtlUint8 rgba[4])
{
  this->m_RGBA[0] = rgba[0];
  this->m_RGBA[1] = rgba[1];
  this->m_RGBA[2] = rgba[2];
  this->m_RGBA[3] = rgba[3];
}


void LabelMetaElement::SetRGBA(igtlUint8 r, igtlUint8 g, igtlUint8 b, igtlUint8 a)
{
  this->m_RGBA[0] = r;
  this->m_RGBA[1] = g;
  this->m_RGBA[2] = b;
  this->m_RGBA[3] = a;
}


void LabelMetaElement::GetRGBA(igtlUint8* rgba)
{
  rgba[0] = this->m_RGBA[0];
  rgba[1] = this->m_RGBA[1];
  rgba[2] = this->m_RGBA[2];
  rgba[3] = this->m_RGBA[3];
}  



void LabelMetaElement::GetRGBA(igtlUint8& r, igtlUint8& g, igtlUint8& b, igtlUint8& a)
{
  r = this->m_RGBA[0];
  g = this->m_RGBA[1];
  b = this->m_RGBA[2];
  a = this->m_RGBA[3];
}


void LabelMetaElement::SetSize(igtlUint16 size[3])
{
  this->m_Size[0] = size[0];
  this->m_Size[1] = size[1];
  this->m_Size[2] = size[2];
}


void LabelMetaElement::SetSize(igtlUint16 si, igtlUint16 sj, igtlUint16 sk)
{
  this->m_Size[0] = si;
  this->m_Size[1] = sj;
  this->m_Size[2] = sk;
}


void LabelMetaElement::GetSize(igtlUint16* size)
{
  size[0] = this->m_Size[0];
  size[1] = this->m_Size[1];
  size[2] = this->m_Size[2];
}


void LabelMetaElement::GetSize(igtlUint16& si, igtlUint16& sj, igtlUint16& sk)
{
  si = this->m_Size[0];
  sj = this->m_Size[1];
  sk = this->m_Size[2];
}


int LabelMetaElement::SetOwner(const char* owner)
{
  if (strlen(owner) <= IGTL_LBMETA_LEN_OWNER)
    {
    this->m_Owner = owner;
    return 1;
    }
  else
    {
    return 0;
    }
  
}


//----------------------------------------------------------------------
// igtl::LabelMetaMessage class

LabelMetaMessage::LabelMetaMessage()
{
  this->m_SendMessageType = "LBMETA";
  this->m_LabelMetaList.clear();
}


LabelMetaMessage::~LabelMetaMessage()
{
}


int LabelMetaMessage::AddLabelMetaElement(LabelMetaElement::Pointer& elem)
{
  m_IsBodyPacked = false;
  this->m_LabelMetaList.push_back(elem);
  return this->m_LabelMetaList.size();
}


void LabelMetaMessage::ClearLabelMetaElement()
{
  this->m_LabelMetaList.clear();
}


int LabelMetaMessage::GetNumberOfLabelMetaElement()
{
  return this->m_LabelMetaList.size();
}


void LabelMetaMessage::GetLabelMetaElement(int index, LabelMetaElement::Pointer& elem)
{
  if (index >= 0 && index < (int)this->m_LabelMetaList.size())
    {
    elem = this->m_LabelMetaList[index];
    }
}


igtlUint64 LabelMetaMessage::CalculateContentBufferSize()
{
  // The body size sum of the header size and status message size.
  return IGTL_LBMETA_ELEMENT_SIZE * this->m_LabelMetaList.size();
}


int LabelMetaMessage::PackContent()
{
  // Allocate buffer
  AllocateBuffer();
  
  igtl_lbmeta_element* element;
  element = (igtl_lbmeta_element*)this->m_Content;
  std::vector<LabelMetaElement::Pointer>::iterator iter;

  for (iter = this->m_LabelMetaList.begin(); iter != this->m_LabelMetaList.end(); iter ++)
    {
    strncpy((char*)element->name,        (*iter)->GetName(),       IGTL_LBMETA_LEN_NAME);
    strncpy((char*)element->device_name, (*iter)->GetDeviceName(), IGTL_LBMETA_LEN_DEVICE_NAME);

    element->label = (*iter)->GetLabel();
    element->reserved = 0;
    igtlUint8 rgba[4];
    (*iter)->GetRGBA(rgba);
    element->rgba[0] = rgba[0];
    element->rgba[1] = rgba[1];
    element->rgba[2] = rgba[2];
    element->rgba[3] = rgba[3];

    igtlUint16 size[3];
    (*iter)->GetSize(size);
    element->size[0] = size[0];
    element->size[1] = size[1];
    element->size[2] = size[2];

    strncpy((char*)element->owner,  (*iter)->GetOwner(),  IGTL_LBMETA_LEN_OWNER);
    
    element ++;
    }

  igtl_lbmeta_convert_byte_order((igtl_lbmeta_element*)this->m_Content, this->m_LabelMetaList.size());

  return 1;
}


int LabelMetaMessage::UnpackContent()
{

  this->m_LabelMetaList.clear();

  igtl_lbmeta_element* element = (igtl_lbmeta_element*) this->m_Content;
  int nElement = igtl_lbmeta_get_data_n(this->m_BodySizeToRead);

  igtl_lbmeta_convert_byte_order(element, nElement);
  
  char strbuf[128];
  for (int i = 0; i < nElement; i ++)
    {
    LabelMetaElement::Pointer elemClass = LabelMetaElement::New();

    // Add '\n' at the end of each string
    // (neccesary for a case, where a string reaches the maximum length.)
    strbuf[IGTL_LBMETA_LEN_NAME] = '\n';
    strncpy(strbuf, (char*)element->name, IGTL_LBMETA_LEN_NAME);
    elemClass->SetName((const char*)strbuf);
    
    strbuf[IGTL_LBMETA_LEN_DEVICE_NAME] = '\n';
    strncpy(strbuf, (char*)element->device_name, IGTL_LBMETA_LEN_DEVICE_NAME);
    elemClass->SetDeviceName(strbuf);
    
    elemClass->SetLabel(element->label);
    elemClass->SetRGBA(element->rgba);
    elemClass->SetSize(element->size);

    strbuf[IGTL_LBMETA_LEN_OWNER] = '\n';
    strncpy(strbuf, (char*)element->owner, IGTL_LBMETA_LEN_OWNER);
    elemClass->SetOwner(strbuf);

    this->m_LabelMetaList.push_back(elemClass);

    element ++;
    }

  return 1;
}

} // namespace igtl
