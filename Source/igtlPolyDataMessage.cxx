/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

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
#include <stdlib.h>

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

  std::vector< Point >::iterator iter;
  for (iter = this->m_Data.begin(); iter != this->m_Data.end(); iter ++)
    {
    iter->resize(3);
    }
}
  
int PolyDataPointArray::GetNumberOfPoints()
{
  return this->m_Data.size();
}
  
int PolyDataPointArray::SetPoint(unsigned int id, igtlFloat32 * point)
{
  if (id >= this->m_Data.size())
    {
    return 0;
    }
  Point & dst = this->m_Data[id];
  dst[0] = point[0];
  dst[1] = point[1];
  dst[2] = point[2];
  return 1;
}
  
int PolyDataPointArray::SetPoint(unsigned int id, igtlFloat32 x, igtlFloat32 y, igtlFloat32 z)
{
  if (id >= this->m_Data.size())
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
  
int PolyDataPointArray::GetPoint(unsigned int id, igtlFloat32 & x, igtlFloat32 & y, igtlFloat32 & z)
{
  if (id >= this->m_Data.size())
    {
    return 0;
    }
  Point & dst = this->m_Data[id];
  x = dst[0];
  y = dst[1];
  z = dst[2];
  return 1;
}

int PolyDataPointArray::GetPoint(unsigned int id, igtlFloat32 * point)
{
  if (id >= this->m_Data.size())
    {
    return 0;
    }
  Point & dst = this->m_Data[id];
  point[0] = dst[0];
  point[1] = dst[1];
  point[2] = dst[2];
  return 1;
}

/// Implement support for C++11 ranged for loops
igtlRangeBasedForBodyMacro(PolyDataPointArray, std::vector<PolyDataPointArray::Point>, m_Data);

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
  
igtlUint32 PolyDataCellArray::GetNumberOfCells()
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
  if (n > 0)
    {
    this->m_Data.push_back(newCell);
    }
}

void PolyDataCellArray::AddCell(const Cell& cell)
{
  this->m_Data.push_back(cell);
}

igtlUint32 PolyDataCellArray::GetCellSize(unsigned int id)
{
  if (id >= this->m_Data.size())
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
    size += ((*iter).size() + 1);
    }

  return size * sizeof(igtlUint32);
}

int PolyDataCellArray::GetCell(unsigned int id, igtlUint32 * cell)
{
  if (id >= this->m_Data.size())
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


int PolyDataCellArray::GetCell(unsigned int id, std::list<igtlUint32>& cell)
{
  if (id >= this->m_Data.size())
    {
    return 0;
    }
  std::list<igtlUint32> & src = this->m_Data[id];
  cell.resize(src.size());

  std::list<igtlUint32>::iterator srcIter;
  std::list<igtlUint32>::iterator dstIter = cell.begin();
  
  for (srcIter = src.begin(); srcIter != src.end(); srcIter ++)
    {
    *dstIter = *srcIter;
    dstIter ++;
    }
  return 1;
}

/// Implement support for C++11 ranged for loops
igtlRangeBasedForBodyMacro(PolyDataCellArray, std::vector<PolyDataCellArray::Cell>, m_Data);

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
  this->m_Size        = 0;
}

int PolyDataAttribute::SetType(int t, int n)
{
  int valid = 0;

  switch(t)
    {
    case POINT_SCALAR:
    case CELL_SCALAR:
      if (n > 0 && n < 128)
        {
        valid = 1;
        this->m_NComponents = n;
        }
      break;
    case POINT_TCOORDS:
    case CELL_TCOORDS:
    case POINT_VECTOR:
    case CELL_VECTOR:
    case POINT_NORMAL:
    case CELL_NORMAL:
      valid = 1;
      this->m_NComponents = 3;
      break;
    case POINT_TENSOR:
    case CELL_TENSOR:
      valid = 1;
      this->m_NComponents = 9;
      break;
    case POINT_RGBA:
    case CELL_RGBA:
      valid = 1;
      this->m_NComponents = 4;
      break;
    default:
      break;
    }
  if (valid)
    {
    this->m_Type = t;
    unsigned int n = this->m_Size * this->m_NComponents;
    if (n != this->m_Data.size())
      {
      // TODO: this may cause unnecessary memory allocation,
      // unless m_Size == 0.
      // Memory should be reallocate just before use.
      this->m_Data.resize(n);
      }
    return t;
    }
  else
    {
    return -1;
    }
}


