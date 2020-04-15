/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlPositionMessage_h
#define __igtlPositionMessage_h

#include "igtlObject.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

namespace igtl
{

/// The POSITION data type is used to transfer position and orientation information.
/// The data are a combination of 3-dimensional vector for the position and quaternion
/// for the orientation. Although equivalent position and orientation can be described
/// with the TRANSFORM data type, the POSITION data type has the advantage of smaller
/// data size (19%). It is therefore more suitable for pushing high frame-rate data
/// from tracking devices.
class IGTLCommon_EXPORT PositionMessage: public MessageBase
{
public:

  /// Types of message formats. The format of the POSITION message type can contain
  /// only a 3-element position vector (POSITION_ONLY), a combination of 3-element
  /// position vector and 3-element quaternion (WITH_QUATERNION3), or a combination
  /// of 3-element position vector and 4-element quaternion (ALL). 
  enum {
    POSITION_ONLY =  1,
    WITH_QUATERNION3,
    ALL,
  };

public:
  igtlTypeMacro(igtl::PositionMessage, igtl::MessageBase);
  igtlNewMacro(igtl::PositionMessage);

public:

  /// Initializes the class.
  void         Init();

  /// Sets the type of the pack. 't' must be either POSITION_ONLY, WITH_QUATERNION3, or ALL.
  void         SetPackType(int t); /* POSITION_ONLY / WITH_QUATERNION3 / ALL */

  /// Gets the type of the pack. The returned value must be either POSITION_ONLY, WITH_QUATERNION3, or ALL.
  int          GetPackType()  { return  m_PackType; };

  /// Specifies the pack type by body size (in most case obtained from general header).
  int          SetPackTypeByContentSize(int s);

  /// Sets the position by 3-element array of x, y, and z coordinates.
  void         SetPosition(const float* pos);

  /// Sets the position by x, y, and z coordinates.
  void         SetPosition(float x, float y, float z);

  /// Sets the quaternion by 4-element array.
  void         SetQuaternion(const float* quat);

  /// Sets the quaternion by elements of the quaternion (ox, oy, oz and w).
  void         SetQuaternion(float ox, float oy, float oz, float w);

  /// Gets the position. The function substitutes 3-element array of x, y and z coordinates in 'pos'.
  void         GetPosition(float* pos);

  /// Gets the position. The function substitutes the coordinates in 'x', 'y', and 'z'.
  void         GetPosition(float* x, float* y, float* z);

  /// Gets the quaternion. The function substitutes the array of elements of the quaternion in 'quat'.
  void         GetQuaternion(float* quat);

  /// Gets the quaternion. The function substitutes the elements of the quaternion in 'ox', 'oy', 'oz' and 'w'.
  void         GetQuaternion(float* ox, float* oy, float* oz, float* w);

protected:
  PositionMessage();
  ~PositionMessage();
  
protected:

  virtual int  CalculateContentBufferSize();
  virtual int  PackContent();
  virtual int  UnpackContent();

  /// The type of message formats (either POSITION_ONLY, WITH_QUATERNION3, or ALL).
  igtlInt32    m_PackType;

  /// An array of x, y, and z coordinates for the position.
  igtlFloat32  m_Position[3];

  /// An array of ox, oy, oz, and w elements for the quaternion.
  igtlFloat32  m_Quaternion[4];

};


} // namespace igtl

#endif // _igtlPositionMessage_h



