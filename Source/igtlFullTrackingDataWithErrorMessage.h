/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlFullTrackingDataWithErrorMessage_h
#define __igtlFullTrackingDataWithErrorMessage_h

#include "igtlTrackingDataMessage.h"
#include "igtlMacro.h"

namespace igtl
{

class IGTLCommon_EXPORT TrackingDataWithErrorElement: public TrackingDataElement
{
public:
  typedef TrackingDataWithErrorElement Self;
  typedef TrackingDataElement          Superclass;
  typedef SmartPointer<Self>           Pointer;
  typedef SmartPointer<const Self>     ConstPointer;

  igtlTypeMacro(igtl::TrackingDataWithErrorElement, igtl::TrackingDataElement);
  igtlNewMacro(igtl::TrackingDataWithErrorElement);

public:
  igtlGetConstMacro(Error,  igtlFloat32);
  igtlSetMacro(Error, igtlFloat32);

protected:
  TrackingDataWithErrorElement() : m_Error(0) {}
  ~TrackingDataWithErrorElement() {}

protected:

  /// Some tracking systems (e.g. NDI Certus) give the tracking error.
  igtlFloat32 m_Error;
};


class IGTLCommon_EXPORT FullTrackingDataWithErrorMessage: public TrackingDataMessage
{
public:
  typedef FullTrackingDataWithErrorMessage Self;
  typedef TrackingDataMessage              Superclass;
  typedef SmartPointer<Self>               Pointer;
  typedef SmartPointer<const Self>         ConstPointer;

  igtlTypeMacro(igtl::FullTrackingDataWithErrorMessage, igtl::TrackingDataMessage);
  igtlNewMacro(igtl::FullTrackingDataWithErrorMessage);

public:

  /// Gets the tracking data element specified by 'index'.
  void GetTrackingDataElement(int index, TrackingDataWithErrorElement::Pointer& elem);

protected:
  FullTrackingDataWithErrorMessage();
  ~FullTrackingDataWithErrorMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

};

} // namespace igtl

#endif // __igtlFullTrackingDataWithErrorMessage_h