igtlUint32 PolyDataAttribute::GetNumberOfComponents()
{
  return this->m_NComponents;
}

igtlUint32 PolyDataAttribute::SetSize(igtlUint32 size)
{
  this->m_Size = size;

  unsigned int n = this->m_Size * this->m_NComponents;
  if (n != this->m_Data.size())
    {
    // TODO: this may cause unnecessary memory allocation.
    // Memory should be reallocate just before use.
    this->m_Data.resize(n);
    }

  this->m_Data.resize(size*this->m_NComponents);
  return this->m_Size;
}

igtlUint32 PolyDataAttribute::GetSize() 
{
  return this->m_Size;
}

void PolyDataAttribute::SetName(const char * name)
{
  this->m_Name = name;
}

int PolyDataAttribute::SetData(igtlFloat32 * data)
{
  if (!data)
    {
    return 0;
    }

  std::vector<igtlFloat32>::iterator iter;
  for (iter = this->m_Data.begin(); iter != this->m_Data.end(); iter ++)
    {
    *iter = *data;
    data ++;
    }

  return 1;
}

int PolyDataAttribute::GetData(igtlFloat32 * data)
{
  if (!data)
    {
    return 0;
    }

  std::vector<igtlFloat32>::iterator iter;
  for (iter = this->m_Data.begin(); iter != this->m_Data.end(); iter ++)
    {
    *data = *iter;
    data ++;
    }
  return 1;
}

int PolyDataAttribute::SetNthData(unsigned int n, igtlFloat32 * data)
{
  if (n >= this->m_Size)
    {
    return 0;
    }

  std::vector<igtlFloat32>::iterator iter;
  iter = this->m_Data.begin() + n*this->m_NComponents;
  for (unsigned int i = 0; i < this->m_NComponents; i ++)
    {
    *iter = *data;
    iter ++;
    data ++;
    }
  return 1;
}

int PolyDataAttribute::GetNthData(unsigned int n, igtlFloat32 * data)
{
  if (n >= this->m_Size)
    {
    return 0;
    }

  std::vector<igtlFloat32>::iterator iter;
  iter = this->m_Data.begin() + n*this->m_NComponents;
  for (unsigned int i = 0; i < this->m_NComponents; i ++)
    {
    *data = *iter;
    iter ++;
    data ++;
    }

  return 1;
}

int PolyDataAttribute::GetNthData(unsigned int n, std::vector<igtlFloat32>& data)
{
  if (n >= this->m_Size)
    {
    return 0;
    }

  data.clear();
  std::vector<igtlFloat32>::iterator iter;
  iter = this->m_Data.begin() + n*this->m_NComponents;
  for (unsigned int i = 0; i < this->m_NComponents; i++)
    {
    data.push_back(*iter);
    iter++;
    }

  return 1;
}

/// Implement support for C++11 ranged for loops
std::vector<igtlFloat32>::iterator PolyDataAttribute::begin()
{
  return m_Data.begin();
}
std::vector<igtlFloat32>::iterator PolyDataAttribute::end()
{
  return m_Data.end();
}
std::vector<igtlFloat32>::const_iterator PolyDataAttribute::begin() const
{
  return m_Data.begin();
}
std::vector<igtlFloat32>::const_iterator PolyDataAttribute::end() const
{
  return m_Data.end();
}

