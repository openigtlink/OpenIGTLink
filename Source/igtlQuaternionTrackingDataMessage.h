/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

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
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"


namespace igtl
{

/// A class to manage tracking data as a quaternion used in the QTDATA message type.
/// A QuaternionTrackingDataElement class instance holds tracking data for 1 tracking device.
class IGTLCommon_EXPORT QuaternionTrackingDataElement: public Object
{
public:
  typedef QuaternionTrackingDataElement  Self;
  typedef Object                         Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::QuaternionTrackingDataElement, igtl::Object);
  igtlNewMacro(igtl::QuaternionTrackingDataElement);

  /// Tracking data type. 
  ///  TYPE_TRACKER:  Tracker
  ///  TYPE_6D:       6D instrument: (regular instrument)
  ///  TYPE_3D:       3D instrument (only tip of the instrument defined)
  ///  TYPE_5D:       5D instrument (tip and handle are defined, but not the normal vector)
  enum {
    TYPE_TRACKER  = 1,
    TYPE_6D       = 2,
    TYPE_3D       = 3,
    TYPE_5D       = 4,
  };

public:

  /// Sets the name of the instrument/tracker.
  int           SetName(const char* name);

  /// Gets the name of the instrument/tracker.
  const char*   GetName()                            { return this->m_Name.c_str(); };

  /// Sets the type of the instrument/tracker.
  int           SetType(igtlUint8 type);

  /// Gets the type of the instrument/tracker.
  igtlUint8     GetType()                            { return this->m_Type; };

  /// Sets the position by 3-element array of x, y, and z coordinates.
  void SetPosition(float p[3]);

  /// Gets the position. The function substitutes 3-element array of x, y and z coordinates in 'p'.
  void GetPosition(float p[3]);

  /// Sets the position by x, y, and z coordinates.
  void SetPosition(float px, float py, float pz);

  /// Gets the position. The function substitutes the xyz coordinates in 'px', 'py', and 'pz'.
  void GetPosition(float* px, float* py, float* pz);

  /// Sets the quaternion by 4-element array.
  void SetQuaternion(float q[4]);

  /// Gets the quaternion. The function substitutes the array of elements of the quaternion in 'q'.
  void GetQuaternion(float q[4]);

  /// Sets the quaternion by elements of the quaternion (x, y, z and w).
  void SetQuaternion(float qx, float qy, float qz, float w);

  /// Gets the quaternion. The function substitutes the elements of the quaternion in 'qx', 'qy', 'qz' and 'qw'.
  void GetQuaternion(float* qx, float* qy, float* qz, float* w);

protected:
  QuaternionTrackingDataElement();
  ~QuaternionTrackingDataElement();

protected:

  /// Name / description (< 20 bytes)
  std::string   m_Name;

  /// Tracking data type (TYPE_TRACKER, TYPE_6D, TYPE_3D, TYPE_5D)
  igtlUint8     m_Type;

  /// position (x, y, z)
  igtlFloat32   m_position[3];

  /// orientation as quaternion (qx, qy, qz, w)
  igtlFloat32   m_quaternion[4];
};


/// A class for the STT_QTDATA message type.
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

  /// Sets the time resolution for streaming of QTDATA messages
  void         SetResolution(igtlInt32 res)  { this->m_Resolution = res; }; // ms

  /// Gets the time resolution for streaming of QTDATA messages
  igtlInt32    GetResolution()               { return this->m_Resolution; };

  /// Sets the name of the coordinate system. The name must be defined by the user.
  int          SetCoordinateName(const char* name);

  /// Gets the name of the coordinate system.
  const char*  GetCoordinateName()            { return this->m_CoordinateName.c_str(); };

protected:
  StartQuaternionTrackingDataMessage();
  ~StartQuaternionTrackingDataMessage();

protected:
  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

protected:

  /// Minimum time between two frames (ms). Use 0 for as fast as possible.
  igtlInt32     m_Resolution;

  /// Name of the coordinate system
  std::string   m_CoordinateName;

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


/// A class for the RTS_QTDATA message type.
class IGTLCommon_EXPORT RTSQuaternionTrackingDataMessage: public MessageBase
{
public:
  typedef RTSQuaternionTrackingDataMessage  Self;
  typedef MessageBase                       Superclass;
  typedef SmartPointer<Self>                Pointer;
  typedef SmartPointer<const Self>          ConstPointer;

  /// Status types
  enum {
    STATUS_SUCCESS = 0,
    STATUS_ERROR = 1
  };

  igtlTypeMacro(igtl::RTSQuaternionTrackingDataMessage, igtl::MessageBase);
  igtlNewMacro(igtl::RTSQuaternionTrackingDataMessage);

  /// Sets the status. 'status' must be either STATUS_SUCCESS or STATUS_ERROR.
  void          SetStatus(igtlUint8 status){ this->m_Status = status; }

  /// Gets the status. The function returns either STATUS_SUCCESS or STATUS_ERROR.
  igtlUint8     GetStatus()                { return this->m_Status; };

protected:
  RTSQuaternionTrackingDataMessage() : MessageBase(), m_Status(0) { this->m_DefaultBodyType  = "RTS_QTDATA"; };
  ~RTSQuaternionTrackingDataMessage() {};

  /// A variable to store the status.
  igtlUint8 m_Status;

protected:
  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

};


/// The QTDATA message type is intended for transferring 3D positions of surgical tools,
/// markers etc. Its role is almost identical to TDATA, except that QTDATA describes
/// orientation by using quaternion.
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

  /// Adds tracking data element.
  int  AddQuaternionTrackingDataElement(QuaternionTrackingDataElement::Pointer& elem);

  /// Clears the all tracking data element in the list.
  void ClearQuaternionTrackingDataElements();

  /// Gets the number of tracking data elements in the list.
  int  GetNumberOfQuaternionTrackingDataElements();

  /// Gets the tracking data element specified by 'index'.
  void GetQuaternionTrackingDataElement(int index, QuaternionTrackingDataElement::Pointer& elem);


protected:
  QuaternionTrackingDataMessage();
  ~QuaternionTrackingDataMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  /// The list of trakcing data elements.
  std::vector<QuaternionTrackingDataElement::Pointer> m_QuaternionTrackingDataList;
  
};


} // namespace igtl

#endif // _igtlQuaternionTrackingDataMessage_h



