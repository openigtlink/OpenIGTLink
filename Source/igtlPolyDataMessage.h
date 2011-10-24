/*=========================================================================

  Program:   The OpenIGTLink Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink2_beta/Source/igtlPolyDataMessage.h $
  Language:  C++
  Date:      $Date: 2009-12-16 23:58:02 -0500 (Wed, 16 Dec 2009) $
  Version:   $Revision: 5466 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlPolyDataMessage_h
#define __igtlPolyDataMessage_h

#include <string>

#include "igtlObject.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

#define IGTL_STRING_MESSAGE_DEFAULT_ENCODING 3 /* Default encoding -- ANSI-X3.5-1968 */

namespace igtl
{

class IGTLCommon_EXPORT PolyDataMessage: public MessageBase
{
public:
  enum {
    TYPE_INT8     = 2,
    TYPE_UINT8    = 3,
    TYPE_INT16    = 4,
    TYPE_UINT16   = 5,
    TYPE_INT32    = 6,
    TYPE_UINT32   = 7,
    TYPE_FLOAT32  = 10,
    TYPE_FLOAT64  = 11,
    TYPE_COMPLEX  = 13,
  };

public:
  typedef PolyDataMessage                 Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::PolyDataMessage, igtl::MessageBase);
  igtlNewMacro(igtl::PolyDataMessage);

public:

  int         SetArray(int type, ArrayBase * a);
  ArrayBase * GetArray();
  int         GetType();

protected:
  PolyDataMessage();
  ~PolyDataMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

  // Parameters in POLYDATA header
  igtl_uint32  m_NPoints;             // Number of points
  igtl_uint32  m_NVertices;           // Number of vertices
  igtl_uint32  m_SizeVertices;        // Size of vertice data (bytes)
  igtl_uint32  m_NLines;              // Number of lines
  igtl_uint32  m_SizeLines;           // Size of line data (bytes)
  igtl_uint32  m_NPolygons;           // Number of polygons
  igtl_uint32  m_SizePolygons;        // Size of polygon data (bytes)
  igtl_uint32  m_NTriangleStrips;     // Number of triangle strips
  igtl_uint32  m_SizeTriangleStrips;  // Size of triangle strips data (bytes)
  igtl_uint32  m_NAttributes;         // Number of attributes

  // Parameters in POLYDATA header
  igtlUint8    m_AttributeType;
  igtlUint8    m_AttributeNComponents;
  igtlUint32   m_AttributeN;

};

} // namespace igtl

#endif // _igtlPolyDataMessage_h



