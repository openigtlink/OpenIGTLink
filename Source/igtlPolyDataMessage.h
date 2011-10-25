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
class IGTLCommon_EXPORT PolyDataPointArray {

 public:
  typedef std::vector<igtlFloat32> Point;

 public:
  PolyDataPointArray()
  {
    Clear();
  }
  ~PolyDataPointArray()
  {
  }

  void Clear()
  {
    this->m_Data.clear();
  }

  void SetNumberOfPoints(int n)
  {
    this->m_Data.resize(n);
  }

  int GetNumberOfPoints()
  {
    return this->m_Data.size();
  }

  int SetPoint(int id, igtlFloat32 * points)
  {
    if (id < 0 || id > this->m_Data.size())
      {
      return 0;
      }
    Point & dst = this->m_Data[id];
    dst[0] = points[0];
    dst[1] = points[1];
    dst[2] = points[2];
    return 1;
  }

  int SetPoint(int id, igtlFloat32 x, igtlFloat32 y, igtlFloat32 z)
  {
    if (id < 0 || id > this->m_Data.size())
      {
      return 0;
      }
    Point & dst = this->m_Data[id];
    dst[0] = x;
    dst[1] = y;
    dst[2] = z;
    return 1;
  }

  int AddPoint(igtlFloat32 * point)
  {
    Point newPoint;
    newPoint.resize(3);
    newPoint[0] = point[0];
    newPoint[1] = point[1];
    newPoint[2] = point[2];
    this->m_Data.push_back(newPoint);

    return 1;
  }
  
  int AddPoint(igtlFloat32 x, igtlFloat32 y, igtlFloat32 z)
  {
    Point newPoint;
    newPoint.resize(3);
    newPoint[0] = x;
    newPoint[1] = y;
    newPoint[2] = z;
    this->m_Data.push_back(newPoint);

    return 1;
  }
  
  int GetPoint(int id, igtlFloat32 & x, igtlFloat32 & y, igtlFloat32 & z)
  {
    if (id < 0 || id > this->m_Data.size())
      {
      return 0;
      }
    Point & dst = this->m_Data[id];
    x = dst[0];
    y = dst[1];
    z = dst[2];
  }

  int GetPoint(int id, igtlFloat32 * point)
  {
    if (id < 0 || id > this->m_Data.size())
      {
      return 0;
      }
    Point & dst = this->m_Data[id];
    point[0] = dst[0];
    point[1] = dst[1];
    point[2] = dst[2];
  }


 private:
  std::vector< Point > m_Data;
};


// Description:
// PolyDataCellArray class to pass vertices, lines, polygons, and triangle strips
class IGTLCommon_EXPORT PolyDataCellArray {
  
 public:
  enum {
    NULL_POINT = 0xFFFFFFFF,
  };

 public:
  PolyDataCellArray()
  {
    Clear();
  }
  ~PolyDataCellArray()  {}

  void Clear()
  {
    this->m_Data.clear();
  }

  igtlUint32 GetNCells()
  {
    return this->m_Data.size();
  }

  void AddCell(int n, igtlUint32 * cell)
  {
    std::list<igtlUint32> newCell;
    for (int i = 0; i < n; i ++)
      {
      newCell.push_back(cell[i]);
      }
    this->m_Data.push_back(newCell);
  }

  void AddCell(std::list<igtlUint32> cell)
  {
    this->m_Data.push_back(cell);
  }

  igtlUint32 GetCellSize(int id) {
    if (id < 0 || id > this->m_Data.size())
      {
      return 0;
      }
    return this->m_Data[id].size();
  }

  int GetCell(int id, igtlUint32 * cell)
  {
    if (id < 0 || id > this->m_Data.size())
      {
      return 0;
      }
    std::list<igtlUint32> & src = this->m_Data[id];
    std::list<igtlUint32>::iterator iter;

    for (iter = src.begin(); iter != src.end(); iter ++)
      {
      *cell = *iter;
      cell ++;
      }
    return 1;
  }

 private:
  std::vector< std::list<igtlUint32> > m_Data;
};

// Description:
// Attribute class used for passing attribute data
class IGTLCommon_EXPORT PolyDataAttribute{
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
  PolyDataAttribute()
  {
    Clear();
  }
  ~PolyDataAttribute() {}

  void Clear()
  {
    this->m_Type        = POINT_SCALAR;
    this->m_NComponents = 1;
    this->m_Name        = "";
    this->m_Data.clear();
  }

  void SetType(int t)
  {
    this->m_Type = t; 
  }

  int SetNComponents(int n)
  {
    if (n > 0 || n < 256)
      {
      this->m_NComponents = n;
      return n;
      }
    else
      {
      return 0;
      }
  }

  igtlUint32 GetSize() 
  {
    return this->m_Data.size();
  }

  void SetName(const char * name)
  {
    this->m_Name = name;
  }

  void SetData(int n, igtlFloat32 * data)
  {
    this->m_Data.resize(n);
    std::list<igtlFloat32>::iterator iter;
    for (iter = this->m_Data.begin(); iter != this->m_Data.end(); iter ++)
      {
      *iter = *data;
      data ++;
      }
  }

 private:
  igtlUint8              m_Type;
  igtlUint8              m_NComponents;
  std::string            m_Name;
  std::list<igtlFloat32> m_Data;
};



class IGTLCommon_EXPORT PolyDataMessage: public MessageBase
{
public:
  typedef PolyDataMessage                 Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::PolyDataMessage, igtl::MessageBase);
  igtlNewMacro(igtl::PolyDataMessage);

public:

  // POLYDATA parameters
  void ClearPoints();
  void SetNumberOfPoints(int n);

  void ClearVertices();
  void GetNumberOfVertices();
 
protected:
  PolyDataMessage();
  ~PolyDataMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();

  // POLYDATA parameters
  std::list<igtlFloat32>  m_Points;
  std::list<igtlUint32>   m_VerticesSize;
  std::list<igtlUint32>   m_VerticesData;
  std::list<igtlUint32>   m_LinesSize;
  std::list<igtlUint32>   m_LinesData;
  std::list<igtlUint32>   m_PolygonsSize;
  std::list<igtlUint32>   m_PolygonsData;
  std::list<igtlUint32>   m_TriangleStripsSize;
  std::list<igtlUint32>   m_TriangleStripsData;

  //std::list<Attribute>    m_Attributes;
};

} // namespace igtl

#endif // _igtlPolyDataMessage_h



