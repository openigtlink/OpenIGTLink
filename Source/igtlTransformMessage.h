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

  void SetPosition(float p[3]);
  void GetPosition(float p[3]);

  void SetPosition(float px, float py, float pz);
  void GetPosition(float* px, float* py, float* pz);

  void SetNormals(float o[3][3]);
  void GetNormals(float o[3][3]);

  void SetNormals(float t[3], float s[3], float n[3]);
  void GetNormals(float t[3], float s[3], float n[3]);

  void SetMatrix(Matrix4x4& mat);
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



