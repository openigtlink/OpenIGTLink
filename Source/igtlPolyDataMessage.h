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
#include "igtlMacro.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

namespace igtl
{

class IGTLCommon_EXPORT GetPolyDataMessage: public MessageBase
{
public:
  typedef GetPolyDataMessage            Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::GetPolyDataMessage, igtl::MessageBase);
  igtlNewMacro(igtl::GetPolyDataMessage);

protected:
  GetPolyDataMessage() : MessageBase() { this->m_DefaultBodyType  = "GET_POLYDATA"; };
  ~GetPolyDataMessage() {};
protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };
};


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
  int  SetPoint(unsigned int id, igtlFloat32 * points);
  int  SetPoint(unsigned int id, igtlFloat32 x, igtlFloat32 y, igtlFloat32 z);
  int  AddPoint(igtlFloat32 * point);
  int  AddPoint(igtlFloat32 x, igtlFloat32 y, igtlFloat32 z);
  int  GetPoint(unsigned int id, igtlFloat32 & x, igtlFloat32 & y, igtlFloat32 & z);
  int  GetPoint(unsigned int id, igtlFloat32 * point);

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
  igtlUint32 GetNumberOfCells();
  void       AddCell(int n, igtlUint32 * cell);
  void       AddCell(std::list<igtlUint32> cell);

  // Description:
  // GetTotalSize() returns the total memory size of the cell data array in
  // POLYDATA message. Cell data array is array of cell data, consisting of
  // <number of points> and array of <point indecies>. Both <number of points>
  // and <point indecies> are unsigned 32-bit integer. 
  // Consequently, the total size can be calculated by: 
  // sizeof(igtlUint32) * (number of points for 0th cell + 1) + (number of points for 1th cell + 1)
  // ... + (number of points for (N-1)th cell + 1). Note that this includes the first igtlUint32 value
  // that specifies the number of points in each cell.
  igtlUint32 GetTotalSize();

  igtlUint32 GetCellSize(unsigned int id);
  int        GetCell(unsigned int id, igtlUint32 * cell);
  int        GetCell(unsigned int id, std::list<igtlUint32>& cell);

 private:
  std::vector< std::list<igtlUint32> > m_Data;
};


// Description:
// Attribute class used for passing attribute data
class IGTLCommon_EXPORT PolyDataAttribute : public Object {
 public:
  /*
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
  */
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
  void        Clear();

  // Description:
  // SetType() is used to set attribute type. If the attribute is set properly,
  // the function returns the type value (POINT_* or CELL_*). Otherwise
  // the function returns negative value. The second argument will be ignored
  // if 't' is neither POINT_SCALAR nor CELL_SCALAR.
  // If the POINT_SCALAR and CELL_SCALAR is specified as 't', the number of
  // components can be specified as the second argument. The number of
  // components must be 0 < n < 128.
  int         SetType(int t, int n=1);
  igtlUint8   GetType() { return this->m_Type; };
  //int         SetNComponents(int n);
  igtlUint32  GetNumberOfComponents();

  igtlUint32  SetSize(igtlUint32 size);
  igtlUint32  GetSize();

  void        SetName(const char * name);
  const char* GetName() { return this->m_Name.c_str(); };

  int         SetData(igtlFloat32 * data);
  int         GetData(igtlFloat32 * data);

  int         SetNthData(unsigned int n, igtlFloat32 * data);
  int         GetNthData(unsigned int n, igtlFloat32 * data);

 private:

  igtlUint8                m_Type;
  igtlUint8                m_NComponents;
  igtlUint32               m_Size;
  std::string              m_Name;
  std::vector<igtlFloat32> m_Data;

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
  /*
  void SetPoints(PolyDataPointArray * points);
  void SetVertices(PolyDataCellArray * vertices);
  void SetLines(PolyDataCellArray * lines);
  void SetPolygons(PolyDataCellArray * polygons);
  void SetTriangleStrips(PolyDataCellArray * triangleStrips);
  */

  igtlSetObjectMacro(Points,   PolyDataPointArray);
  igtlGetObjectMacro(Points,   PolyDataPointArray);
  igtlSetObjectMacro(Vertices, PolyDataCellArray);
  igtlGetObjectMacro(Vertices, PolyDataCellArray);
  igtlSetObjectMacro(Lines,    PolyDataCellArray);
  igtlGetObjectMacro(Lines,    PolyDataCellArray);
  igtlSetObjectMacro(Polygons, PolyDataCellArray);
  igtlGetObjectMacro(Polygons, PolyDataCellArray);
  igtlSetObjectMacro(TriangleStrips, PolyDataCellArray);
  igtlGetObjectMacro(TriangleStrips, PolyDataCellArray);

  /*
  PolyDataPointArray * GetPoints();
  PolyDataCellArray  * GetVertices();
  PolyDataCellArray  * GetLines();
  PolyDataCellArray  * GetPolygons();
  PolyDataCellArray  * GetTriangleStrips();
  */

  void ClearAttributes();
  void AddAttribute(PolyDataAttribute * att);
  int  GetNumberOfAttributes();
  PolyDataAttribute * GetAttribute(unsigned int id);
 
protected:
  PolyDataMessage();
  ~PolyDataMessage();

protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

  // POLYDATA parameters
  PolyDataPointArray::Pointer m_Points;
  PolyDataCellArray::Pointer  m_Vertices;
  PolyDataCellArray::Pointer  m_Lines;
  PolyDataCellArray::Pointer  m_Polygons;
  PolyDataCellArray::Pointer  m_TriangleStrips;
  
  std::vector<PolyDataAttribute::Pointer> m_Attributes;

};

} // namespace igtl

#endif // _igtlPolyDataMessage_h