std::vector<igtlFloat32>::reverse_iterator PolyDataAttribute::rbegin()
{
  return m_Data.rbegin();
}
std::vector<igtlFloat32>::reverse_iterator PolyDataAttribute::rend()
{
  return m_Data.rend();
}
std::vector<igtlFloat32>::const_reverse_iterator PolyDataAttribute::rbegin() const
{
  return m_Data.rbegin();
}
std::vector<igtlFloat32>::const_reverse_iterator PolyDataAttribute::rend() const
{
  return m_Data.rend();
}

/// Implement support for C++11 ranged for loops
std::vector<igtlFloat32>::iterator begin(PolyDataAttribute& list)
{
  return list.begin();
}
std::vector<igtlFloat32>::iterator end(PolyDataAttribute& list)
{
  return list.end();
}
std::vector<igtlFloat32>::const_iterator begin(const PolyDataAttribute& list)
{
  return list.begin();
}
std::vector<igtlFloat32>::const_iterator end(const PolyDataAttribute& list)
{
  return list.end();
}

std::vector<igtlFloat32>::reverse_iterator rbegin(PolyDataAttribute& list)
{
  return list.rbegin();
}
std::vector<igtlFloat32>::reverse_iterator rend(PolyDataAttribute& list)
{
  return list.rend();
}
std::vector<igtlFloat32>::const_reverse_iterator rbegin(const PolyDataAttribute& list)
{
  return list.rbegin();
}
std::vector<igtlFloat32>::const_reverse_iterator rend(const PolyDataAttribute& list)
{
  return list.rend();
}

// Description:
// PolyDataMessage class implementation
PolyDataMessage::PolyDataMessage()
{
  this->m_SendMessageType = "POLYDATA";
  Clear();
}


PolyDataMessage::~PolyDataMessage()
{
}


void IGTLCommon_EXPORT SetPolyDataInfo(igtl_polydata_info * info, PolyDataMessage * pdm)
{

  igtl_polydata_init_info(info);

  if (pdm->GetPoints())
    {
    info->header.npoints = pdm->GetPoints()->GetNumberOfPoints();
    }
  else
    {
    info->header.npoints = 0;
    }

  if (pdm->GetVertices())
    {
    info->header.nvertices = pdm->GetVertices()->GetNumberOfCells();
    info->header.size_vertices = pdm->GetVertices()->GetTotalSize();
    }
  else
    {
    info->header.nvertices = 0;
    info->header.size_vertices = 0;
    }

  if (pdm->GetLines())
    {
    info->header.nlines = pdm->GetLines()->GetNumberOfCells();
    info->header.size_lines = pdm->GetLines()->GetTotalSize();
    }
  else
    {
    info->header.nlines = 0;
    info->header.size_lines = 0;
    }
    
  if (pdm->GetPolygons())
    {
    info->header.npolygons = pdm->GetPolygons()->GetNumberOfCells();
    info->header.size_polygons = pdm->GetPolygons()->GetTotalSize();
    }
  else
    {
    info->header.npolygons = 0;
    info->header.size_polygons = 0;
    }

  if (pdm->GetTriangleStrips())
    {
    info->header.ntriangle_strips = pdm->GetTriangleStrips()->GetNumberOfCells();
    info->header.size_triangle_strips = pdm->GetTriangleStrips()->GetTotalSize();
    }
  else
    {
    info->header.ntriangle_strips = 0;
    info->header.size_triangle_strips = 0;
    }

  info->header.nattributes = pdm->GetNumberOfAttributes();

}


void IGTLCommon_EXPORT SetPolyDataInfoAttribute(igtl_polydata_info * info, PolyDataMessage * pdm)
{

  igtl_polydata_attribute * attr = info->attributes;
  for (unsigned int i = 0; i < info->header.nattributes; i ++)
    {
    PolyDataAttribute * src =  pdm->GetAttribute(static_cast<PolyDataMessage::AttributeList::size_type>(i));
    if (src)
      {
      attr->type = src->GetType();
      attr->ncomponents = src->GetNumberOfComponents();
      attr->n = src->GetSize();
      //attr->name = const_cast<char*>(src->GetName());
      // TODO: aloways allocating memory isn't a good approach...
      if (attr->name)
        {
        free(attr->name);
        }
      attr->name =  (char *) malloc(strlen(src->GetName())+1);
      if (attr->name)
        {
        strcpy(attr->name, src->GetName());
        }
      if (attr->data)
        {
        free(attr->data);
        }
      igtlUint32 size = attr->ncomponents * attr->n;
      attr->data = (igtlFloat32*)malloc((size_t)size*sizeof(igtlFloat32));
      if (attr->data)
        {
        src->GetData(attr->data);
        }
      attr ++;
      }
    }
}


