/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

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
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

#include "igtlImageMessage.h"

namespace igtl
{

/// A class to manage meta data of images.
class IGTLCommon_EXPORT ImageMetaElement: public Object
{
public:
  igtlTypeMacro(igtl::ImageMetaElement, igtl::Object);
  igtlNewMacro(igtl::ImageMetaElement);

public:

  /// Sets the image name/description. The string 'name' must not exceed 64 characters.
  int           SetName(const char* name);

  /// Gets the image name/description.
  const char*   GetName()                            { return this->m_Name.c_str(); };

  /// Sets the device name (message name). The string 'devname' must not exceed 20
  /// characters.
  int           SetDeviceName(const char* devname);

  /// Gets the device name (message name).
  const char*   GetDeviceName()                      { return this->m_DeviceName.c_str(); };

  /// Sets the name of the image modality (e.g. CT, MRI). The string 'modality' must not
  /// exceed 32 characters.
  int           SetModality(const char* modality);

  /// Gets the name of the image modality. 
  const char*   GetModality()                        { return this->m_Modality.c_str(); };

  /// Sets the patient name. The string 'patname' must not exceed 64 characters.
  int           SetPatientName(const char* patname);

  /// Gets the patient name.
  const char*   GetPatientName()                     { return this->m_PatientName.c_str(); };

  /// Sets the patient ID. The string 'patid' must not exceed 64 characters.
  int           SetPatientID(const char* patid);

  /// Gets the patient ID.
  const char*   GetPatientID()                       { return this->m_PatientID.c_str(); }

  /// Sets the scan time for the image.
  void          SetTimeStamp(igtl::TimeStamp::Pointer& time);

  /// Gets the scan time for the image.
  void          GetTimeStamp(igtl::TimeStamp::Pointer& time);

  /// Sets the size of the image by an array containing the numbers of voxels in i, j, and k
  /// directions.
  void          SetSize(igtlUint16 size[3]);
  
  /// Sets the size of the image by the numbers of voxels in i, j, and k directions.
  void          SetSize(igtlUint16 si, igtlUint16 sj, igtlUint16 sk);

  /// Gets the size of the image using an array containing the numbers of voxels in i, j, and k
  /// directions.
  void          GetSize(igtlUint16* size);

  /// Gets the size of the image by the numbers of voxels in i, j, and k directions.
  void          GetSize(igtlUint16& si, igtlUint16& sj, igtlUint16& sk);

  /// Sets the scalar type of the image
  void          SetScalarType(igtlUint8 type);

  /// Gets the scalar type of the image.
  igtlUint8     GetScalarType();

protected:
  ImageMetaElement();
  ~ImageMetaElement();

protected:

  /// name / description (<= 64 bytes)
  std::string   m_Name;

  /// device name to query the IMAGE and COLORT
  std::string   m_DeviceName;

  /// modality name (<= 32 bytes)
  std::string   m_Modality;

  /// patient name (<= 64 bytes)
  std::string   m_PatientName;

  /// patient ID (MRN etc.) (<= 64 bytes)
  std::string   m_PatientID;

  /// scan time
  TimeStamp::Pointer m_TimeStamp;

  /// entire image volume size
  igtlUint16    m_Size[3];

  /// scalar type. see scalar_type in IMAGE message
  igtlUint8     m_ScalarType;

};


/// A class for the GET_IMGMETA message type.
class IGTLCommon_EXPORT GetImageMetaMessage: public MessageBase
{
public:
  igtlTypeMacro(igtl::GetImageMetaMessage, igtl::MessageBase);
  igtlNewMacro(igtl::GetImageMetaMessage);

protected:
  GetImageMetaMessage() : MessageBase() { this->m_SendMessageType  = "GET_IMGMETA"; };
  ~GetImageMetaMessage() {};
protected:
  igtlUint64 CalculateContentBufferSize() override { return 0; };
  int  PackContent()  override       { AllocateBuffer(); return 1; };
  int  UnpackContent()   override    { return 1; };
};


/// The IMGMETA message is used to transfer image meta information which are not
/// available in the IMAGE message type, such as patient name, medical record number,
/// modality etc. An IMGMETA message can contain meta data for multiple images.
/// This message type may be used to obtain a list of images available in
/// the remote system, such as image database or commercial image-guided surgery (IGS) system.
class IGTLCommon_EXPORT ImageMetaMessage: public MessageBase
{
public:
  igtlTypeMacro(igtl::ImageMetaMessage, igtl::MessageBase);
  igtlNewMacro(igtl::ImageMetaMessage);

public:

  /// Adds an image meta element to the list. Returns the number of elements after
  /// adding the image meta element.
  int  AddImageMetaElement(ImageMetaElement::Pointer& elem);

  /// Clears the all image meta elements in the list.
  void ClearImageMetaElement();
  
  /// Gets the number of the image meta elements in the list.
  int  GetNumberOfImageMetaElement();

  /// Gets the image meta data specified by the index.
  void GetImageMetaElement(int index, ImageMetaElement::Pointer& elem);


protected:
  ImageMetaMessage();
  ~ImageMetaMessage();
  
protected:

  igtlUint64 CalculateContentBufferSize() override;
  int  PackContent() override;
  int  UnpackContent() override;

  /// A list of pointers to image meta data.
  std::vector<ImageMetaElement::Pointer> m_ImageMetaList;
  
};


} // namespace igtl

#endif // _igtlImageMetaMessage_h
