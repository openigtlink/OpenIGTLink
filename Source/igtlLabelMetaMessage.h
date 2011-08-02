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

#ifndef __igtlLabelMetaMessage_h
#define __igtlLabelMetaMessage_h

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

class IGTLCommon_EXPORT LabelMetaElement: public Object
{
public:
  typedef LabelMetaElement               Self;
  typedef Object                         Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::LabelMetaElement, igtl::Object);
  igtlNewMacro(igtl::LabelMetaElement);

public:
  int           SetName(const char* name);
  const char*   GetName()                            { return this->m_Name.c_str(); };

  int           SetDeviceName(const char* devname);
  const char*   GetDeviceName()                      { return this->m_DeviceName.c_str(); };

  void          SetLabel(igtlUint8 label)            { this->m_Label = label; };
  igtlUint8     GetLabel()                           { return this->m_Label; };

  void          SetRGBA(igtlUint8 rgba[4]);
  void          SetRGBA(igtlUint8 r, igtlUint8 g, igtlUint8 b, igtlUint8 a);
  void          GetRGBA(igtlUint8* rgba);
  void          GetRGBA(igtlUint8& r, igtlUint8& g, igtlUint8& b, igtlUint8& a);
  
  void          SetSize(igtlUint16 size[3]);
  void          SetSize(igtlUint16 sx, igtlUint16 sy, igtlUint16 sz);
  void          GetSize(igtlUint16* size);
  void          GetSize(igtlUint16& sx, igtlUint16& sy, igtlUint16& sz);
  
  int           SetOwner(const char* owner);
  const char*   GetOwner()                            { return this->m_Owner.c_str(); };

protected:
  LabelMetaElement();
  ~LabelMetaElement();

protected:

  std::string   m_Name;        /* Name / description (< 64 bytes)*/
  std::string   m_DeviceName;  /* Device name to query the IMAGE and COLORT */
  igtlUint8     m_Label;       /* Label */
  igtlUint8     m_RGBA[4];     /* Color in RGBA. default: (0, 0, 0, 0) */
  igtlUint16    m_Size[3];     /* entire image volume size */ 
  std::string   m_Owner;       /* device name of the owner image. (can be empty) */

};


class IGTLCommon_EXPORT GetLabelMetaMessage: public MessageBase
{
public:
  typedef GetLabelMetaMessage            Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::GetLabelMetaMessage, igtl::MessageBase);
  igtlNewMacro(igtl::GetLabelMetaMessage);

protected:
  GetLabelMetaMessage() : MessageBase() { this->m_DefaultBodyType  = "GET_LBMETA"; };
  ~GetLabelMetaMessage() {};
protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };
};


class IGTLCommon_EXPORT LabelMetaMessage: public MessageBase
{
public:
  typedef LabelMetaMessage               Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::LabelMetaMessage, igtl::MessageBase);
  igtlNewMacro(igtl::LabelMetaMessage);

public:
  int  AddLabelMetaElement(LabelMetaElement::Pointer& elem);
  void ClearLabelMetaElement();

  int  GetNumberOfLabelMetaElement();
  void GetLabelMetaElement(int index, LabelMetaElement::Pointer& elem);


protected:
  LabelMetaMessage();
  ~LabelMetaMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  std::vector<LabelMetaElement::Pointer> m_LabelMetaList;
  
};


} // namespace igtl

#endif // _igtlLabelMetaMessage_h