void IGTLCommon_EXPORT UnSetPolyDataInfoAttribute(igtl_polydata_info * info)
{

  igtl_polydata_attribute * attr = info->attributes;
  for (unsigned int i = 0; i < info->header.nattributes; i ++)
    {
    attr->type = 0;
    attr->ncomponents = 0;
    attr->n = 0;
    if (attr->name)
      {
      free(attr->name);
      attr->name = NULL;
      }
    if (attr->data)
      {
      free(attr->data);
      attr->data = NULL;
      }
    attr ++;
    }
}


igtlUint64 PolyDataMessage::CalculateContentBufferSize()
{
  // TODO: The current implementation of GetBodyPackSize() allocates
  // igtl_polydata_info and the array of igtl_polydata_attribute to calculate
  // the size of pack. However, this approach is not efficient because
  // it causes unnecessary memory allocation. 

  int dataSize;
  igtl_polydata_info info;

  SetPolyDataInfo(&info, this);

  // Instead of calling igtl_polydata_alloc_info(), we only allocate
  // memory for the attribute array, since igtl_polydata_alloc_info()
  // allocates also allocates the memory area for actual points and
  // cell data, which is not necessary to calculate polydata size.

  info.attributes = new igtl_polydata_attribute[info.header.nattributes];
  
  if (!info.attributes)
    {
    //ERROR
    return 0;
    }

  // init attributes
  igtl_polydata_attribute * attr = info.attributes;
  for (unsigned int i = 0; i < info.header.nattributes; i ++)
    {
    attr->type = 0;
    attr->ncomponents = 0;
    attr->n = 0;
    attr->name = NULL;
    attr->data = NULL;
    attr ++;
    }

  SetPolyDataInfoAttribute(&info, this);
  
  dataSize = igtl_polydata_get_size(&info, IGTL_TYPE_PREFIX_NONE);
  
  UnSetPolyDataInfoAttribute(&info);

  //delete [] (info.attributes);
  delete [] info.attributes;

  return dataSize;
}


int PolyDataMessage::PackContent()
{
  // Allocate buffer
  AllocateBuffer();

  igtl_polydata_info info;

  SetPolyDataInfo(&info, this);

  if (igtl_polydata_alloc_info(&info) == 0)
    {
    return 0;
    }

  //SetPolyDataInfoAttribute(&info, this);

  // Points
  if (info.points)
    {
    igtlFloat32 * ptr_f = info.points;
    for (int i = 0; i < this->m_Points->GetNumberOfPoints(); i ++)
      {
      igtlFloat32 points[3];
      this->m_Points->GetPoint(i, points);
      *(ptr_f++) = points[0];
      *(ptr_f++) = points[1];
      *(ptr_f++) = points[2];
      }
    }

  //Vertices
  if (info.vertices)
    {
    igtlUint32 * ptr_i = info.vertices;
    for (unsigned int i = 0; i < this->m_Vertices->GetNumberOfCells(); i ++)
      {
      *ptr_i = this->m_Vertices->GetCellSize(i);
      ptr_i ++;
      this->m_Vertices->GetCell(i, ptr_i);
      ptr_i += this->m_Vertices->GetCellSize(i);
      }
    }

  //Lines
  if (info.lines)
    {
    igtlUint32 * ptr_i = info.lines;
    for (unsigned int i = 0; i < this->m_Lines->GetNumberOfCells(); i ++)
      {
      *ptr_i = this->m_Lines->GetCellSize(i);
      ptr_i ++;
      this->m_Lines->GetCell(i, ptr_i);
      ptr_i += this->m_Lines->GetCellSize(i);
      }
    }

  //Polygons
  if (info.polygons)
    {
    igtlUint32 * ptr_i = info.polygons;
    for (unsigned int i = 0; i < this->m_Polygons->GetNumberOfCells(); i ++)
      {
      *ptr_i = this->m_Polygons->GetCellSize(i);
      ptr_i ++;
      this->m_Polygons->GetCell(i, ptr_i);
      ptr_i += this->m_Polygons->GetCellSize(i);
      }
    }

  //TriangleStrips
  if (info.triangle_strips)
    {
    igtlUint32 * ptr_i = info.triangle_strips;
    for (unsigned int i = 0; i < this->m_TriangleStrips->GetNumberOfCells(); i ++)
      {
      *ptr_i = this->m_TriangleStrips->GetCellSize(i);
      ptr_i ++;
      this->m_TriangleStrips->GetCell(i, ptr_i);
      ptr_i += this->m_TriangleStrips->GetCellSize(i);
      }
    }

  SetPolyDataInfoAttribute(&info, this);

  igtl_polydata_pack(&info, this->m_Content, IGTL_TYPE_PREFIX_NONE);
  
  igtl_polydata_free_info(&info);

  return 1;
}


