/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlPointMessage_h
#define __igtlPointMessage_h

#include <vector>
#include <string>

#include "igtlObject.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

#include "igtlImageMessage.h"

namespace igtl
{


/// A class to manage point information.
class IGTLCommon_EXPORT PointElement: public Object
{
public:
  igtlTypeMacro(igtl::PointElement, igtl::Object);
  igtlNewMacro(igtl::PointElement);

public:

  /// Sets the name/description of the point. The string 'name' must not exceed 64 characters.
  int           SetName(const char* name);

  /// Gets the name/description of the point.
  const char*   GetName()                            { return this->m_Name.c_str(); };

  /// Sets the group name e.g. "Labeled Point", "Landmark", "Fiducial", etc.
  int           SetGroupName(const char* grpname);

  /// Gets the group name
  const char*   GetGroupName()                       { return this->m_GroupName.c_str(); };

  /// Sets the color of the point specified by an array of R, G, B and A.
  void          SetRGBA(igtlUint8 rgba[4]);

  /// Sets the color of the point specified by values of R, G, B and A.
  void          SetRGBA(igtlUint8 r, igtlUint8 g, igtlUint8 b, igtlUint8 a);

  /// Gets the color of the point using an array of R, G, B and A.
  void          GetRGBA(igtlUint8* rgba);

  /// Gets the color of the point using values of R, G, B and A.
  void          GetRGBA(igtlUint8& r, igtlUint8& g, igtlUint8& b, igtlUint8& a);

  /// Sets the position of the point by an array of x, y and z coordinates.
  void          SetPosition(igtlFloat32 position[3]);

  /// Sets the position of the point by x, y and z coordinates.
  void          SetPosition(igtlFloat32 x, igtlFloat32 y, igtlFloat32 z);

  /// Gets the position of the point using an array of x, y and z coordinates.
  void          GetPosition(igtlFloat32* position);

  /// Gets the position of the point using x, y and z coordinates.
  void          GetPosition(igtlFloat32& x, igtlFloat32& y, igtlFloat32& z);

  /// Sets the radius of the point.
  void          SetRadius(igtlFloat32 radius)        { this->m_Radius = radius; };

  /// Gets the radius of the point.
  igtlFloat32   GetRadius()                          { return this->m_Radius; };

  /// Sets the name of the image that owns this label map.
  int           SetOwner(const char* owner);

  /// Gets the name of the image that owns this label map.
  const char*   GetOwner()                           { return this->m_Owner.c_str(); };

protected:
  PointElement();
  ~PointElement();

protected:

  /// name / description (< 64 bytes)
  std::string   m_Name;

  /// Can be "Labeled Point", "Landmark", Fiducial", ...
  std::string   m_GroupName;

  /// Color in R/G/B/A
  igtlUint8     m_RGBA[4];

  /// Position
  igtlFloat32   m_Position[3];

  /// Radius of the point. Can be 0.
  igtlFloat32   m_Radius;

  /// Device name of the ower image
  std::string   m_Owner; 
};

/// A class for the GET_POINT message type.
class IGTLCommon_EXPORT GetPointMessage: public MessageBase
{
public:
  igtlTypeMacro(igtl::GetPointMessage, igtl::MessageBase);
  igtlNewMacro(igtl::GetPointMessage);

protected:
  GetPointMessage() : MessageBase() { this->m_SendMessageType  = "GET_POINT"; };
  ~GetPointMessage() override {};
protected:
  igtlUint64 CalculateContentBufferSize() override { return 0; };
  int  PackContent() override        { AllocateBuffer(); return 1; };
  int  UnpackContent() override      { return 1; };
};


/// A class for the POINT message type.
/// The POINT message type is designed to transfer information about fiducials, which are often used in surgical planning and navigation in the image-guided therapy.
class IGTLCommon_EXPORT PointMessage: public MessageBase
{
public:
  igtlTypeMacro(igtl::PointMessage, igtl::MessageBase);
  igtlNewMacro(igtl::PointMessage);

public:

  /// Adds a point to the list. It returns the number of points in the list after 
  /// adding the point.
  int  AddPointElement(PointElement::Pointer& elem);

  /// Clears the points in the list.
  void ClearPointElement();

  /// Gets the number of points in the list.
  int  GetNumberOfPointElement();

  /// Gets a pointer to the point specified by 'index'.
  void GetPointElement(int index, PointElement::Pointer& elem);


protected:
  PointMessage();
  ~PointMessage() override;
  
protected:

  igtlUint64 CalculateContentBufferSize() override;
  int  PackContent() override;
  int  UnpackContent() override;

  /// A list of pointers to the points.
  std::vector<PointElement::Pointer> m_PointList;
  
};


} // namespace igtl

#endif // _igtlPointMessage_h