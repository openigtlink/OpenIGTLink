/*=========================================================================

  Program:   The OpenIGTLink Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink2_beta/Source/igtlPolyDataMessage.cxx $
  Language:  C++
  Date:      $Date: 2010-01-17 13:04:20 -0500 (Sun, 17 Jan 2010) $
  Version:   $Revision: 5575 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlPolyDataMessage.h"

#include "igtlTypes.h"

#include "igtl_header.h"
#include "igtl_polydata.h"

// Disable warning C4996 (strncpy() may be unsafe) in Windows. 
#define _CRT_SECURE_NO_WARNINGS

#include <string.h>

namespace igtl {


// Description:
// PolyDataPointArray class
PolyDataPointArray::PolyDataPointArray()
  : Object()
{
  Clear();
}

PolyDataPointArray::~PolyDataPointArray()
{
}

void PolyDataPointArray::Clear()
{
  this->m_Data.clear();
}

  void PolyDataPointArray::SetNumberOfPoints(int n)
  {
  this->m_Data.resize(n);
}
  
int PolyDataPointArray::GetNumberOfPoints()
{
  return this->m_Data.size();
}
  
int PolyDataPointArray::SetPoint(int id, igtlFloat32 * points)
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
  
int PolyDataPointArray::SetPoint(int id, igtlFloat32 x, igtlFloat32 y, igtlFloat32 z)
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
  
int PolyDataPointArray::AddPoint(igtlFloat32 * point)
{
  Point newPoint;
  newPoint.resize(3);
  newPoint[0] = point[0];
  newPoint[1] = point[1];
  newPoint[2] = point[2];
  this->m_Data.push_back(newPoint);
  
  return 1;
}
  
int PolyDataPointArray::AddPoint(igtlFloat32 x, igtlFloat32 y, igtlFloat32 z)
{
  Point newPoint;
  newPoint.resize(3);
  newPoint[0] = x;
  newPoint[1] = y;
  newPoint[2] = z;
  this->m_Data.push_back(newPoint);
  
  return 1;
}
  
int PolyDataPointArray::GetPoint(int id, igtlFloat32 & x, igtlFloat32 & y, igtlFloat32 & z)
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

int PolyDataPointArray::GetPoint(int id, igtlFloat32 * point)
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


// Description:
// PolyDataCellArray class to pass vertices, lines, polygons, and triangle strips
PolyDataCellArray::PolyDataCellArray()
  : Object()
{
  Clear();
}

PolyDataCellArray::~PolyDataCellArray()
{}

void PolyDataCellArray::Clear()
{
  this->m_Data.clear();
}
  
igtlUint32 PolyDataCellArray::GetNCells()
{
  return this->m_Data.size();
}
  
void PolyDataCellArray::AddCell(int n, igtlUint32 * cell)
{
  std::list<igtlUint32> newCell;
  for (int i = 0; i < n; i ++)
    {
    newCell.push_back(cell[i]);
    }
  this->m_Data.push_back(newCell);
}

void PolyDataCellArray::AddCell(std::list<igtlUint32> cell)
{
  this->m_Data.push_back(cell);
}

igtlUint32 PolyDataCellArray::GetCellSize(int id) {
    if (id < 0 || id > this->m_Data.size())
      {
      return 0;
      }
    return this->m_Data[id].size();
  }

int PolyDataCellArray::GetCell(int id, igtlUint32 * cell)
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

// Description:
// Attribute class used for passing attribute data
PolyDataAttribute::PolyDataAttribute()
  : Object()
{
  Clear();
}

PolyDataAttribute::~PolyDataAttribute()
{
}

void PolyDataAttribute::Clear()
{
  this->m_Type        = POINT_SCALAR;
  this->m_NComponents = 1;
  this->m_Name        = "";
  this->m_Data.clear();
}

void PolyDataAttribute::SetType(int t)
{
  this->m_Type = t; 
}

int PolyDataAttribute::SetNComponents(int n)
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

int PolyDataAttribute::GetNComponents()
{
  return this->m_NComponents;
}

igtlUint32 PolyDataAttribute::GetSize() 
{
  return this->m_Data.size();
}

void PolyDataAttribute::SetName(const char * name)
{
  this->m_Name = name;
}

void PolyDataAttribute::SetData(int n, igtlFloat32 * data)
{
  this->m_Data.resize(n);
  std::list<igtlFloat32>::iterator iter;
  for (iter = this->m_Data.begin(); iter != this->m_Data.end(); iter ++)
    {
    *iter = *data;
    data ++;
    }
}


// Description:
// PolyDataMessage class implementation
PolyDataMessage::PolyDataMessage():
  MessageBase()
{
  this->m_DefaultBodyType = "POLYDATA";
  Clear();
}


PolyDataMessage::~PolyDataMessage()
{
}


int PolyDataMessage::GetBodyPackSize()
{
  int dataSize;

  igtl_polydata_info info;
  
  if (this->m_Points)
    {
    info.header.npoints = this->m_Points->GetNumberOfPoints();
    }
  else
    {
    info.header.npoints = 0;
    }

  if (this->m_Vertices)
    {
    info.header.nvertices = this->m_Vertices->GetNCells();
    info.header.size_vertices = this->m_Vertices->TotalSize();
    }
  else
    {
    info.header.nvertices = 0;
    info.header.size_vertices = 0;
    }

  if (this->m_Lines)
    {
    info.header.nlines = this->m_Lines->GetNCells();
    info.header.size_lines = this->m_Lines->TotalSize();
    }
  else
    {
    info.header.nlines = 0;
    info.header.size_lines = 0;
    }
    
  if (this->m_Polygons)
    {
    info.header.npolygons = this->m_Polygons->GetNCells();
    info.header.size_polygons = this->m_Polygons->TotalSize();
    }
  else
    {
    info.header.npolygons = 0;
    info.header.size_polygons = 0;
    }

  if (this->m_TriangleStrips)
    {
    info.header.ntriangle_strips = this->m_TriangleStrips->GetNCells();
    info.header.size_triangle_strips = this->m_TriangleStrips->TotalSize();
    }
  else
    {
    info.header.ntriangle_strips = 0;
    info.header.size_triangle_strips = 0;
    }
    
  if (this->m_Polygons)
    {
    info.header.npolygons = this->m_Polygons->GetNCells();
    info.header.size_polygons = this->m_Polygons->TotalSize();
    }
  else
    {
    info.header.npolygons = 0;
    info.header.size_polygons = 0;
    }

  info.header.nattributes = this->m_Attributes.size();
  info.attributes = new igtl_polydata_attribute[info.header.nattributes];

  igtl_polydata_attribute * attr = info.attributes;
  std::list<PolyDataAttribute*>::iterator iter;
  for (iter = this->m_Attributes.begin(); iter != this->m_Attributes.end(); iter ++)
    {
    (*attr)->type = (*iter)->GetType();
    (*attr)->ncomponents = (*iter)->GetNComponents();
    (*attr)->n = (*iter)->GetSize();
    (*attr)->name = (*iter)->GetName();
    }

  dataSize = igtl_polydata_get_size(info, IGTL_TYPE_PREFIX_NONE);

  return  dataSize;
}


int PolyDataMessage::PackBody()
{
  // Allocate pack
  AllocatePack();

  //if (this->m_Array == NULL)
  //  {
  //  return 0;
  //  }
  //
  //igtl_ndarray_info info;
  //
  //igtl_ndarray_init_info(&info);
  //info.dim  = this->m_Array->GetDimension();
  //info.type = this->m_Type;
  //
  //ArrayBase::IndexType size = this->m_Array->GetSize();
  //
  //igtl_uint16 * s = new igtl_uint16[info.dim];
  //for (int i = 0; i < info.dim; i ++)
  //  {
  //  s[i] = size[i];
  //  }
  //int r = igtl_ndarray_alloc_info(&info, s);
  //delete s;
  //
  //if (r == 0)
  //  {
  //  return 0;
  //  }
  //
//// size.data() doesn't work for some environtments (MacOS X 10.5 and Win32, as far as I know)
////  if (igtl_ndarray_alloc_info(&info, size.data()) == 0)
////    {
////    return 0;
////    }
  //
  //memcpy(info.array, this->m_Array->GetRawArray(), this->m_Array->GetRawArraySize());
  //igtl_ndarray_pack(&info, this->m_Body, IGTL_TYPE_PREFIX_NONE);

  return 1;
}


int PolyDataMessage::UnpackBody()
{

//  igtl_ndarray_info info;
//
//  igtl_ndarray_unpack(IGTL_TYPE_PREFIX_NONE, this->m_Body, &info, this->GetPackBodySize());
//
//  this->m_Type = info.type;
//  ArrayBase::IndexType size;
//  size.resize(info.dim);
//  for (int i = 0; i < info.dim; i ++)
//    {
//    size[i] = info.size[i];
//    }
//
//  switch (this->m_Type)
//    {
//    case TYPE_INT8:
//      this->m_Array = new Array<igtlInt8>;
//      break;
//    case TYPE_UINT8:
//      this->m_Array = new Array<igtlUint8>;
//      break;
//    case TYPE_INT16:
//      this->m_Array = new Array<igtlInt16>;
//      break;
//    case TYPE_UINT16:
//      this->m_Array = new Array<igtlUint16>;
//      break;
//    case TYPE_INT32:
//      this->m_Array = new Array<igtlInt32>;
//      break;
//    case TYPE_UINT32:
//      this->m_Array = new Array<igtlUint32>;
//      break;
//    case TYPE_FLOAT32:
//      this->m_Array = new Array<igtlFloat32>;
//      break;
//    case TYPE_FLOAT64:
//      this->m_Array = new Array<igtlFloat64>;
//      break;
//    case TYPE_COMPLEX:
//      this->m_Array = new Array<igtlComplex>;
//      break;
//    default:
//      return 0;
//      break;
//    }
//
//  this->m_Array->SetSize(size);
//  memcpy(this->m_Array->GetRawArray(), info.array, this->m_Array->GetRawArraySize());
//  
  return 1;
}

void PolyDataMessage::Clear()
{

  if (this->m_Points)
    {
    //this->m_Points->Delete();
    this->m_Points = NULL;
    }
  if (this->m_Vertices)
    {
    //this->m_Vertices->Delete();
    this->m_Vertices = NULL;
    }
  if(this->m_Lines)
    {
    //this->m_Lines->Delete();
    this->m_Lines = NULL;
    }
  if (this->m_Polygons)
    {
    //this->m_Polygons->Delete();
    this->m_Polygons = NULL;
    }
  if (this->m_TriangleStrips)
    {
    //this->m_TriangleStrips->Delete();
    this->m_TriangleStrips = NULL;
    }
  this->m_Attributes.clear();
}

void PolyDataMessage::SetPoints(PolyDataPointArray * points)
{
  if (this->m_Points)
    {
    //this->m_Points->Delete();
    }
  this->m_Points = points;
  
}

void PolyDataMessage::SetVertices(PolyDataCellArray * vertices)
{
  if (this->m_Vertices)
    {
    //this->m_Vertices->Delete();
    }
  this->m_Vertices = vertices;
}

void PolyDataMessage::SetLines(PolyDataCellArray * lines)
{
  if (this->m_Lines)
    {
    //this->m_Lines->Delete();
    }
  this->m_Lines = lines;
}

void PolyDataMessage::SetPolygons(PolyDataCellArray * polygons)
{
  if (this->m_Polygons)
    {
    //this->m_Polygons->Delete();
    }
  this->m_Polygons = polygons;
}

void PolyDataMessage::SetTriangleStrips(PolyDataCellArray * triangleStrips)
{
  if (this->m_TriangleStrips)
    {
    //this->m_TriangleStrips->Delete();
    }
  this->m_TriangleStrips = triangleStrips;
}

void PolyDataMessage::ClearAttributes()
{
  std::list<PolyDataAttribute*>::iterator iter;
  for (iter = this->m_Attributes.begin(); iter != this->m_Attributes.end(); iter ++)
    {
    //(*iter)->Delete();
    *iter = NULL;
    }
  this->m_Attributes.clear();
}

void PolyDataMessage::AddAttribute(PolyDataAttribute * att)
{
  this->m_Attributes.push_back(att);
}

} // namespace igtl





