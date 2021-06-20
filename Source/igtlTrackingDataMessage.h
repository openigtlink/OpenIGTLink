/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

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
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"


namespace igtl
{

class IGTLCommon_EXPORT TrackingDataElement: public Object
{
public:
  igtlTypeMacro(igtl::TrackingDataElement, igtl::Object);
  igtlNewMacro(igtl::TrackingDataElement);

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

  /// Sets the 4-by-4 transformation matrix.
  void SetMatrix(Matrix4x4& mat);

  /// Gets the 4-by-4 transformation matrix. 
  void GetMatrix(Matrix4x4& mat);

protected:
  TrackingDataElement();
  ~TrackingDataElement();

protected:

  /// Name / description (< 20 bytes
  std::string   m_Name;

  /// Tracking data type (TYPE_TRACKER, TYPE_6D, TYPE_3D, TYPE_5D)
  igtlUint8     m_Type;

  /// Transform matrix
  Matrix4x4     m_Matrix;
};


/// A class for the STT_TDATA message type.
class IGTLCommon_EXPORT StartTrackingDataMessage: public MessageBase
{

public:
  igtlTypeMacro(igtl::StartTrackingDataMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StartTrackingDataMessage);

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
  StartTrackingDataMessage();
  ~StartTrackingDataMessage();

protected:
  igtlUint64 CalculateContentBufferSize() override;
  int  PackContent() override;
  int  UnpackContent() override;

protected:

  /// Minimum time between two frames (ms). Use 0 for as fast as possible.
  igtlInt32     m_Resolution;

  /// Name of the coordinate system.
  std::string   m_CoordinateName;

};


/// A class for the STP_TDATA message type.
class IGTLCommon_EXPORT StopTrackingDataMessage: public MessageBase
{
public:
  igtlTypeMacro(igtl::StopTrackingDataMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StopTrackingDataMessage);

protected:
  StopTrackingDataMessage() : MessageBase() { this->m_SendMessageType  = "STP_TDATA"; };
  ~StopTrackingDataMessage() {};

protected:
  igtlUint64 CalculateContentBufferSize() override { return 0; };
  int  PackContent()    override     { AllocateBuffer(); return 1; };
  int  UnpackContent()   override    { return 1; };

};


/// A class for the RTS_TDATA message type.
class IGTLCommon_EXPORT RTSTrackingDataMessage: public MessageBase
{
public:
  /// Status type
  enum {
    STATUS_SUCCESS = 0,
    STATUS_ERROR = 1
  };

  igtlTypeMacro(igtl::RTSTrackingDataMessage, igtl::MessageBase);
  igtlNewMacro(igtl::RTSTrackingDataMessage);

  /// Sets the status. 'status' must be either STATUS_SUCCESS or STATUS_ERROR.
  void          SetStatus(igtlUint8 status){ this->m_Status = status; }

  /// Gets the status. The function returns either STATUS_SUCCESS or STATUS_ERROR.
  igtlUint8     GetStatus()                { return this->m_Status; };

protected:
  RTSTrackingDataMessage() : MessageBase(), m_Status(0) { this->m_SendMessageType  = "RTS_TDATA"; };
  ~RTSTrackingDataMessage() {};

  /// A variable to store the status.
  igtlUint8 m_Status;

protected:
  igtlUint64 CalculateContentBufferSize() override;
  int  PackContent() override;
  int  UnpackContent() override;

};


/// The TDATA message type is intended for transferring 3D positions of surgical tools,
/// markers etc. Those positions are often measured by optical, electromagnetic or other
/// type of 3D position sensor continuously and transferred as series of messages.
/// Since it is important for software that receives TDATA to control data flow,
/// STT_TDATA query data type has interval field to control the frame rate of consecutive messages.
class IGTLCommon_EXPORT TrackingDataMessage: public MessageBase
{
public:
  igtlTypeMacro(igtl::TrackingDataMessage, igtl::MessageBase);
  igtlNewMacro(igtl::TrackingDataMessage);

public:

  /// Adds tracking data element.
  int  AddTrackingDataElement(TrackingDataElement::Pointer& elem);

  /// Clears the all tracking data element in the list.
  void ClearTrackingDataElements();

  /// Gets the number of tracking data elements in the list.
  int  GetNumberOfTrackingDataElements();

  inline int  GetNumberOfTrackingDataElement() { return GetNumberOfTrackingDataElements(); }; // will be removed.

  /// Gets the tracking data element specified by 'index'.
  void GetTrackingDataElement(int index, TrackingDataElement::Pointer& elem);


protected:
  TrackingDataMessage();
  ~TrackingDataMessage();
  
protected:

  igtlUint64 CalculateContentBufferSize() override;
  int  PackContent() override;
  int  UnpackContent() override;

  /// The list of tracking data elements.  
  std::vector<TrackingDataElement::Pointer> m_TrackingDataList;
  
};


} // namespace igtl

#endif // _igtlTrackingDataMessage_h
