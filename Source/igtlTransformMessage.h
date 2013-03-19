/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlTransformMessage.h $
  Language:  C++
  Date:      $Date: 2008-12-22 19:05:42 -0500 (Mon, 22 Dec 2008) $
  Version:   $Revision: 3460 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlTransformMessage_h
#define __igtlTransformMessage_h

#include "igtlObject.h"
//#include "igtlMacros.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"

namespace igtl
{

class IGTLCommon_EXPORT TransformMessage: public MessageBase
{

public:

  typedef TransformMessage               Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

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

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  Matrix4x4 matrix;

  unsigned char*  m_Transform;

};


} // namespace igtl

#endif // _igtlTransformMessage_h



