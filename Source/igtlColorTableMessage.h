/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlColorTableMessage_h
#define __igtlColorTableMessage_h

#include "igtlObject.h"
//#include "igtlMacros.h"
#include "igtlMacro.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"

namespace igtl
{

/// A class for the GET_COLORT message type.
class IGTLCommon_EXPORT GetColorTableMessage: public MessageBase
{
public:
  typedef GetColorTableMessage           Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::GetColorTableMessage, igtl::MessageBase);
  igtlNewMacro(igtl::GetColorTableMessage);

protected:
  GetColorTableMessage() : MessageBase() { this->m_DefaultBodyType  = "GET_COLORT"; };
  ~GetColorTableMessage() {};

protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };
};


/// A class for the COLORT message type.
class IGTLCommon_EXPORT ColorTableMessage: public MessageBase
{
public:
  typedef ColorTableMessage         Self;
  typedef MessageBase               Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::ColorTableMessage, igtl::MessageBase)
  igtlNewMacro(igtl::ColorTableMessage);

public:

  enum {
    INDEX_UINT8    = 3,
    INDEX_UINT16   = 5,
    MAP_UINT8      = 3,
    MAP_UINT16     = 5,
    MAP_RGB        = 19,
  };

public:

  // ColorTable index type
  void SetIndexType(int t)     { indexType = t; };
  void SetIndexTypeToUint8()   { indexType = INDEX_UINT8; };
  void SetIndexTypeToUint16()  { indexType = INDEX_UINT16; };
  int  GetIndexType()          { return indexType; };

  // Map scalar type
  void SetMapType(int t)       { mapType = t; };
  void SetMapTypeToUint8()     { mapType = MAP_UINT8; };
  void SetMapTypeToUint16()    { mapType = MAP_UINT16; };
  int  GetMapType()            { return mapType; };

  // Should returned value be 64-bit integer?
  int   GetColorTableSize();
  void  AllocateTable();
  void* GetTablePointer();

protected:
  ColorTableMessage();
  ~ColorTableMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  int    indexType;
  int    mapType;

  unsigned char*  m_ColorTableHeader;
  unsigned char*  m_ColorTable;

};


} // namespace igtl

#endif // _igtlColorTableMessage_h


