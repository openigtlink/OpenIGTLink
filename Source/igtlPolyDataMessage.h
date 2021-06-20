/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

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

/// A class for the GET_POLYDATA message type.
class IGTLCommon_EXPORT GetPolyDataMessage: public MessageBase
{
public:
  igtlTypeMacro(igtl::GetPolyDataMessage, igtl::MessageBase);
  igtlNewMacro(igtl::GetPolyDataMessage);

protected:
  GetPolyDataMessage();
  ~GetPolyDataMessage() {};
protected:
  igtlUint64 CalculateContentBufferSize() override { return 0; };
  int  PackContent()   override      { AllocateBuffer(); return 1; };
  int  UnpackContent()  override     { return 1; };
};

/// A class for the RTS_POLYDATA message type.
class IGTLCommon_EXPORT RTSPolyDataMessage : public MessageBase
{
public:
  igtlTypeMacro(igtl::RTSPolyDataMessage, igtl::MessageBase);
  igtlNewMacro(igtl::RTSPolyDataMessage);

  bool GetStatus() const;
  void SetStatus(bool status);

protected:
  RTSPolyDataMessage() : MessageBase() { this->m_SendMessageType = "RTS_POLYDATA"; };
  ~RTSPolyDataMessage() {};

protected:
  igtlUint64 CalculateContentBufferSize() override;
  int  PackContent() override;
  int  UnpackContent() override;

protected:
  /// Result of the previous GET_POLYDATA/POLYDATA message
  igtl_uint8 m_Status;
};

/// A class for the STP_POLYDATA message type.
class IGTLCommon_EXPORT StopPolyDataMessage: public MessageBase
{
public:
  igtlTypeMacro(igtl::StopPolyDataMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StopPolyDataMessage);
  
protected:
  StopPolyDataMessage() : MessageBase() { this->m_SendMessageType = "STP_POLYDATA"; };
  ~StopPolyDataMessage() {};
protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };
};

/// A class for the STT_POLYDATA message type.
class IGTLCommon_EXPORT StartPolyDataMessage : public MessageBase
{
public:
  igtlTypeMacro(igtl::StartPolyDataMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StartPolyDataMessage);

protected:
  StartPolyDataMessage() : MessageBase() { this->m_SendMessageType = "STT_POLYDATA"; };
  ~StartPolyDataMessage() {};
protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody() { AllocatePack(); return 1; };
  virtual int  UnpackBody() { return 1; };
};
  
  
  
// A class to manage a point array.
class IGTLCommon_EXPORT PolyDataPointArray : public Object {

 public:

  /// A vector to represent coordinates of a point.
  typedef std::vector<igtlFloat32> Point;

 public:
  igtlTypeMacro(igtl::PolyDataPointArray, igtl::Object);
  igtlNewMacro(igtl::PolyDataPointArray);
  
 protected:
  PolyDataPointArray();
  ~PolyDataPointArray();

 public:

  /// Clears the all points in the list.
  void Clear();

  /// Sets the number of points. This function will change the size of the list.
  void SetNumberOfPoints(int n);

  /// Gets the number of points in the list.
  int  GetNumberOfPoints();

  /// Substitutes the point specified by 'id' with a point specified by 'point'.
  /// The 'point' contains the x, y, and z coordinates of the point.
  int  SetPoint(unsigned int id, igtlFloat32 * point);

  /// Substitutes the point specified by 'id' with a point specified by 'x', 'y' and 'z'.
  int  SetPoint(unsigned int id, igtlFloat32 x, igtlFloat32 y, igtlFloat32 z);

  /// Adds a point specified by 'point' to the list.
  /// The 'point' contains the x, y, and z coordinates of the point.
  int  AddPoint(igtlFloat32 * point);

  /// Adds a point 'point' specified by 'x', 'y' and 'z'. 
  int  AddPoint(igtlFloat32 x, igtlFloat32 y, igtlFloat32 z);

  /// Gets the coordinates of the point specified by 'id'
  int  GetPoint(unsigned int id, igtlFloat32 & x, igtlFloat32 & y, igtlFloat32 & z);

  /// Gets the coordinates of the point specified by 'id'
  int  GetPoint(unsigned int id, igtlFloat32 * point);

  /// Implement support for C++11 ranged for loops
  igtlRangeBasedForHeaderMemberMacro(std::vector<Point>);

 private:
  /// A list of the points.
  std::vector< Point > m_Data;
};

igtlRangeBasedForHeaderExternalMacro(PolyDataPointArray, std::vector<PolyDataPointArray::Point>, IGTLCommon_EXPORT);

// The PolyDataCellArray class is used to pass vertices, lines, polygons, and triangle strips
class IGTLCommon_EXPORT PolyDataCellArray : public Object {
  
 public:

   typedef std::list<igtlUint32> Cell;

  enum {
    NULL_POINT = 0xFFFFFFFF,
  };

