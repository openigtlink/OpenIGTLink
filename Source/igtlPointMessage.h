/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink2_beta/Source/igtlImgmetaMessage.h $
  Language:  C++
  Date:      $Date: 2009-12-16 23:58:02 -0500 (Wed, 16 Dec 2009) $
  Version:   $Revision: 5466 $

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
//#include "igtlMacros.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

#include "igtlImageMessage.h"

namespace igtl
{

class IGTLCommon_EXPORT PointElement: public Object
{
public:
  typedef PointElement                   Self;
  typedef Object                         Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::PointElement, igtl::Object);
  igtlNewMacro(igtl::PointElement);

public:
  int           SetName(const char* name);
  const char*   GetName()                            { return this->m_Name.c_str(); };

  int           SetGroupName(const char* grpname);
  const char*   GetGroupName()                       { return this->m_GroupName.c_str(); };

  void          SetRGBA(igtlUint8 rgba[4]);
  void          SetRGBA(igtlUint8 r, igtlUint8 g, igtlUint8 b, igtlUint8 a);
  void          GetRGBA(igtlUint8* rgba);
  void          GetRGBA(igtlUint8& r, igtlUint8& g, igtlUint8& b, igtlUint8& a);

  void          SetPosition(igtlFloat32 position[3]);
  void          SetPosition(igtlFloat32 x, igtlFloat32 y, igtlFloat32 z);
  void          GetPosition(igtlFloat32* position);
  void          GetPosition(igtlFloat32& x, igtlFloat32& y, igtlFloat32& z);

  void          SetRadius(igtlFloat32 radius)        { this->m_Radius = radius; };
  igtlFloat32   GetRadius()                          { return this->m_Radius; };

  int           SetOwner(const char* owner);
  const char*   GetOwner()                           { return this->m_Owner.c_str(); };

protected:
  PointElement();
  ~PointElement();

protected:

  std::string   m_Name;        /* name / description (< 64 bytes)*/
  std::string   m_GroupName;   /* Can be "Labeled Point", "Landmark", Fiducial", ... */
  igtlUint8     m_RGBA[4];     /* Color in R/G/B/A */
  igtlFloat32   m_Position[3]; /* Position */ 
  igtlFloat32   m_Radius;      /* Radius of the point. Can be 0. */
  std::string   m_Owner;       /* Device name of the ower image */
};


class IGTLCommon_EXPORT GetPointMessage: public MessageBase
{
public:
  typedef GetPointMessage            Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::GetPointMessage, igtl::MessageBase);
  igtlNewMacro(igtl::GetPointMessage);

protected:
  GetPointMessage() : MessageBase() { this->m_DefaultBodyType  = "GET_POINT"; };
  ~GetPointMessage() {};
protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };
};


class IGTLCommon_EXPORT PointMessage: public MessageBase
{
public:
  typedef PointMessage               Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::PointMessage, igtl::MessageBase);
  igtlNewMacro(igtl::PointMessage);

public:
  int  AddPointElement(PointElement::Pointer& elem);
  void ClearPointElement();

  int  GetNumberOfPointElement();
  void GetPointElement(int index, PointElement::Pointer& elem);


protected:
  PointMessage();
  ~PointMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  std::vector<PointElement::Pointer> m_PointList;
  
};


} // namespace igtl

#endif // _igtlPointMessage_h



