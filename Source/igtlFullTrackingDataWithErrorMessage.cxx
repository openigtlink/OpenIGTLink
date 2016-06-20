/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlFullTrackingDataWithErrorMessage.h"

#include "igtl_header.h"
#include "igtl_fulltdata.h"
#include "igtlMacro.h"

// Disable warning C4996 (strncpy() may be unsafe) in Windows. 
#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <vector>

namespace igtl {

//----------------------------------------------------------------------
FullTrackingDataWithErrorMessage::FullTrackingDataWithErrorMessage()
  : TrackingDataMessage()
{
  this->m_DefaultBodyType = "FULLTDATA";
}


//----------------------------------------------------------------------
FullTrackingDataWithErrorMessage::~FullTrackingDataWithErrorMessage()
{
}


//----------------------------------------------------------------------
int FullTrackingDataWithErrorMessage::GetBodyPackSize()
{
  // The body size sum of the header size and status message size.
  return IGTL_FULLTDATA_ELEMENT_SIZE * this->m_TrackingDataList.size();
}


//----------------------------------------------------------------------
int FullTrackingDataWithErrorMessage::PackBody()
{
  // allocate pack
  AllocatePack();
  
  igtl_fulltdata_element* element;
  element = (igtl_fulltdata_element*)this->m_Body;
  std::vector<TrackingDataElement::Pointer>::iterator iter;

  for (iter = this->m_TrackingDataList.begin(); iter != this->m_TrackingDataList.end(); iter ++)
  {
    TrackingDataWithErrorElement* elem = dynamic_cast<TrackingDataWithErrorElement*>((*iter).GetPointer());
    if (elem == NULL)
    {
      igtlExceptionMacro("Found list item that is not a TrackingDataWithErrorElement");
    }

    strncpy((char*)element->name, (*iter)->GetName(), IGTL_FULLTDATA_LEN_NAME);
    element->type = (*iter)->GetType();

    Matrix4x4 matrix;
    (*iter)->GetMatrix(matrix);
    for (int i = 0; i < 4; i ++)
    {
      element->transform[i]   = matrix[i][0];
      element->transform[i+4] = matrix[i][1];
      element->transform[i+8] = matrix[i][2];
      element->transform[i+12] = matrix[i][3];
    }

    float error;
    error = elem->GetError();
    element->error = error;

    element ++;
  }
  
  igtl_fulltdata_convert_byte_order((igtl_fulltdata_element*)this->m_Body, this->m_TrackingDataList.size());

  return 1;
}


//----------------------------------------------------------------------
int FullTrackingDataWithErrorMessage::UnpackBody()
{

  this->m_TrackingDataList.clear();

  igtl_fulltdata_element* element = (igtl_fulltdata_element*) this->m_Body;
  int nElement = igtl_fulltdata_get_data_n(this->m_BodySizeToRead);

  igtl_fulltdata_convert_byte_order(element, nElement);
  
  char strbuf[128];
  for (int i = 0; i < nElement; i ++)
  {
    igtl::TrackingDataWithErrorElement::Pointer elemClass = TrackingDataWithErrorElement::New();

    // Add '\n' at the end of each string
    // (neccesary for a case, where a string reaches the maximum length.)
    strbuf[IGTL_FULLTDATA_LEN_NAME] = '\n';
    strncpy(strbuf, (char*)element->name, IGTL_FULLTDATA_LEN_NAME);
    elemClass->SetName((const char*)strbuf);
    elemClass->SetType(element->type);

    Matrix4x4 matrix;
    IdentityMatrix(matrix);
    for (int j = 0; j < 4; j ++)
    {
      matrix[j][0] = element->transform[j];
      matrix[j][1] = element->transform[j+4];
      matrix[j][2] = element->transform[j+8];
      matrix[j][3] = element->transform[j+12];
    }
    elemClass->SetMatrix(matrix);

    float error;
    error = element->error;
    elemClass->SetError(error);

    igtl::TrackingDataElement::Pointer pointerToBase = igtl::TrackingDataElement::New();
    pointerToBase = elemClass.GetPointer();

    this->m_TrackingDataList.push_back(pointerToBase);

    element ++;
  }

  return 1;
}

//----------------------------------------------------------------------
void FullTrackingDataWithErrorMessage::GetTrackingDataElement(int index, TrackingDataWithErrorElement::Pointer& elem)
{
  if (index >= 0 && index < (int)this->m_TrackingDataList.size())
  {
    TrackingDataWithErrorElement* element = dynamic_cast<TrackingDataWithErrorElement*>(this->m_TrackingDataList[index].GetPointer());
    if (element == NULL)
    {
      igtlExceptionMacro("Found list item that is not a TrackingDataWithErrorElement");
    }
    igtl::TrackingDataWithErrorElement::Pointer tmp = element;
    elem = tmp;
  }
}

} // namespace igtl





