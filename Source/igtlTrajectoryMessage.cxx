/*=========================================================================
  
  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlTrajectoryMessage.h"

#include "igtl_header.h"
#include "igtl_trajectory.h"

// Disable warning C4996 (strncpy() may be unsafe) in Windows. 
#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <vector>

namespace igtl {


//----------------------------------------------------------------------
// igtl::TrajectoryElement class

TrajectoryElement::TrajectoryElement() : Object()
{
  this->m_Name        = "";
  this->m_GroupName   = "";
  this->m_Type        = TYPE_TARGET_ONLY;
  this->m_RGBA[0]     = 0;
  this->m_RGBA[1]     = 0;
  this->m_RGBA[2]     = 0;
  this->m_RGBA[3]     = 0;
  this->m_EntryPosition[0] = 0.0;
  this->m_EntryPosition[1] = 0.0;
  this->m_EntryPosition[2] = 0.0;
  this->m_TargetPosition[0] = 0.0;
  this->m_TargetPosition[1] = 0.0;
  this->m_TargetPosition[2] = 0.0;
  this->m_Radius      = 0.0;
  this->m_Owner       = "";
}


TrajectoryElement::~TrajectoryElement()
{
}


int TrajectoryElement::SetName(const char* name)
{
  if (name != NULL && strlen(name) <= IGTL_TRAJECTORY_LEN_NAME)
    {
    this->m_Name = name;
    return 1;
    }
  else
    {
    return 0;
    }
}


int TrajectoryElement::SetGroupName(const char* grpname)
{
  if (grpname != NULL && strlen(grpname) <= IGTL_TRAJECTORY_LEN_GROUP_NAME)
    {
    this->m_GroupName = grpname;
    return 1;
    }
  else
    {
    return 0;
    }
}


int TrajectoryElement::SetType(igtlUint8 type)
{
  if (type == TYPE_ENTRY_ONLY||
      type == TYPE_TARGET_ONLY||
      TYPE_ENTRY_TARGET)
    {
    this->m_Type = type;
    return 1;
    }
  else
    {
    return 0;
    }
}


void TrajectoryElement::SetRGBA(igtlUint8 rgba[4])
{
  this->m_RGBA[0] = rgba[0];
  this->m_RGBA[1] = rgba[1];
  this->m_RGBA[2] = rgba[2];
  this->m_RGBA[3] = rgba[3];
}


void TrajectoryElement::SetRGBA(igtlUint8 r, igtlUint8 g, igtlUint8 b, igtlUint8 a)
{
  this->m_RGBA[0] = r;
  this->m_RGBA[1] = g;
  this->m_RGBA[2] = b;
  this->m_RGBA[3] = a;
}


void TrajectoryElement::GetRGBA(igtlUint8* rgba)
{
  rgba[0] = this->m_RGBA[0];
  rgba[1] = this->m_RGBA[1];
  rgba[2] = this->m_RGBA[2];
  rgba[3] = this->m_RGBA[3];
}  



void TrajectoryElement::GetRGBA(igtlUint8& r, igtlUint8& g, igtlUint8& b, igtlUint8& a)
{
  r = this->m_RGBA[0];
  g = this->m_RGBA[1];
  b = this->m_RGBA[2];
  a = this->m_RGBA[3];
}

void TrajectoryElement::SetEntryPosition(igtlFloat32 position[3])
{
  this->m_EntryPosition[0] = position[0];
  this->m_EntryPosition[1] = position[1];
  this->m_EntryPosition[2] = position[2];
}


void TrajectoryElement::SetEntryPosition(igtlFloat32 x, igtlFloat32 y, igtlFloat32 z)
{
  this->m_EntryPosition[0] = x;
  this->m_EntryPosition[1] = y;
  this->m_EntryPosition[2] = z;
}


void TrajectoryElement::GetEntryPosition(igtlFloat32* position)
{
  position[0] = this->m_EntryPosition[0];
  position[1] = this->m_EntryPosition[1];
  position[2] = this->m_EntryPosition[2];
}


void TrajectoryElement::GetEntryPosition(igtlFloat32& x, igtlFloat32& y, igtlFloat32& z)
{
  x = this->m_EntryPosition[0];
  y = this->m_EntryPosition[1];
  z = this->m_EntryPosition[2];
}


void TrajectoryElement::SetTargetPosition(igtlFloat32 position[3])
{
  this->m_TargetPosition[0] = position[0];
  this->m_TargetPosition[1] = position[1];
  this->m_TargetPosition[2] = position[2];
}


void TrajectoryElement::SetTargetPosition(igtlFloat32 x, igtlFloat32 y, igtlFloat32 z)
{
  this->m_TargetPosition[0] = x;
  this->m_TargetPosition[1] = y;
  this->m_TargetPosition[2] = z;
}


void TrajectoryElement::GetTargetPosition(igtlFloat32* position)
{
  position[0] = this->m_TargetPosition[0];
  position[1] = this->m_TargetPosition[1];
  position[2] = this->m_TargetPosition[2];
}


void TrajectoryElement::GetTargetPosition(igtlFloat32& x, igtlFloat32& y, igtlFloat32& z)
{
  x = this->m_TargetPosition[0];
  y = this->m_TargetPosition[1];
  z = this->m_TargetPosition[2];
}


int TrajectoryElement::SetOwner(const char* owner)
{
  if (strlen(owner) <= IGTL_TRAJECTORY_LEN_OWNER)
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
// igtl::TrajectoryMessage class
TrajectoryMessage::TrajectoryMessage()
{
  this->m_SendMessageType = "TRAJ";
  this->m_TrajectoryList.clear();
}


TrajectoryMessage::~TrajectoryMessage()
{
}


int TrajectoryMessage::AddTrajectoryElement(TrajectoryElement::Pointer& elem)
{
  m_IsBodyPacked = false;
  this->m_TrajectoryList.push_back(elem);
  return this->m_TrajectoryList.size();
}


void TrajectoryMessage::ClearTrajectoryElement(TrajectoryElement::Pointer& elem)
{
  igtlLegacyReplaceBodyMacro(TrajectoryMessage::ClearTrajectoryElement, 3.0, TrajectoryMessage::ClearAllTrajectoryElements);
  (void)(elem); // Unused variable
  this->m_TrajectoryList.clear();
}

  
void TrajectoryMessage::ClearAllTrajectoryElements()
{
  this->m_TrajectoryList.clear();
}

int TrajectoryMessage::GetNumberOfTrajectoryElement()
{
  return this->m_TrajectoryList.size();
}


void TrajectoryMessage::GetTrajectoryElement(int index, TrajectoryElement::Pointer& elem)
{
  if (index >= 0 && index < (int)this->m_TrajectoryList.size())
    {
    elem = this->m_TrajectoryList[index];
    }
}


igtlUint64 TrajectoryMessage::CalculateContentBufferSize()
{
  return IGTL_TRAJECTORY_ELEMENT_SIZE * this->m_TrajectoryList.size();
}


int TrajectoryMessage::PackContent()
{
  // Allocate buffer
  AllocateBuffer();
  
  igtl_trajectory_element* element = NULL;
  element = (igtl_trajectory_element*)(this->m_Content);

  igtl_trajectory_element * elementHolder = element;
  std::vector<TrajectoryElement::Pointer>::iterator iter;
  for (iter = this->m_TrajectoryList.begin(); iter != this->m_TrajectoryList.end(); iter ++)
    {
    strncpy((char*)element->name,       (*iter)->GetName(),      IGTL_TRAJECTORY_LEN_NAME);
    strncpy((char*)element->group_name, (*iter)->GetGroupName(), IGTL_TRAJECTORY_LEN_GROUP_NAME);
    
    element->type = (*iter)->GetType();
    element->reserved = 0;
    igtlUint8 rgba[4];
    (*iter)->GetRGBA(rgba);
    element->rgba[0] = rgba[0];
    element->rgba[1] = rgba[1];
    element->rgba[2] = rgba[2];
    element->rgba[3] = rgba[3];

    igtlFloat32 position[3];
    (*iter)->GetEntryPosition(position);
    element->entry_pos[0] = position[0];
    element->entry_pos[1] = position[1];
    element->entry_pos[2] = position[2];

    (*iter)->GetTargetPosition(position);
    element->target_pos[0] = position[0];
    element->target_pos[1] = position[1];
    element->target_pos[2] = position[2];

    element->radius = (*iter)->GetRadius();

    strncpy((char*)element->owner_name,  (*iter)->GetOwner(), IGTL_TRAJECTORY_LEN_OWNER);
    
    element ++;
    }

  igtl_trajectory_convert_byte_order(elementHolder, this->m_TrajectoryList.size());

  return 1;
}


int TrajectoryMessage::UnpackContent()
{
  this->m_TrajectoryList.clear();

  igtl_trajectory_element* element = NULL;
  int nElement = 0;
#if OpenIGTLink_HEADER_VERSION >= 2
  element = (igtl_trajectory_element*)(this->m_Content);
  bool isUnpacked(true);
  nElement = igtl_trajectory_get_data_n(CalculateReceiveContentSize(isUnpacked));
#elif OpenIGTLink_PROTOCOL_VERSION <=2
  element = (igtl_trajectory_element*)this->m_Body;
  nElement = igtl_trajectory_get_data_n(this->m_BodySizeToRead);
#endif
  
  igtl_trajectory_convert_byte_order(element, nElement);
  
  char strbuf[128];
  for (int i = 0; i < nElement; i ++)
    {
    TrajectoryElement::Pointer elemClass = TrajectoryElement::New();

    // Add '\n' at the end of each string
    // (necessary for a case, where a string reaches the maximum length.)
    strbuf[IGTL_TRAJECTORY_LEN_NAME] = '\n';
    strncpy(strbuf, (char*)element->name, IGTL_TRAJECTORY_LEN_NAME);
    elemClass->SetName((const char*)strbuf);
    
    strbuf[IGTL_TRAJECTORY_LEN_GROUP_NAME] = '\n';
    strncpy(strbuf, (char*)element->group_name, IGTL_TRAJECTORY_LEN_GROUP_NAME);
    elemClass->SetGroupName(strbuf);

    elemClass->SetType(element->type);

    elemClass->SetRGBA(element->rgba);
    elemClass->SetEntryPosition(element->entry_pos);
    elemClass->SetTargetPosition(element->target_pos);
    elemClass->SetRadius(element->radius);

    strbuf[IGTL_TRAJECTORY_LEN_OWNER] = '\n';
    strncpy(strbuf, (char*)element->owner_name, IGTL_TRAJECTORY_LEN_OWNER);
    elemClass->SetOwner(strbuf);

    this->m_TrajectoryList.push_back(elemClass);

    element ++;
    }

  return 1;
}

} // namespace igtl
