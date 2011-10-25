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

//
//
//

class IGTLCommon_EXPORT Cell{
  public:
    void Clear() {
      this->m_Size.clear();
      this->m_Data.clear();
    }
  private:
    std::list<igtlUint32> m_Size;
    std::list<igtlUint32> m_Data;
};

  class IGTLCommon_EXPORT Attribute{
  public:
    void Clear()
    {
      
    }
  private:
    igtlUint8              m_Type;
    igtlUint8              m_NComponents;
    std::string            m_Name;
    std::list<igtlUint32>  m_Data;
  } Attribute;



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

  // POLYDATA parameters
  void ClearPoints();
  void SetPoints(std::list<igtlFloat32> points);
  void SetPoints(int n, igtlFloat32 * points);
  void SetNumberOfPoints(int n);
  void SetPoint(int id, igtlFloat32 * point);
  void SetPoint(int id, igtlFloat32 x, igtlFloat32 y, igtlFloat32 z);
  void AddPoint(igtlFloat32 * point);
  void AddPoint(igtlFloat32 x, igtlFloat32 y, igtlFloat32 z);

  int  GetNumberOfPoints();
  int  GetPoint(int id, igtlFloat32 & x, igtlFloat32 & y, igtlFloat32 & z);
  igtlFloat32 * GetPoint(int id);

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

  std::list<Attribute>    m_Attributes;
};

} // namespace igtl

#endif // _igtlPolyDataMessage_h



