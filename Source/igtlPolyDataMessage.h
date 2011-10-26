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

namespace igtl
{

// Description:
// PolyDataPointArray class
class IGTLCommon_EXPORT PolyDataPointArray : public Object {

 public:
  typedef std::vector<igtlFloat32> Point;

 public:
  typedef PolyDataPointArray        Self;
  typedef Object                    Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::PolyDataPointArray, igtl::Object);
  igtlNewMacro(igtl::PolyDataPointArray);
  
 protected:
  PolyDataPointArray();
  ~PolyDataPointArray();

 public:
  void Clear();
  void SetNumberOfPoints(int n);
  int  GetNumberOfPoints();
  int  SetPoint(int id, igtlFloat32 * points);
  int  SetPoint(int id, igtlFloat32 x, igtlFloat32 y, igtlFloat32 z);
  int  AddPoint(igtlFloat32 * point);
  int  AddPoint(igtlFloat32 x, igtlFloat32 y, igtlFloat32 z);
  int  GetPoint(int id, igtlFloat32 & x, igtlFloat32 & y, igtlFloat32 & z);
  int  GetPoint(int id, igtlFloat32 * point);

 private:
  std::vector< Point > m_Data;
};


// Description:
// PolyDataCellArray class to pass vertices, lines, polygons, and triangle strips
class IGTLCommon_EXPORT PolyDataCellArray : public Object {
  
 public:
  enum {
    NULL_POINT = 0xFFFFFFFF,
  };

 public:
  typedef PolyDataCellArray         Self;
  typedef Object                    Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  igtlTypeMacro(igtl::PolyDataCellArray, igtl::Object);
  igtlNewMacro(igtl::PolyDataCellArray);

 protected:
  PolyDataCellArray();
  ~PolyDataCellArray();

 public:
  void       Clear();
  igtlUint32 GetNCells();
  void       AddCell(int n, igtlUint32 * cell);
  void       AddCell(std::list<igtlUint32> cell);
  igtlUint32 GetCellSize(int id);
  int        GetCell(int id, igtlUint32 * cell);

 private:
  std::vector< std::list<igtlUint32> > m_Data;
};


// Description:
// Attribute class used for passing attribute data
class IGTLCommon_EXPORT PolyDataAttribute : public Object {
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
  enum {
    POINT_SCALAR = 0x00,
    POINT_VECTOR = 0x01,
    POINT_NORMAL = 0x02,
    POINT_TENSOR = 0x03,
    POINT_RGBA   = 0x04,
    CELL_SCALAR  = 0x10,
    CELL_VECTOR  = 0x11,
    CELL_NORMAL  = 0x12,
    CELL_TENSOR  = 0x13,
    CELL_RGBA    = 0x14,
  };

 public:
  typedef PolyDataAttribute         Self;
  typedef Object                    Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  igtlTypeMacro(igtl::PolyDataAttribute, igtl::Object);
  igtlNewMacro(igtl::PolyDataAttribute);

 protected:
  PolyDataAttribute();
  ~PolyDataAttribute();

 public:
  void       Clear();
  void       SetType(int t);
  int        SetNComponents(int n);
  igtlUint32 GetSize();
  void       SetName(const char * name);
  void       SetData(int n, igtlFloat32 * data);

 private:
  igtlUint8              m_Type;
  igtlUint8              m_NComponents;
  std::string            m_Name;
  std::list<igtlFloat32> m_Data;
};


class IGTLCommon_EXPORT PolyDataMessage: public MessageBase
{
public:
  typedef PolyDataMessage                Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::PolyDataMessage, igtl::MessageBase);
  igtlNewMacro(igtl::PolyDataMessage);

public:

  void Clear();
  void SetPoints(PolyDataPointArray * points);
  void SetVertices(PolyDataCellArray * vertices);
  void SetLines(PolyDataCellArray * lines);
  void SetPolygons(PolyDataCellArray * polygons);
  void SetTriangleStrips(PolyDataCellArray * triangleStrips);
  void ClearAttributes();
  void AddAttribute(PolyDataAttribute * att);
 
protected:
  PolyDataMessage();
  ~PolyDataMessage();

protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

  // POLYDATA parameters
  PolyDataPointArray * m_Points;
  PolyDataCellArray  * m_Vertices;
  PolyDataCellArray  * m_Lines;
  PolyDataCellArray  * m_Polygons;
  PolyDataCellArray  * m_TriangleStrips;

  std::list<PolyDataAttribute*>    m_Attributes;
};

} // namespace igtl

#endif // _igtlPolyDataMessage_h