int PolyDataMessage::UnpackContent()
{
  igtl_polydata_info info;

  igtl_polydata_init_info(&info);
  
  int r = 0;

  bool isUnpacked(true);
  r = igtl_polydata_unpack(IGTL_TYPE_PREFIX_NONE, (void*)this->m_Content, &info, this->CalculateReceiveContentSize(isUnpacked));

  if ( r == 0)
    {
    return 0;
    }
  
  // Points
  if (this->m_Points.IsNull())
    {
    this->m_Points = igtl::PolyDataPointArray::New();
    }
  this->m_Points->Clear();
  if (info.header.npoints > 0)
    {
    this->m_Points->SetNumberOfPoints(info.header.npoints);
    for (unsigned int i = 0; i < info.header.npoints; i ++)
      {
      this->m_Points->SetPoint(i, &(info.points[i*3]));
      }
    }

  igtlUint32 * ptr;

  // Vertices
  if (this->m_Vertices.IsNull())
    {
    this->m_Vertices = igtl::PolyDataCellArray::New();
    }
  this->m_Vertices->Clear();
  ptr = info.vertices;
  for (unsigned int i = 0; i < info.header.nvertices; i ++)
    {
    unsigned int n = *ptr;
    ptr ++;
    this->m_Vertices->AddCell(n, ptr);
    ptr += n;
    }

  // Lines
  if (this->m_Lines.IsNull())
    {
    this->m_Lines = igtl::PolyDataCellArray::New();
    }
  this->m_Lines->Clear();
  ptr = info.lines;
  for (unsigned int i = 0; i < info.header.nlines; i ++)
    {
    unsigned int n = *ptr;
    ptr ++;
    this->m_Lines->AddCell(n, ptr);
    ptr += n;
    }

  // Polygons
  if (this->m_Polygons.IsNull())
    {
    this->m_Polygons = igtl::PolyDataCellArray::New();
    }
  this->m_Polygons->Clear();
  ptr = info.polygons;
  for (unsigned int i = 0; i < info.header.npolygons; i ++)
    {
    unsigned int n = *ptr;
    ptr ++;
    this->m_Polygons->AddCell(n, ptr);
    ptr += n;
    }

  // TriangleStrips
  if (this->m_TriangleStrips.IsNull())
    {
    this->m_TriangleStrips = igtl::PolyDataCellArray::New();
    }
  this->m_TriangleStrips->Clear();
  ptr = info.triangle_strips;
  for (unsigned int i = 0; i < info.header.ntriangle_strips; i ++)
    {
    unsigned int n = *ptr;
    ptr ++;
    this->m_TriangleStrips->AddCell(n, ptr);
    ptr += n;
    }
  
  // Attributes
  this->m_Attributes.clear();
  igtl_polydata_attribute * attr = info.attributes;
  for (unsigned int i = 0; i < info.header.nattributes; i ++)
    {
    

    PolyDataAttribute::Pointer pda = PolyDataAttribute::New();
    if (pda.IsNotNull())
      {
      pda->Clear();
      pda->SetType(attr->type, attr->ncomponents);
      pda->SetSize(attr->n);
      pda->SetName(attr->name);
      pda->SetData(attr->data);
      attr ++;
      this->m_Attributes.push_back(pda);
      }
    }


  return 1;
}