 public:
  igtlTypeMacro(igtl::PolyDataCellArray, igtl::Object);
  igtlNewMacro(igtl::PolyDataCellArray);

 protected:
  PolyDataCellArray();
  ~PolyDataCellArray();

 public:

  /// Clears the cell array.
  void       Clear();

  /// Gets the number of cells in the array.
  igtlUint32 GetNumberOfCells();

  /// Adds an array of cells stored in 'cell'. The number of cells is specified by 'n'.
  void       AddCell(int n, igtlUint32 * cell);

  /// Adds an array of cells stored in 'cell'.
  void       AddCell(const Cell& cell);

  /// GetTotalSize() returns the total memory size of the cell data array in
  /// POLYDATA message. Cell data array is array of cell data, consisting of
  /// <number of points> and array of <point indecies>. Both <number of points>
  /// and <point indecies> are unsigned 32-bit integer. 
  /// Consequently, the total size can be calculated by: 
  /// sizeof(igtlUint32) * (number of points for 0th cell + 1) + (number of points for 1th cell + 1)
  /// ... + (number of points for (N-1)th cell + 1). Note that this includes the first igtlUint32 value
  /// that specifies the number of points in each cell.
  igtlUint32 GetTotalSize();

  /// Gets the size of the cell specified by 'id'.
  igtlUint32 GetCellSize(unsigned int id);

  /// Gets the cell specified by the 'id'. A list of points in the cell will be stored in the
  /// 'cell'. A memory area sufficient to store the points in the cell must be allocated 
  /// before calling GetCell() function, and specified as 'cell'.
  int        GetCell(unsigned int id, igtlUint32 * cell);

  /// Gets the cell specified by the 'id'. A list of points in the cell will be stored in the 'cell'.
  int        GetCell(unsigned int id, Cell& cell);

  /// Implement support for C++11 ranged for loops
  igtlRangeBasedForHeaderMemberMacro(std::vector<Cell>);

 private:
  /// A lists of the cells. Each cell consists of multiple points. 
  std::vector<Cell> m_Data;
};

/// Implement support for C++11 ranged for loops
igtlRangeBasedForHeaderExternalMacro(PolyDataCellArray, std::vector<PolyDataCellArray::Cell>, IGTLCommon_EXPORT);

/// Attribute class used for passing attribute data.
class IGTLCommon_EXPORT PolyDataAttribute : public Object {
 public:

  /// Point and cell types.
  enum {
    POINT_SCALAR  = 0x00,
    POINT_VECTOR  = 0x01,
    POINT_NORMAL  = 0x02,
    POINT_TENSOR  = 0x03,
    POINT_RGBA    = 0x04,
    POINT_TCOORDS = 0x05,
    CELL_SCALAR   = 0x10,
    CELL_VECTOR   = 0x11,
    CELL_NORMAL   = 0x12,
    CELL_TENSOR   = 0x13,
    CELL_RGBA     = 0x14,
    CELL_TCOORDS  = 0x15
  };

 public:
  igtlTypeMacro(igtl::PolyDataAttribute, igtl::Object);
  igtlNewMacro(igtl::PolyDataAttribute);

 protected:
  PolyDataAttribute();
  ~PolyDataAttribute();

 public:

  /// Clears the attributes
  void        Clear();

  /// SetType() is used to set the attribute type. If the attribute is set properly,
  /// the function returns the type value (POINT_* or CELL_*). Otherwise
  /// the function returns negative value. The second argument will be ignored
  /// if 't' is neither POINT_SCALAR nor CELL_SCALAR.
  /// If the POINT_SCALAR and CELL_SCALAR is specified as 't', the number of
  /// components can be specified as the second argument. The number of
  /// components must be 0 < n < 128.
  int         SetType(int t, int n=1);

  /// Gets the attribute type.
  igtlUint8   GetType() { return this->m_Type; };

  /// Gets the number of components. The number depends on the type of the points/cells e.g. 
  /// 3 in case of POINT_VECTOR.
  igtlUint32  GetNumberOfComponents();

  /// Sets the size of the attribute.
  igtlUint32  SetSize(igtlUint32 size);

  /// Gets the size of the attribute.
  igtlUint32  GetSize();

  /// Sets the name of the attribute.
  void        SetName(const char * name);

  /// Gets the name of the attribute.
  const char* GetName() { return this->m_Name.c_str(); };

  /// Sets the attribute by byte array.
  int         SetData(igtlFloat32 * data);

  /// Gets the attribute as a byte array.
  int         GetData(igtlFloat32 * data);

  /// Sets the Nth data.
  int         SetNthData(unsigned int n, igtlFloat32 * data);

  /// Gets the Nth data.
  int         GetNthData(unsigned int n, igtlFloat32 * data);

  /// Gets the Nth data.
  int         GetNthData(unsigned int n, std::vector<igtlFloat32>& data);

