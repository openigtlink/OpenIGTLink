/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlTransformMessage_h
#define __igtlTransformMessage_h

#include "igtlObject.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"

namespace igtl
{

/// A class for the GET_TRANS message type.
class IGTLCommon_EXPORT GetTransformMessage: public HeaderOnlyMessageBase
{
public:
  igtlTypeMacro(igtl::GetTransformMessage, igtl::HeaderOnlyMessageBase);
  igtlNewMacro(igtl::GetTransformMessage);

protected:
  GetTransformMessage() : HeaderOnlyMessageBase() { this->m_SendMessageType  = "GET_TRANS"; };
  ~GetTransformMessage() {};
};



/// The TRANSFORM data type is used to transfer a homogeneous linear transformation
/// in 4-by-4 matrix form. One such matrix was shown earlier in equation (1).
/// Note that if a device is sending only translation and rotation, then TRANSFORM
/// is equivalent to POSITION. But TRANSFORM can also be used to transfer affine
/// transformations or simple scaling. Like IMAGE and POSITION, TRANSFORM carries
/// information about the coordinate system used.
class IGTLCommon_EXPORT TransformMessage: public MessageBase
{

public:
  igtlTypeMacro(igtl::TransformMessage, igtl::MessageBase);
  igtlNewMacro(igtl::TransformMessage);

public:

  /// Sets a position (or a translation vector) in the RAS coordinate system.
  void SetPosition(float p[3]);

  /// Gets a position (or a translation vector) in the RAS coordinate system. 
  void GetPosition(float p[3]);

  /// Sets a position (or a translation vector) in the RAS coordinate system. 
  void SetPosition(float px, float py, float pz);

  /// Gets a position (or a translation vector) in the RAS coordinate system. 
  void GetPosition(float* px, float* py, float* pz);

  /// Sets normal vectors (or a rotation matrix) in the RAS coordinate system.
  void SetNormals(float o[3][3]);

  /// Gets normal vectors (or a rotation matrix) in the RAS coordinate system.
  void GetNormals(float o[3][3]);

  /// Sets normal vectors (or a rotation matrix) in the RAS coordinate system.
  void SetNormals(float t[3], float s[3], float n[3]);

  /// Gets normal vectors (or a rotation matrix) in the RAS coordinate system.
  void GetNormals(float t[3], float s[3], float n[3]);

  /// Sets rotation matrix using igtl::Matrix4x4. 
  void SetMatrix(Matrix4x4& mat);

  /// Sets rotation matrix using igtl::Matrix4x4. 
  void GetMatrix(Matrix4x4& mat);


protected:
  TransformMessage();
  ~TransformMessage();
  
protected:

  igtlUint64 CalculateContentBufferSize() override;
  int  PackContent() override;
  int  UnpackContent() override;
  
  /// The transformation matrix.
  Matrix4x4 matrix;

  /// The byte array for the serialized transform data.
  unsigned char*  m_Transform;

};


} // namespace igtl

#endif // _igtlTransformMessage_h



