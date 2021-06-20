/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlPositionMessage.h"

#include "igtl_header.h"
#include "igtl_position.h"

#include <string>
#include <cmath>

namespace igtl {

PositionMessage::PositionMessage():
  MessageBase()
{
  Init();
  m_SendMessageType  = "POSITION";
}

PositionMessage::~PositionMessage()
{
}

void PositionMessage::Init()
{
  this->m_PackType      = ALL;

  this->m_Position[0]   = 0.0;
  this->m_Position[1]   = 0.0;
  this->m_Position[2]   = 0.0;

  this->m_Quaternion[0] = 0.0;
  this->m_Quaternion[1] = 0.0;
  this->m_Quaternion[2] = 0.0;
  this->m_Quaternion[3] = 1.0;
}

void PositionMessage::SetPackType(int t)
{
  if (t >= POSITION_ONLY && t <= ALL)
    {
    this->m_PackType = t;
    }
}

int PositionMessage::SetPackTypeByContentSize(int s)
{
  m_IsBodyPacked = false;
  if (s == IGTL_POSITION_MESSAGE_POSITON_ONLY_SIZE)
    {
    this->m_PackType = POSITION_ONLY;
    }
  else if (s == IGTL_POSITION_MESSAGE_WITH_QUATERNION3_SIZE)
    {
    this->m_PackType = WITH_QUATERNION3;
    }
  else if (s == IGTL_POSITION_MESSAGE_DEFAULT_SIZE)
    {
    this->m_PackType = ALL;
    }
  else
    {
    // Do any error handling?
    this->m_PackType = ALL;
    return 0;
    }

  return this->m_PackType;
}

void PositionMessage::SetPosition(const float* pos)
{
  m_IsBodyPacked = false;
  this->m_Position[0] = pos[0];
  this->m_Position[1] = pos[1];
  this->m_Position[2] = pos[2];
}

void PositionMessage::SetPosition(float x, float y, float z)
{
  m_IsBodyPacked = false;
  this->m_Position[0] = x;
  this->m_Position[1] = y;
  this->m_Position[2] = z;
}


void PositionMessage::SetQuaternion(const float* quat)
{
  m_IsBodyPacked = false;
  this->m_Quaternion[0] = quat[0];
  this->m_Quaternion[1] = quat[1];
  this->m_Quaternion[2] = quat[2];
  this->m_Quaternion[3] = quat[3];
}

void PositionMessage::SetQuaternion(float ox, float oy, float oz, float w)
{
  m_IsBodyPacked = false;
  this->m_Quaternion[0] = ox;
  this->m_Quaternion[1] = oy;
  this->m_Quaternion[2] = oz;
  this->m_Quaternion[3] = w;
}

void PositionMessage::GetPosition(float* pos)
{
  pos[0] = this->m_Position[0];
  pos[1] = this->m_Position[1];
  pos[2] = this->m_Position[2];
}

void PositionMessage::GetPosition(float* x, float* y, float* z)
{
  *x = this->m_Position[0];
  *y = this->m_Position[1];
  *z = this->m_Position[2];
}

void PositionMessage::GetQuaternion(float* quat)
{
  quat[0] = this->m_Quaternion[0];
  quat[1] = this->m_Quaternion[1];
  quat[2] = this->m_Quaternion[2];
  quat[3] = this->m_Quaternion[3];
}

void PositionMessage::GetQuaternion(float* ox, float* oy, float* oz, float* w)
{
  *ox = this->m_Quaternion[0];
  *oy = this->m_Quaternion[1];
  *oz = this->m_Quaternion[2];
  *w  = this->m_Quaternion[3];
}

igtlUint64 PositionMessage::CalculateContentBufferSize()
{
  igtlUint64 ret;

  switch (this->m_PackType)
    {
    case POSITION_ONLY:
      ret = IGTL_POSITION_MESSAGE_POSITON_ONLY_SIZE;
      break;
    case WITH_QUATERNION3:
      ret = IGTL_POSITION_MESSAGE_WITH_QUATERNION3_SIZE;
      break;
    default:
      ret = IGTL_POSITION_MESSAGE_DEFAULT_SIZE;
      break;
    }

  return ret;
}

int PositionMessage::PackContent()
{
  // Allocate buffer
  AllocateBuffer();
  igtl_position* p = NULL;
#if OpenIGTLink_HEADER_VERSION >= 2
  p = (igtl_position*)(this->m_Content);
#elif OpenIGTLink_PROTOCOL_VERSION <=2
  p = (igtl_position*)(this->m_Body);
#endif

  p->position[0]   = this->m_Position[0];
  p->position[1]   = this->m_Position[1];
  p->position[2]   = this->m_Position[2];

  if (this->m_PackType == WITH_QUATERNION3)
    {
    p->quaternion[0] = this->m_Quaternion[0];
    p->quaternion[1] = this->m_Quaternion[1];
    p->quaternion[2] = this->m_Quaternion[2];
    }
  else if (this->m_PackType == ALL)
    {
    p->quaternion[0] = this->m_Quaternion[0];
    p->quaternion[1] = this->m_Quaternion[1];
    p->quaternion[2] = this->m_Quaternion[2];
    p->quaternion[3] = this->m_Quaternion[3];
    }

  switch (this->m_PackType)
    {
    case POSITION_ONLY:
      igtl_position_convert_byte_order_position_only(p);
      break;
    case WITH_QUATERNION3:
      igtl_position_convert_byte_order_quaternion3(p);
      break;
    default: //IGTL_POSITION_MESSAGE_DEFAULT_SIZE
      igtl_position_convert_byte_order(p);
      break;
    }

  return 1;
}

int PositionMessage::UnpackContent()
{
  igtl_position* p = NULL;
#if OpenIGTLink_HEADER_VERSION >= 2
  p = (igtl_position*)(this->m_Content);
#elif OpenIGTLink_PROTOCOL_VERSION <=2
  p = (igtl_position*)(this->m_Body);
#endif

  bool isUnpacked(true);
  igtl_uint64 contentSize = CalculateReceiveContentSize(isUnpacked);
  if( !isUnpacked)
  {
    return 0;
  }
  switch (contentSize)
    {
    case IGTL_POSITION_MESSAGE_POSITON_ONLY_SIZE:
      this->m_PackType = POSITION_ONLY;
      igtl_position_convert_byte_order_position_only(p);
      break;
    case IGTL_POSITION_MESSAGE_WITH_QUATERNION3_SIZE:
      this->m_PackType = WITH_QUATERNION3;
      igtl_position_convert_byte_order_quaternion3(p);
      break;
    default: //IGTL_POSITION_MESSAGE_DEFAULT_SIZE
      this->m_PackType = ALL;
      igtl_position_convert_byte_order(p);
      break;
    }

  this->m_Position[0]   = p->position[0];
  this->m_Position[1]   = p->position[1];
  this->m_Position[2]   = p->position[2];

  if (this->m_PackType == POSITION_ONLY)
    {
    this->m_Quaternion[0] = 0.0;
    this->m_Quaternion[1] = 0.0;
    this->m_Quaternion[2] = 0.0;
    this->m_Quaternion[3] = 1.0;
    }

  if (this->m_PackType == WITH_QUATERNION3)
    {
    float ox = p->quaternion[0];
    float oy = p->quaternion[1];
    float oz = p->quaternion[2];
    float sq = 1.0 - (ox*ox + oy*oy + oz*oz);
    if (sq < 0.0)
      {
      // TODO: what should we do with invalid quaternion?
      // Tentatively we set (0, 0, 0, 1);
      this->m_Quaternion[0] = 0.0;
      this->m_Quaternion[1] = 0.0;
      this->m_Quaternion[2] = 0.0;
      this->m_Quaternion[3] = 1.0;
      }
    else
      {
      float w = sqrt(sq);
      this->m_Quaternion[0] = ox;
      this->m_Quaternion[1] = oy;
      this->m_Quaternion[2] = oz;
      this->m_Quaternion[3] = w;
      }
    }
  else if (this->m_PackType == ALL)
    {
    this->m_Quaternion[0] = p->quaternion[0];
    this->m_Quaternion[1] = p->quaternion[1];
    this->m_Quaternion[2] = p->quaternion[2];
    this->m_Quaternion[3] = p->quaternion[3];
    }
    
  return 1;
}

} // namespace igtl
