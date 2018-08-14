/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlTrajectoryMessage_h
#define __igtlTrajectoryMessage_h

#include <vector>
#include <string>

#include "igtlObject.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

#include "igtlImageMessage.h"

namespace igtl
{

/// TrajectoryElement class is used to manage a trajectory in TrajectoryMessage class.
class IGTLCommon_EXPORT TrajectoryElement: public Object
{
public:
  igtlTypeMacro(igtl::TrajectoryElement, igtl::Object);
  igtlNewMacro(igtl::TrajectoryElement);

  /// Types of trajectory. 
  enum {
    TYPE_ENTRY_ONLY   = 1, /* Trajectory with only entry point */
    TYPE_TARGET_ONLY  = 2, /* Trajectory with only target point */
    TYPE_ENTRY_TARGET = 3, /* Trajectory with entry and target point */
  };

public:

  /// Sets the name of the trajectory.
  int           SetName(const char* name);

  /// Gets the name of the trajectory.
  const char*   GetName()                            { return this->m_Name.c_str(); };

  /// Sets the group name e.g. "Trajectory"
  int           SetGroupName(const char* grpname);

  /// Gets the group name.
  const char*   GetGroupName()                       { return this->m_GroupName.c_str(); };

  /// Sets the trajectory type. 'type' must be either TYPE_ENTRY_ONLY, TYPE_TARGET_ONLY, or TYPE_ENTRY_TARGET.
  int           SetType(igtlUint8 type);
  /// Gets the trajectory type. The returned value is either TYPE_ENTRY_ONLY, TYPE_TARGET_ONLY, or TYPE_ENTRY_TARGET.
  igtlUint8     GetType()                            { return this->m_Type; };

  /// Sets the color of the trajectory using an array of r, g, b and alpha.
  void          SetRGBA(igtlUint8 rgba[4]);

  /// Sets the color of the trajectory by r, g, b and alpha.
  void          SetRGBA(igtlUint8 r, igtlUint8 g, igtlUint8 b, igtlUint8 a);

  /// Gets the color of the trajectory. An array of r, g, b and alpha is stored in 'rgba'
  void          GetRGBA(igtlUint8* rgba);

  /// Gets the color of the trajectory.
  void          GetRGBA(igtlUint8& r, igtlUint8& g, igtlUint8& b, igtlUint8& a);

  /// Sets the entry position using an array.
  void          SetEntryPosition(igtlFloat32 position[3]);

  /// Sets the entry position.
  void          SetEntryPosition(igtlFloat32 x, igtlFloat32 y, igtlFloat32 z);

  /// Sets the entry position using an array of x, y, and z coordinates.
  void          GetEntryPosition(igtlFloat32* position);

  /// Gets the entry position.
  void          GetEntryPosition(igtlFloat32& x, igtlFloat32& y, igtlFloat32& z);

  /// Sets the target position using an array of x, y, and z coordinates.
  void          SetTargetPosition(igtlFloat32 position[3]);

  /// Sets the target position.
  void          SetTargetPosition(igtlFloat32 x, igtlFloat32 y, igtlFloat32 z);

  /// Gets the target position. Stores an array of x, y, and z coordinates in 'position'
  void          GetTargetPosition(igtlFloat32* position);

  /// Gets the target position.
  void          GetTargetPosition(igtlFloat32& x, igtlFloat32& y, igtlFloat32& z);

  /// Sets the radius. 
  void          SetRadius(igtlFloat32 radius)        { this->m_Radius = radius; };

  /// Gets the radius. 
  igtlFloat32   GetRadius()                          { return this->m_Radius; };

  /// Sets the owner of the trajectory. 'owner' must be a name of image.
  int           SetOwner(const char* owner);

  /// Gets the owner of the trajectory.
  const char*   GetOwner()                           { return this->m_Owner.c_str(); };

protected:
  TrajectoryElement();
  ~TrajectoryElement();

protected:

  /// name / description (< 64 bytes)
  std::string   m_Name;

  /// Can be "Labeled Trajectory", "Landmark", Fiducial", ...
  std::string   m_GroupName;

  /// Trajectory type (see TYPE_* constants)
  igtlUint8     m_Type;

  /// Color in R/G/B/A
  igtlUint8     m_RGBA[4];

  /// Coordinate of the entry point
  igtlFloat32   m_EntryPosition[3];

  /// Coordinate of the target point
  igtlFloat32   m_TargetPosition[3];

  /// Radius of the trajectory. Can be 0.
  igtlFloat32   m_Radius;

  /// Device name of the ower image.
  std::string   m_Owner;
};


/// A class for the GET_TRAJ message type.
class IGTLCommon_EXPORT GetTrajectoryMessage: public MessageBase
{
public:
  igtlTypeMacro(igtl::GetTrajectoryMessage, igtl::MessageBase);
  igtlNewMacro(igtl::GetTrajectoryMessage);

protected:
  GetTrajectoryMessage() : MessageBase() { this->m_SendMessageType  = "GET_TRAJ"; };
  ~GetTrajectoryMessage() {};
protected:
  virtual int  CalculateContentBufferSize() { return 0; };
  virtual int  PackContent()        { AllocateBuffer(); return 1; };
  virtual int  UnpackContent()      { return 1; };
};


/// The TRAJECTORY message type support to transfer information about 3D trajectory,
/// which is often used in surgical planning and guidance in image-guided therapy.
class IGTLCommon_EXPORT TrajectoryMessage: public MessageBase
{
public:
  igtlTypeMacro(igtl::TrajectoryMessage, igtl::MessageBase);
  igtlNewMacro(igtl::TrajectoryMessage);

public:

  /// Adds a trajectory to the list.
  int  AddTrajectoryElement(TrajectoryElement::Pointer& elem);
  
  /// Clears the all trajectory from the list.
  igtlLegacyMacro(
  void ClearTrajectoryElement(TrajectoryElement::Pointer& elem));
  
  /// Clears all trajectory elements from the list.
  void ClearAllTrajectoryElements();

  /// Gets the number of trajectory in the list.
  int  GetNumberOfTrajectoryElement();

  /// Gets the trajectory specified by 'index'.
  void GetTrajectoryElement(int index, TrajectoryElement::Pointer& elem);


protected:
  TrajectoryMessage();
  ~TrajectoryMessage();
  
protected:

  virtual int  CalculateContentBufferSize();
  virtual int  PackContent();
  virtual int  UnpackContent();
  
  /// A list of pointers to the trajectories.
  std::vector<TrajectoryElement::Pointer> m_TrajectoryList;
  
};

} // namespace igtl

#endif // _igtlTrajectoryMessage_h