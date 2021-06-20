/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlVideoMetaMessage_h
#define __igtlVideoMetaMessage_h

#include <vector>
#include <string>

#include "igtlObject.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

#include "igtlVideoMessage.h"

namespace igtl
{

/// A class to manage meta data of images.
class IGTLCommon_EXPORT VideoMetaElement: public Object
{
public:
  igtlTypeMacro(igtl::VideoMetaElement, igtl::Object);
  igtlNewMacro(igtl::VideoMetaElement);

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

  /// Sets the zoom level of the camera (e.g. +4, -4).
  void           SetZoomLevel(igtlInt16 level)       {this->m_ZoomLevel = level;};

  /// Gets the zoom level of the camera (e.g. +4, -4).
  igtlInt16     GetZoomLevel()                        { return this->m_ZoomLevel; };
  
  /// Sets the focal length of the camera (e.g. +50.0mm, +200.0mm).
  void           SetFocalLength(igtlFloat64 length)    {this->m_FocalLength = length;};
  
  /// Gets the focal length of the camera (e.g.+50.0mm, +200.0mm).
  igtlFloat64  GetFocalLength()                        { return this->m_FocalLength; };
  
  /// Sets the patient name. The string 'patname' must not exceed 64 characters.
  int           SetPatientName(const char* patname);

  /// Gets the patient name.
  const char*   GetPatientName()                     { return this->m_PatientName.c_str(); };

  /// Sets the patient ID. The string 'patid' must not exceed 64 characters.
  int           SetPatientID(const char* patid);

  /// Gets the patient ID.
  const char*   GetPatientID()                       { return this->m_PatientID.c_str(); }

  /// Sets the size of the image by the numbers of voxels in i, j, and k directions.
  void          SetSize(igtlUint16 si, igtlUint16 sj, igtlUint16 sk);

  /// Gets the size of the image by the numbers of voxels in i, j, and k directions.
  void          GetSize(igtlUint16& si, igtlUint16& sj, igtlUint16& sk);
  
  /// Sets the origin/orientation matrix.
  void          SetMatrix(Matrix4x4& mat);
  
  /// Gets the origin/orientation matrix.
  void          GetMatrix(Matrix4x4& mat);
  
  /// Sets spacings by spacing values in i, j and k directions.
  void SetSpacing(float si, float sj, float sk);
  
  /// Gets spacings using spacing values in i, j and k directions.
  void GetSpacing(float &si, float &sj, float &sk);
  
  /// Sets the coordinates of the origin by positions along the first (R or L), second (A or P)
  /// and the third (S) axes.
  void SetOrigin(float px, float py, float pz);
  
  /// Gets the coordinates of the origin by positions along the first (R or L), second (A or P)
  /// and the third (S) axes.
  void GetOrigin(float &px, float &py, float &pz);
  
  /// Sets the orientation of the image by the normal vectors for the i, j and k indeces.
  void SetNormals(float t[3], float s[3], float n[3]);
  
  /// Gets the orientation of the image using the normal vectors for the i, j and k indeces.
  void GetNormals(float t[3], float s[3], float n[3]);

  /// Sets the scalar type of the image
  void          SetScalarType(igtlUint8 type);

  /// Gets the scalar type of the image.
  igtlUint8     GetScalarType();

protected:
  VideoMetaElement();
  ~VideoMetaElement();

protected:

  /// name / description (<= 64 bytes)
  std::string   m_Name;

  /// device name to query the IMAGE and COLORT
  std::string   m_DeviceName;
  
  /// patient name (<= 64 bytes)
  std::string   m_PatientName;
  
  /// patient ID (MRN etc.) (<= 64 bytes)
  std::string   m_PatientID;
  
  /// Zoom level of the camera
  igtlInt16     m_ZoomLevel;
  
  /// Focal length of the camera
  igtlFloat64   m_FocalLength;
  
  /// A vector containing the spacings of the voxels in i, j and k directions.
  float  spacing[3];

  /// A matrix representing the origin and the orientation of the image,
  /// if the camera is attached with tracking device.
  /// The matrix is set to identity by default
  Matrix4x4     matrix;

  /// entire image volume size, if the video is from ultrasound
  igtlUint16    m_Size[3];

  /// scalar type. see scalar_type in Video message
  igtlUint8     m_ScalarType;

};


/// A class for the GET_IMGMETA message type.
class IGTLCommon_EXPORT GetVideoMetaMessage: public MessageBase
{
public:
  igtlTypeMacro(igtl::GetVideoMetaMessage, igtl::MessageBase);
  igtlNewMacro(igtl::GetVideoMetaMessage);

protected:
  GetVideoMetaMessage() : MessageBase() { this->m_SendMessageType  = "GET_VMETA"; };
  ~GetVideoMetaMessage() {};
protected:
  igtlUint64 CalculateContentBufferSize() override { return 0; };
  int  PackContent()    override     { AllocateBuffer(); return 1; };
  int  UnpackContent()   override    { return 1; };
};


/// The IMGMETA message is used to transfer image meta information which are not
/// available in the IMAGE message type, such as patient name, medical record number,
/// modality etc. An IMGMETA message can contain meta data for multiple images.
/// This message type may be used to obtain a list of images available in
/// the remote system, such as image database or commercial image-guided surgery (IGS) system.
class IGTLCommon_EXPORT VideoMetaMessage: public MessageBase
{
public:
  igtlTypeMacro(igtl::VideoMetaMessage, igtl::MessageBase);
  igtlNewMacro(igtl::VideoMetaMessage);

public:

  /// Adds an video meta element to the list. Returns the number of elements after
  /// adding the image meta element.
  int  AddVideoMetaElement(VideoMetaElement::Pointer& elem);

  /// Clears the all image meta elements in the list.
  void ClearVideoMetaElement();
  
  /// Gets the number of the image meta elements in the list.
  int  GetNumberOfVideoMetaElement();

  /// Gets the image meta data specified by the index.
  void GetVideoMetaElement(int index, VideoMetaElement::Pointer& elem);


protected:
  VideoMetaMessage();
  ~VideoMetaMessage();
  
protected:

  igtlUint64 CalculateContentBufferSize() override;
  int  PackContent() override;
  int  UnpackContent() override;

  /// A list of pointers to image meta data.
  std::vector<VideoMetaElement::Pointer> m_VideoMetaList;
  
};


} // namespace igtl

#endif // _igtlVideoMetaMessage_h
