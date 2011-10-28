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

igtlUint32 PolyDataCellArray::GetCellSize(int id)
{
  if (id < 0 || id > this->m_Data.size())
    {
      return 0;
    }
  return this->m_Data[id].size();
}
  
igtlUint32 PolyDataCellArray::GetTotalSize()
{
  igtlUint32 size;

  size = 0;
  std::vector< std::list<igtlUint32> >::iterator iter;
  for (iter = this->m_Data.begin(); iter != this->m_Data.end(); iter ++)
    {
    size += (*iter).size();
    }

  return size;
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


void SetPolyDataInfo(igtl_polydata_info * info, PolyDataMessage * pdmsg)
{

  if (pdmsg->GetPoints())
    {
    info->header.npoints = pdmsg->GetPoints()->GetNumberOfPoints();
    }
  else
    {
    info->header.npoints = 0;
    }

  if (pdmsg->GetVertices())
    {
    info->header.nvertices = pdmsg->GetVertices()->GetNCells();
    info->header.size_vertices = pdmsg->GetVertices()->GetTotalSize();
    }
  else
    {
    info->header.nvertices = 0;
    info->header.size_vertices = 0;
    }

  if (pdmsg->GetLines())
    {
    info->header.nlines = pdmsg->GetLines()->GetNCells();
    info->header.size_lines = pdmsg->GetLines()->GetTotalSize();
    }
  else
    {
    info->header.nlines = 0;
    info->header.size_lines = 0;
    }
    
  if (pdmsg->GetPolygons())
    {
    info->header.npolygons = pdmsg->GetPolygons()->GetNCells();
    info->header.size_polygons = pdmsg->GetPolygons()->GetTotalSize();
    }
  else
    {
    info->header.npolygons = 0;
    info->header.size_polygons = 0;
    }

  if (pdmsg->GetTriangleStrips())
    {
    info->header.ntriangle_strips = pdmsg->GetTriangleStrips()->GetNCells();
    info->header.size_triangle_strips = pdmsg->GetTriangleStrips()->GetTotalSize();
    }
  else
    {
    info->header.ntriangle_strips = 0;
    info->header.size_triangle_strips = 0;
    }
    
  if (pdmsg->GetPolygons())
    {
    info->header.npolygons = pdmsg->GetPolygons()->GetNCells();
    info->header.size_polygons = pdmsg->GetPolygons()->GetTotalSize();
    }
  else
    {
    info->header.npolygons = 0;
    info->header.size_polygons = 0;
    }

  info->header.nattributes = pdmsg->GetNumberOfAttributes();
  info->attributes = new igtl_polydata_attribute[info->header.nattributes];

  igtl_polydata_attribute * attr = info->attributes;
  for (int i = 0; i < info->header.nattributes; i ++)
    {
    PolyDataAttribute * src =  pdmsg->GetAttribute(i);
    if (src)
      {
      attr->type = src->GetType();
      attr->ncomponents = src->GetNComponents();
      attr->n = src->GetSize();
      attr->name = const_cast<char*>(src->GetName());
      attr ++;
      }
    }

}


void UnSetPolyDataInfo(igtl_polydata_info * info)
{
  delete [] info->attributes;
}


int PolyDataMessage::GetBodyPackSize()
{
  int dataSize;
  igtl_polydata_info info;

  SetPolyDataInfo(&info, this);
  dataSize = igtl_polydata_get_size(&info, IGTL_TYPE_PREFIX_NONE);
  UnSetPolyDataInfo(&info);

  return  dataSize;
}


int PolyDataMessage::PackBody()
{
  // Allocate pack
  AllocatePack();

  igtl_polydata_info info;

  SetPolyDataInfo(&info, this);

  if (igtl_polydata_alloc_info(&info) == 0)
    {
    return 0;
    }

  // Points
  if (info.points)
    {
    igtlFloat32 * ptr_f = info.points;
    for (int i = 0; i < this->m_Points->GetNumberOfPoints(); i ++)
      {
      igtlFloat32 * points;
      this->m_Points->GetPoint(i, points);
      *(ptr_f++) = points[0];
      *(ptr_f++) = points[1];
      *(ptr_f++) = points[2];
      }
    }
  else
    {
    return 0;
    }

  //Vertices
  if (info.vertices)
    {
    igtlUint32 * ptr_i = info.vertices;
    for (int i = 0; i < this->m_Vertices->GetNCells(); i ++)
      {
      this->m_Vertices->GetCell(i, ptr_i);
      ptr_i += this->m_Vertices->GetCellSize(i);
      }
    }

  //Lines
  if (info.lines)
    {
    igtlUint32 * ptr_i = info.lines;
    for (int i = 0; i < this->m_Lines->GetNCells(); i ++)
      {
      this->m_Lines->GetCell(i, ptr_i);
      ptr_i += this->m_Lines->GetCellSize(i);
      }
    }

  //Polygons
  if (info.polygons)
    {
    igtlUint32 * ptr_i = info.polygons;
    for (int i = 0; i < this->m_Polygons->GetNCells(); i ++)
      {
      this->m_Polygons->GetCell(i, ptr_i);
      ptr_i += this->m_Polygons->GetCellSize(i);
      }
    }

  //TriangleStrips
  if (info.triangle_strips)
    {
    igtlUint32 * ptr_i = info.triangle_strips;
    for (int i = 0; i < this->m_TriangleStrips->GetNCells(); i ++)
      {
      this->m_TriangleStrips->GetCell(i, ptr_i);
      ptr_i += this->m_TriangleStrips->GetCellSize(i);
      }
    }

  igtl_polydata_pack(&info, this->m_Body, IGTL_TYPE_PREFIX_NONE);

  UnSetPolyDataInfo(&info);
  
  return 1;
}


int PolyDataMessage::UnpackBody()
{
  igtl_polydata_info info;

  if (igtl_polydata_unpack(IGTL_TYPE_PREFIX_NONE, (void*)this->m_Body, &info, this->GetPackBodySize()) == 0)  
    {
    return 0;
    }
  
  // Points
  if (this->m_Points == NULL)
    {
    this->m_Points = igtl::PolyDataPointArray::New();
    }
  this->m_Points->Clear();
  if (info.header.npoints > 0)
    {
    this->m_Points->SetNumberOfPoints(info.header.npoints);
    for (int i = 0; i < info.header.npoints; i ++)
      {
      this->m_Points->SetPoint(i, &(info.points[i*3]));
      }
    }

  igtlUint32 * ptr;

  // Vertices
  if (this->m_Vertices == NULL)
    {
    this->m_Vertices = igtl::PolyDataCellArray::New();
    }
  this->m_Vertices->Clear();
  ptr = info.vertices;
  for (int i = 0; i < info.header.nvertices; i ++)
    {
    this->m_Vertices->AddCell(i, ptr);
    ptr += this->m_Vertices->GetCellSize(i);
    }

  // Lines
  if (this->m_Lines == NULL)
    {
    this->m_Lines = igtl::PolyDataCellArray::New();
    }
  this->m_Lines->Clear();
  ptr = info.lines;
  for (int i = 0; i < info.header.nlines; i ++)
    {
    this->m_Lines->AddCell(i, ptr);
    ptr += this->m_Lines->GetCellSize(i);
    }

  // Polygons
  if (this->m_Polygons == NULL)
    {
    this->m_Polygons = igtl::PolyDataCellArray::New();
    }
  this->m_Polygons->Clear();
  ptr = info.polygons;
  for (int i = 0; i < info.header.npolygons; i ++)
    {
    this->m_Polygons->AddCell(i, ptr);
    ptr += this->m_Polygons->GetCellSize(i);
    }

  // TriangleStrips
  if (this->m_TriangleStrips == NULL)
    {
    this->m_TriangleStrips = igtl::PolyDataCellArray::New();
    }
  this->m_TriangleStrips->Clear();
  ptr = info.triangle_strips;
  for (int i = 0; i < info.header.ntriangle_strips; i ++)
    {
    this->m_TriangleStrips->AddCell(i, ptr);
    ptr += this->m_TriangleStrips->GetCellSize(i);
    }

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

PolyDataPointArray* PolyDataMessage::GetPoints()
{
  return this->m_Points;
}

PolyDataCellArray* PolyDataMessage::GetVertices()
{
  return this->m_Vertices;
}

PolyDataCellArray* PolyDataMessage::GetLines()
{
  return this->m_Lines;
}

PolyDataCellArray* PolyDataMessage::GetPolygons()
{
  return this->m_Polygons;
}

PolyDataCellArray* PolyDataMessage::GetTriangleStrips()
{
  return this->m_TriangleStrips;
}

void PolyDataMessage::ClearAttributes()
{
  std::vector<PolyDataAttribute*>::iterator iter;
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

int PolyDataMessage::GetNumberOfAttributes()
{
  return this->m_Attributes.size();
}

PolyDataAttribute * PolyDataMessage::GetAttribute(int id)
{
  if (id < 0 || id >= this->m_Attributes.size())
    {
    return NULL;
    }

  return this->m_Attributes[id];
}

} // namespace igtl





