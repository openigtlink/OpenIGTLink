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

#ifndef __igtlTrackingDataMessage_h
#define __igtlTrackingDataMessage_h

#include <vector>
#include <string>

#include "igtlObject.h"
//#include "igtlMacros.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"


namespace igtl
{

class IGTLCommon_EXPORT TrackingDataElement: public Object
{
public:
  typedef TrackingDataElement               Self;
  typedef Object                         Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::TrackingDataElement, igtl::Object);
  igtlNewMacro(igtl::TrackingDataElement);

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

  void SetMatrix(Matrix4x4& mat);
  void GetMatrix(Matrix4x4& mat);

protected:
  TrackingDataElement();
  ~TrackingDataElement();

protected:

  std::string   m_Name;          /* Name / description (< 20 bytes) */
  igtlUint8     m_Type;          /* Tracking data type (TYPE_TRACKER, TYPE_6D, TYPE_3D, TYPE_5D) */
  Matrix4x4     m_Matrix;        /* Transform matrix */
};


class IGTLCommon_EXPORT StartTrackingDataMessage: public MessageBase
{

public:
  typedef StartTrackingDataMessage       Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::StartTrackingDataMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StartTrackingDataMessage);

public:
  void         SetResolution(igtlInt32 res)  { this->m_Resolution = res; }; // ms
  igtlInt32    GetResolution()               { return this->m_Resolution; };

  int          SetCoordinateName(const char* name);
  const char*  GetCoordinateName()            { return this->m_CoordinateName.c_str(); };

protected:
  StartTrackingDataMessage();
  ~StartTrackingDataMessage();

protected:
  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

protected:
  igtlInt32     m_Resolution;     /* Minimum time between two frames (ms). Use 0 for as fast as possible. */
  std::string   m_CoordinateName; /* Name of the coordinate system */

};


class IGTLCommon_EXPORT StopTrackingDataMessage: public MessageBase
{
public:
  typedef StopTrackingDataMessage        Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::StopTrackingDataMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StopTrackingDataMessage);

protected:
  StopTrackingDataMessage() : MessageBase() { this->m_DefaultBodyType  = "STP_TDATA"; };
  ~StopTrackingDataMessage() {};

protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };

};


class IGTLCommon_EXPORT RTSTrackingDataMessage: public MessageBase
{
public:
  typedef RTSTrackingDataMessage         Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  // Status type
  enum {
    STATUS_SUCCESS = 0,
    STATUS_ERROR = 1
  };


  igtlTypeMacro(igtl::RTSTrackingDataMessage, igtl::MessageBase);
  igtlNewMacro(igtl::RTSTrackingDataMessage);

  void          SetStatus(igtlUint8 status){ this->m_Status = status; }
  igtlUint8     GetStatus()                { return this->m_Status; };

protected:
  RTSTrackingDataMessage() : MessageBase(), m_Status(0) { this->m_DefaultBodyType  = "RTS_TDATA"; };
  ~RTSTrackingDataMessage() {};

  igtlUint8 m_Status;

protected:
  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

};



class IGTLCommon_EXPORT TrackingDataMessage: public MessageBase
{
public:
  typedef TrackingDataMessage            Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::TrackingDataMessage, igtl::MessageBase);
  igtlNewMacro(igtl::TrackingDataMessage);

public:
  int  AddTrackingDataElement(TrackingDataElement::Pointer& elem);
  void ClearTrackingDataElements();

  int  GetNumberOfTrackingDataElements();
  inline int  GetNumberOfTrackingDataElement() { return GetNumberOfTrackingDataElements(); }; // will be removed.
  void GetTrackingDataElement(int index, TrackingDataElement::Pointer& elem);


protected:
  TrackingDataMessage();
  ~TrackingDataMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  std::vector<TrackingDataElement::Pointer> m_TrackingDataList;
  
};


} // namespace igtl

#endif // _igtlTrackingDataMessage_h



