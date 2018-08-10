/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlColorTableMessage_h
#define __igtlColorTableMessage_h

#include "igtlObject.h"
#include "igtlMacro.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"

namespace igtl
{

/// A class for the GET_COLORT message type.
class IGTLCommon_EXPORT GetColorTableMessage: public MessageBase
{
public:
  igtlTypeMacro(igtl::GetColorTableMessage, igtl::MessageBase);
  igtlNewMacro(igtl::GetColorTableMessage);

protected:
  GetColorTableMessage() : MessageBase() { this->m_SendMessageType  = "GET_COLORT"; };
  ~GetColorTableMessage() {};

protected:
  virtual int  CalculateContentBufferSize() { return 0; };
  virtual int  PackContent()        { AllocateBuffer(); return 1; };
  virtual int  UnpackContent()      { return 1; };
};


/// A class for the COLORT message type.
class IGTLCommon_EXPORT ColorTableMessage: public MessageBase
{
public:
  igtlTypeMacro(igtl::ColorTableMessage, igtl::MessageBase)
  igtlNewMacro(igtl::ColorTableMessage);

public:

  /// Index value types (UINT8 or UINT16) and map types (UINT8, UINT16 and RGB(UINT8x3))
  enum {
    INDEX_UINT8    = 3,
    INDEX_UINT16   = 5,
    MAP_UINT8      = 3,
    MAP_UINT16     = 5,
    MAP_RGB        = 19,
  };

public:

  /// Sets the index type for the color type
  void SetIndexType(int t)     { indexType = t; };

  /// Sets the index type for the color type to 8-bit unsigned integer.
  void SetIndexTypeToUint8()   { indexType = INDEX_UINT8; };

  /// Sets the index type for the color type to 16-bit unsigned integer.
  void SetIndexTypeToUint16()  { indexType = INDEX_UINT16; };

  /// Gets the index type. Returns either INDEX_UINT8 or INDEX_UINT16.
  int  GetIndexType()          { return indexType; };

  /// Sets the scalar type of the map.
  void SetMapType(int t)       { mapType = t; };

  /// Sets the scalar type of the map to 8-bit unsigned integer.
  void SetMapTypeToUint8()     { mapType = MAP_UINT8; };

  /// Sets the scalar type of the map to 16-bit unsigned integer.
  void SetMapTypeToUint16()    { mapType = MAP_UINT16; };
  
  /// Gets the type of the map.
  int  GetMapType()            { return mapType; };
  
  /// Gets the size of the color table.
  int   GetColorTableSize();

  /// Allocates a memory area for the color table.
  void  AllocateTable();

  /// Returns a pointer to the color table.
  void* GetTablePointer();

protected:
  ColorTableMessage();
  ~ColorTableMessage();
  
protected:

  virtual int  CalculateContentBufferSize();
  virtual int  PackContent();
  virtual int  UnpackContent();
  
  /// A variable to store the index type. Either INDEX_UINT8 or INDEX_UINT16.
  int indexType;

  /// A variable to store the type of the map. Either MAP_UINT8, MAP_UINT16 or MAP_RGB.
  int mapType;

  /// A pointer to the header for the color table.
  unsigned char*  m_ColorTableHeader;

  /// A pointer to the color table data.
  unsigned char*  m_ColorTable;

};


} // namespace igtl

#endif // _igtlColorTableMessage_h