  /// Implement support for C++11 ranged for loops
  std::vector<igtlFloat32>::iterator begin();
  std::vector<igtlFloat32>::iterator end();
  std::vector<igtlFloat32>::const_iterator begin() const;
  std::vector<igtlFloat32>::const_iterator end() const;

  std::vector<igtlFloat32>::reverse_iterator rbegin();
  std::vector<igtlFloat32>::reverse_iterator rend();
  std::vector<igtlFloat32>::const_reverse_iterator rbegin() const;
  std::vector<igtlFloat32>::const_reverse_iterator rend() const;

 private:
  /// The attribute type.
  igtlUint8                m_Type;

  /// The number of components.
  igtlUint8                m_NComponents;

  /// The size of the attribute.
  igtlUint32               m_Size;

  /// The name of the attribute.
  std::string              m_Name;

  /// The list of attributes.
  std::vector<igtlFloat32> m_Data;

};

/// Implement support for C++11 ranged for loops
IGTLCommon_EXPORT std::vector<igtlFloat32>::iterator begin(PolyDataAttribute& list);
IGTLCommon_EXPORT std::vector<igtlFloat32>::iterator end(PolyDataAttribute& list);
IGTLCommon_EXPORT std::vector<igtlFloat32>::const_iterator begin(const PolyDataAttribute& list);
IGTLCommon_EXPORT std::vector<igtlFloat32>::const_iterator end(const PolyDataAttribute& list);

IGTLCommon_EXPORT std::vector<igtlFloat32>::reverse_iterator rbegin(PolyDataAttribute& list);
IGTLCommon_EXPORT std::vector<igtlFloat32>::reverse_iterator rend(PolyDataAttribute& list);
IGTLCommon_EXPORT std::vector<igtlFloat32>::const_reverse_iterator rbegin(const PolyDataAttribute& list);
IGTLCommon_EXPORT std::vector<igtlFloat32>::const_reverse_iterator rend(const PolyDataAttribute& list);

/// A class for the POLYDATA message type.
class IGTLCommon_EXPORT PolyDataMessage: public MessageBase
{
public:
  typedef std::vector<PolyDataAttribute::Pointer> AttributeList;

  igtlTypeMacro(igtl::PolyDataMessage, igtl::MessageBase);
  igtlNewMacro(igtl::PolyDataMessage);

public:

  /// Clears the polydata.
  void Clear();

  /// Sets an array of points.
  igtlSetObjectMacro(Points,   PolyDataPointArray);

  /// Gets an array of points.
  igtlGetObjectMacro(Points,   PolyDataPointArray);

  /// Sets an array of vertices.
  igtlSetObjectMacro(Vertices, PolyDataCellArray);

  /// Gets an array of vertices.
  igtlGetObjectMacro(Vertices, PolyDataCellArray);

  /// Sets an array of lines.
  igtlSetObjectMacro(Lines,    PolyDataCellArray);

  /// Gets an array of lines.
  igtlGetObjectMacro(Lines,    PolyDataCellArray);

  /// Sets an array of polygons.
  igtlSetObjectMacro(Polygons, PolyDataCellArray);

  /// Gets an array of polygons.
  igtlGetObjectMacro(Polygons, PolyDataCellArray);

  /// Sets an array of triangle strips.
  igtlSetObjectMacro(TriangleStrips, PolyDataCellArray);

  /// Gets an array of triangle strips.
  igtlGetObjectMacro(TriangleStrips, PolyDataCellArray);

  /// Clears the attributes.
  void ClearAttributes();

  /// Adds an attribute.
  void AddAttribute(PolyDataAttribute * att);

  /// Gets the number of attributes.
  int  GetNumberOfAttributes();

  /// Gets an attribute specified by 'id'.
  PolyDataAttribute * GetAttribute(AttributeList::size_type id);

  /// Gets an attribute specified by 'name'.
  PolyDataAttribute * GetAttribute(const std::string& name);

  /// Gets an attribute specified by 'type'.
  PolyDataAttribute * GetAttribute(int type);
 
protected:
  PolyDataMessage();
  ~PolyDataMessage();

protected:
  igtlUint64 CalculateContentBufferSize() override;
  int  PackContent() override;
  int  UnpackContent() override;

  /// A pointer to the array of points.
  PolyDataPointArray::Pointer m_Points;

  /// A pointer to the array of vertices.
  PolyDataCellArray::Pointer  m_Vertices;

  /// A pointer to the array of lines.
  PolyDataCellArray::Pointer  m_Lines;

  /// A pointer to the array of polygons.
  PolyDataCellArray::Pointer  m_Polygons;

  /// A pointer to the array of triangle strips.
  PolyDataCellArray::Pointer  m_TriangleStrips;

  /// A list of pointers to the attributes.
  AttributeList m_Attributes;

};

} // namespace igtl

#endif // _igtlPolyDataMessage_h
