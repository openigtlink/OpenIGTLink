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
#include "igtl_tdata.h"

// Disable warning C4996 (strncpy() may be unsafe) in Windows. 
#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <vector>

namespace igtl {

//----------------------------------------------------------------------
FullTrackingDataWithErrorMessage::FullTrackingDataWithErrorMessage()
  : TrackingDataMessage()
{
  this->m_DefaultBodyType = "FULLDATA";
}


//----------------------------------------------------------------------
FullTrackingDataWithErrorMessage::~FullTrackingDataWithErrorMessage()
{
}


//----------------------------------------------------------------------
int FullTrackingDataWithErrorMessage::GetBodyPackSize()
{
/*
  // The body size sum of the header size and status message size.
  return IGTL_TDATA_ELEMENT_SIZE * this->m_TrackingDataList.size();
*/
  return 1;
}


//----------------------------------------------------------------------
int FullTrackingDataWithErrorMessage::PackBody()
{
/*
  // allocate pack
  AllocatePack();
  
  igtl_tdata_element* element;
  element = (igtl_tdata_element*)this->m_Body;
  std::vector<TrackingDataElement::Pointer>::iterator iter;

  for (iter = this->m_TrackingDataList.begin(); iter != this->m_TrackingDataList.end(); iter ++)
    {
    strncpy((char*)element->name, (*iter)->GetName(), IGTL_TDATA_LEN_NAME);
    element->type = (*iter)->GetType();

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
  
  igtl_tdata_convert_byte_order((igtl_tdata_element*)this->m_Body, this->m_TrackingDataList.size());
*/
  return 1;
}


//----------------------------------------------------------------------
int FullTrackingDataWithErrorMessage::UnpackBody()
{
/*
  this->m_TrackingDataList.clear();

  igtl_tdata_element* element = (igtl_tdata_element*) this->m_Body;
  int nElement = igtl_tdata_get_data_n(this->m_BodySizeToRead);

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
*/
  return 1;
}

} // namespace igtl





