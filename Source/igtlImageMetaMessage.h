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

#ifndef __igtlImageMetaMessage_h
#define __igtlImageMetaMessage_h

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

class IGTLCommon_EXPORT ImageMetaElement: public Object
{
public:
  typedef ImageMetaElement               Self;
  typedef Object                         Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::ImageMetaElement, igtl::Object);
  igtlNewMacro(igtl::ImageMetaElement);

public:
  int           SetName(const char* name);
  const char*   GetName()                            { return this->m_Name.c_str(); };

  int           SetDeviceName(const char* devname);
  const char*   GetDeviceName()                      { return this->m_DeviceName.c_str(); };

  int           SetModality(const char* modality);
  const char*   GetModality()                        { return this->m_Modality.c_str(); };

  int           SetPatientName(const char* patname);
  const char*   GetPatientName()                     { return this->m_PatientName.c_str(); };

  int           SetPatientID(const char* patid);
  const char*   GetPatientID()                       { return this->m_PatientID.c_str(); }

  void          SetTimeStamp(igtl::TimeStamp::Pointer& time);
  void          GetTimeStamp(igtl::TimeStamp::Pointer& time);

  void          SetSize(igtlUint16 size[3]);
  void          SetSize(igtlUint16 sx, igtlUint16 sy, igtlUint16 sz);
  void          GetSize(igtlUint16* size);
  void          GetSize(igtlUint16& sx, igtlUint16& sy, igtlUint16& sz);
  
  void          SetScalarType(igtlUint8 type);
  igtlUint8     GetScalarType();

protected:
  ImageMetaElement();
  ~ImageMetaElement();

protected:

  std::string   m_Name;        /* name / description (< 64 bytes)*/
  std::string   m_DeviceName;  /* device name to query the IMAGE and COLORT */
  std::string   m_Modality;    /* modality name (< 32 bytes) */
  std::string   m_PatientName; /* patient name (< 64 bytes) */ 
  std::string   m_PatientID;   /* patient ID (MRN etc.) (< 64 bytes) */  
  TimeStamp::Pointer m_TimeStamp;   /* scan time */
  igtlUint16    m_Size[3];     /* entire image volume size */ 
  igtlUint8     m_ScalarType;  /* scalar type. see scalar_type in IMAGE message */

};


class IGTLCommon_EXPORT GetImageMetaMessage: public MessageBase
{
public:
  typedef GetImageMetaMessage            Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::GetImageMetaMessage, igtl::MessageBase);
  igtlNewMacro(igtl::GetImageMetaMessage);

protected:
  GetImageMetaMessage() : MessageBase() { this->m_DefaultBodyType  = "GET_IMGMETA"; };
  ~GetImageMetaMessage() {};
protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };
};


class IGTLCommon_EXPORT ImageMetaMessage: public MessageBase
{
public:
  typedef ImageMetaMessage               Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::ImageMetaMessage, igtl::MessageBase);
  igtlNewMacro(igtl::ImageMetaMessage);

public:
  int  AddImageMetaElement(ImageMetaElement::Pointer& elem);
  void ClearImageMetaElement();

  int  GetNumberOfImageMetaElement();
  void GetImageMetaElement(int index, ImageMetaElement::Pointer& elem);


protected:
  ImageMetaMessage();
  ~ImageMetaMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  std::vector<ImageMetaElement::Pointer> m_ImageMetaList;
  
};


} // namespace igtl

#endif // _igtlImageMetaMessage_h



