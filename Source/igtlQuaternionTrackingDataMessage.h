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

#ifndef __igtlQuaternionTrackingDataMessage_h
#define __igtlQuaternionTrackingDataMessage_h

#include <vector>
#include <string>

#include "igtlObject.h"
//#include "igtlMacros.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"


namespace igtl
{

class IGTLCommon_EXPORT QuaternionTrackingDataElement: public Object
{
public:
  typedef QuaternionTrackingDataElement  Self;
  typedef Object                         Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::QuaternionTrackingDataElement, igtl::Object);
  igtlNewMacro(igtl::QuaternionTrackingDataElement);

  // Tracking data type
  enum {
    TYPE_TRACKER  = 1,  /* Tracker */
    TYPE_6D       = 2,  /* 6D instrument (regular instrument) */
    TYPE_3D       = 3,  /* 3D instrument (only tip of the instrument defined) */
    TYPE_5D       = 4,  /* 5D instrument (tip and handle are defined,
                           but not the normal vector) */
  };

public:
  int           SetName(const char* name);
  const char*   GetName()                            { return this->m_Name.c_str(); };

  int           SetType(igtlUint8 type);
  igtlUint8     GetType()                            { return this->m_Type; };

  void SetPosition(float p[3]);
  void GetPosition(float p[3]);

  void SetPosition(float px, float py, float pz);
  void GetPosition(float* px, float* py, float* pz);

  void SetQuaternion(float q[4]);
  void GetQuaternion(float q[4]);

  void SetQuaternion(float qx, float qy, float qz, float w);
  void GetQuaternion(float* qx, float* qy, float* qz, float* w);

protected:
  QuaternionTrackingDataElement();
  ~QuaternionTrackingDataElement();

protected:

  std::string   m_Name;          /* Name / description (< 20 bytes) */
  igtlUint8     m_Type;          /* Tracking data type (TYPE_TRACKER, TYPE_6D, TYPE_3D, TYPE_5D) */
  igtlFloat32   m_position[3];   /* position (x, y, z) */
  igtlFloat32   m_quaternion[4]; /* orientation as quaternion (qx, qy, qz, w) */
};


class IGTLCommon_EXPORT StartQuaternionTrackingDataMessage: public MessageBase
{

public:
  typedef StartQuaternionTrackingDataMessage  Self;
  typedef MessageBase                         Superclass;
  typedef SmartPointer<Self>                  Pointer;
  typedef SmartPointer<const Self>            ConstPointer;

  igtlTypeMacro(igtl::StartQuaternionTrackingDataMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StartQuaternionTrackingDataMessage);

public:
  void         SetResolution(igtlInt32 res)  { this->m_Resolution = res; }; // ms
  igtlInt32    GetResolution()               { return this->m_Resolution; };

  int          SetCoordinateName(const char* name);
  const char*  GetCoordinateName()            { return this->m_CoordinateName.c_str(); };

protected:
  StartQuaternionTrackingDataMessage();
  ~StartQuaternionTrackingDataMessage();

protected:
  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

protected:
  igtlInt32     m_Resolution;     /* Minimum time between two frames (ms). Use 0 for as fast as possible. */
  std::string   m_CoordinateName; /* Name of the coordinate system */

};


class IGTLCommon_EXPORT StopQuaternionTrackingDataMessage: public MessageBase
{
public:
  typedef StopQuaternionTrackingDataMessage  Self;
  typedef MessageBase                        Superclass;
  typedef SmartPointer<Self>                 Pointer;
  typedef SmartPointer<const Self>           ConstPointer;

  igtlTypeMacro(igtl::StopQuaternionTrackingDataMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StopQuaternionTrackingDataMessage);

protected:
  StopQuaternionTrackingDataMessage() : MessageBase() { this->m_DefaultBodyType  = "STP_QTDATA"; };
  ~StopQuaternionTrackingDataMessage() {};

protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };

};


class IGTLCommon_EXPORT RTSQuaternionTrackingDataMessage: public MessageBase
{
public:
  typedef RTSQuaternionTrackingDataMessage  Self;
  typedef MessageBase                       Superclass;
  typedef SmartPointer<Self>                Pointer;
  typedef SmartPointer<const Self>          ConstPointer;

  // Status type
  enum {
    STATUS_SUCCESS = 0,
    STATUS_ERROR = 1
  };


  igtlTypeMacro(igtl::RTSQuaternionTrackingDataMessage, igtl::MessageBase);
  igtlNewMacro(igtl::RTSQuaternionTrackingDataMessage);

  void          SetStatus(igtlUint8 status){ this->m_Status = status; }
  igtlUint8     GetStatus()                { return this->m_Status; };

protected:
  RTSQuaternionTrackingDataMessage() : MessageBase(), m_Status(0) { this->m_DefaultBodyType  = "RTS_QTDATA"; };
  ~RTSQuaternionTrackingDataMessage() {};

  igtlUint8 m_Status;

protected:
  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

};


class IGTLCommon_EXPORT QuaternionTrackingDataMessage: public MessageBase
{
public:
  typedef QuaternionTrackingDataMessage  Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::QuaternionTrackingDataMessage, igtl::MessageBase);
  igtlNewMacro(igtl::QuaternionTrackingDataMessage);

public:
  int  AddQuaternionTrackingDataElement(QuaternionTrackingDataElement::Pointer& elem);
  void ClearQuaternionTrackingDataElements();

  int  GetNumberOfQuaternionTrackingDataElements();
  void GetQuaternionTrackingDataElement(int index, QuaternionTrackingDataElement::Pointer& elem);


protected:
  QuaternionTrackingDataMessage();
  ~QuaternionTrackingDataMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  std::vector<QuaternionTrackingDataElement::Pointer> m_QuaternionTrackingDataList;
  
};


} // namespace igtl

#endif // _igtlQuaternionTrackingDataMessage_h



