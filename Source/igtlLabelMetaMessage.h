/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlLabelMetaMessage_h
#define __igtlLabelMetaMessage_h

#include <vector>
#include <string>

#include "igtlObject.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

#include "igtlImageMessage.h"

namespace igtl
{

class IGTLCommon_EXPORT LabelMetaElement: public Object
{
public:
  igtlTypeMacro(igtl::LabelMetaElement, igtl::Object);
  igtlNewMacro(igtl::LabelMetaElement);

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

  /// Sets the label of the structure.
  void          SetLabel(igtlUint8 label)            { this->m_Label = label; };

  /// Gets the label of the structure.
  igtlUint8     GetLabel()                           { return this->m_Label; };

  /// Sets the color of the structure by an array representing RGBA.
  void          SetRGBA(igtlUint8 rgba[4]);
  
  /// Sets the color of the structure by R, G, B, and A values.
  void          SetRGBA(igtlUint8 r, igtlUint8 g, igtlUint8 b, igtlUint8 a);

  /// Gets the color of the structure by an array representing RGBA.
  void          GetRGBA(igtlUint8* rgba);

  /// Gets the color of the structure by R, G, B, and A values.
  void          GetRGBA(igtlUint8& r, igtlUint8& g, igtlUint8& b, igtlUint8& a);
  
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

  /// Sets the name of the image that owns this label map.
  int           SetOwner(const char* owner);

  /// Gets the name of the image that owns this label map.
  const char*   GetOwner()                            { return this->m_Owner.c_str(); };

protected:
  LabelMetaElement();
  ~LabelMetaElement();

protected:

  /// Name / description (<= 64 bytes)
  std::string   m_Name;

  /// Device name to query the IMAGE and COLORT
  std::string   m_DeviceName;

  /// Label
  igtlUint8     m_Label;

  /// Color in RGBA. default: (0, 0, 0, 0)
  igtlUint8     m_RGBA[4];

  /// entire image volume size
  igtlUint16    m_Size[3];

  /// device name of the owner image. (can be empty)
  std::string   m_Owner;

};


/// A class for the GET_LBMETA message type.
class IGTLCommon_EXPORT GetLabelMetaMessage: public MessageBase
{
public:
  igtlTypeMacro(igtl::GetLabelMetaMessage, igtl::MessageBase);
  igtlNewMacro(igtl::GetLabelMetaMessage);

protected:
  GetLabelMetaMessage() : MessageBase() { this->m_SendMessageType  = "GET_LBMETA"; };
  ~GetLabelMetaMessage() {};
protected:
  virtual int  CalculateContentBufferSize() { return 0; };
  virtual int  PackContent()        { AllocateBuffer(); return 1; };
  virtual int  UnpackContent()      { return 1; };
};


/// The LBMETA is used to transfer meta information for label maps, which are not available
/// in the IMAGE message type. To retrieve voxel objects or a label map, GET_IMAGE / IMAGE
/// can be used. But the client should be able to get a list of available structures.
class IGTLCommon_EXPORT LabelMetaMessage: public MessageBase
{
public:
  igtlTypeMacro(igtl::LabelMetaMessage, igtl::MessageBase);
  igtlNewMacro(igtl::LabelMetaMessage);

public:

  /// Adds an label meta element to the list.
  int  AddLabelMetaElement(LabelMetaElement::Pointer& elem);

  /// Clears the all label meta elements in the list.
  void ClearLabelMetaElement();

  /// Gets the number of the label meta elements in the list.
  int  GetNumberOfLabelMetaElement();

  /// Gets the label meta element specified by the index.
  void GetLabelMetaElement(int index, LabelMetaElement::Pointer& elem);


protected:
  LabelMetaMessage();
  ~LabelMetaMessage();
  
protected:

  virtual int  CalculateContentBufferSize();
  virtual int  PackContent();
  virtual int  UnpackContent();
  
  // A list of pointers to label meta data.
  std::vector<LabelMetaElement::Pointer> m_LabelMetaList;
  
};


} // namespace igtl

#endif // _igtlLabelMetaMessage_h