void PolyDataMessage::Clear()
{

  if (this->m_Points.IsNotNull())
    {
    //this->m_Points->Delete();
    this->m_Points = NULL;
    }
  if (this->m_Vertices.IsNotNull())
    {
    //this->m_Vertices->Delete();
    this->m_Vertices = NULL;
    }
  if(this->m_Lines.IsNotNull())
    {
    //this->m_Lines->Delete();
    this->m_Lines = NULL;
    }
  if (this->m_Polygons.IsNotNull())
    {
    //this->m_Polygons->Delete();
    this->m_Polygons = NULL;
    }
  if (this->m_TriangleStrips.IsNotNull())
    {
    //this->m_TriangleStrips->Delete();
    this->m_TriangleStrips = NULL;
    }
  // TODO: is this OK?
  this->m_Attributes.clear();
}

void PolyDataMessage::ClearAttributes()
{
  std::vector<PolyDataAttribute::Pointer>::iterator iter;
  for (iter = this->m_Attributes.begin(); iter != this->m_Attributes.end(); iter ++)
    {
    *iter = NULL;
    }
  this->m_Attributes.clear();
}

void PolyDataMessage::AddAttribute(PolyDataAttribute * att)
{
  m_IsBodyPacked = false;
  this->m_Attributes.push_back(att);
}

int PolyDataMessage::GetNumberOfAttributes()
{
  return this->m_Attributes.size();
}

PolyDataAttribute * PolyDataMessage::GetAttribute(AttributeList::size_type id)
{
  if (id >= this->m_Attributes.size())
    {
    return NULL;
    }

  return this->m_Attributes[id];
}

PolyDataAttribute * PolyDataMessage::GetAttribute(const std::string& name)
{
  for(AttributeList::size_type i = 0; i < this->m_Attributes.size(); ++i)
    {
    if (this->m_Attributes[i]->GetName() == name)
      {
        return this->m_Attributes[i];
      }
    }

  return NULL;
}

PolyDataAttribute * PolyDataMessage::GetAttribute(int type)
{
  for (AttributeList::size_type i = 0; i < this->m_Attributes.size(); ++i)
  {
    if (this->m_Attributes[i]->GetType() == type)
    {
      return this->m_Attributes[i];
    }
  }

  return NULL;
}

GetPolyDataMessage::GetPolyDataMessage()
{
  this->m_SendMessageType  = "GET_POLYDATA";
}

bool RTSPolyDataMessage::GetStatus() const
{
  return m_Status == 1;
}

void RTSPolyDataMessage::SetStatus(bool status)
{
  m_IsBodyPacked = false;
  m_Status = status ? 1 : 0;
}

igtlUint64 RTSPolyDataMessage::CalculateContentBufferSize()
{
  return sizeof(igtl_uint8);
}

int RTSPolyDataMessage::PackContent()
{
  AllocateBuffer();

  igtl_uint8* content;
  // Copy data
#if OpenIGTLink_HEADER_VERSION >= 2
  content = this->m_Content;
#else
  content = this->m_Body;
#endif

  *content = m_Status;

  return 1;
}

int RTSPolyDataMessage::UnpackContent()
{
  igtl_uint8* content;

#if OpenIGTLink_HEADER_VERSION >= 2
  content = this->m_Content;
#else
  content = this->m_Body;
#endif

  this->m_Status = *content;

  return 1;
}

} // namespace igtl
