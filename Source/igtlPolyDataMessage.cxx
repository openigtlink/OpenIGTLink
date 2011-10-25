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
#include "igtl_ndarray.h"

// Disable warning C4996 (strncpy() may be unsafe) in Windows. 
#define _CRT_SECURE_NO_WARNINGS

#include <string.h>

namespace igtl {


PolyDataMessage::PolyDataMessage():
  MessageBase()
{
  this->m_DefaultBodyType = "NDARRAY";
  this->m_Array = NULL;
  this->m_Type = 0;
}


PolyDataMessage::~PolyDataMessage()
{
}


int PolyDataMessage::SetArray(int type, ArrayBase * a)
{
  // Check if type is valid
  if (type < 2 || type > 13 ||
      type == 8 || type == 9 || type == 12)
    {
    return 0;
    }
  this->m_Type = type;

  if (a)
    {
    this->m_Array = a;
    return 1;
    }
  else
    {
    return 0;
    }
}


int PolyDataMessage::GetBodyPackSize()
{
  int dataSize;
  int dim;

  if (this->m_Array == NULL)
    {
    return 0;
    }

  dim = this->m_Array->GetDimension();
  dataSize = sizeof(igtlUint8) * 2 + sizeof(igtlUint16) * (igtl_uint64) dim
    + this->m_Array->GetRawArraySize();
  return  dataSize;
}


int PolyDataMessage::PackBody()
{
  // Allocate pack
  AllocatePack();

  if (this->m_Array == NULL)
    {
    return 0;
    }

  igtl_ndarray_info info;

  igtl_ndarray_init_info(&info);
  info.dim  = this->m_Array->GetDimension();
  info.type = this->m_Type;

  ArrayBase::IndexType size = this->m_Array->GetSize();

  igtl_uint16 * s = new igtl_uint16[info.dim];
  for (int i = 0; i < info.dim; i ++)
    {
    s[i] = size[i];
    }
  int r = igtl_ndarray_alloc_info(&info, s);
  delete s;

  if (r == 0)
    {
    return 0;
    }

// size.data() doesn't work for some environtments (MacOS X 10.5 and Win32, as far as I know)
//  if (igtl_ndarray_alloc_info(&info, size.data()) == 0)
//    {
//    return 0;
//    }

  memcpy(info.array, this->m_Array->GetRawArray(), this->m_Array->GetRawArraySize());
  igtl_ndarray_pack(&info, this->m_Body, IGTL_TYPE_PREFIX_NONE);

  return 1;
}


int PolyDataMessage::UnpackBody()
{

  igtl_ndarray_info info;

  igtl_ndarray_unpack(IGTL_TYPE_PREFIX_NONE, this->m_Body, &info, this->GetPackBodySize());

  this->m_Type = info.type;
  ArrayBase::IndexType size;
  size.resize(info.dim);
  for (int i = 0; i < info.dim; i ++)
    {
    size[i] = info.size[i];
    }

  switch (this->m_Type)
    {
    case TYPE_INT8:
      this->m_Array = new Array<igtlInt8>;
      break;
    case TYPE_UINT8:
      this->m_Array = new Array<igtlUint8>;
      break;
    case TYPE_INT16:
      this->m_Array = new Array<igtlInt16>;
      break;
    case TYPE_UINT16:
      this->m_Array = new Array<igtlUint16>;
      break;
    case TYPE_INT32:
      this->m_Array = new Array<igtlInt32>;
      break;
    case TYPE_UINT32:
      this->m_Array = new Array<igtlUint32>;
      break;
    case TYPE_FLOAT32:
      this->m_Array = new Array<igtlFloat32>;
      break;
    case TYPE_FLOAT64:
      this->m_Array = new Array<igtlFloat64>;
      break;
    case TYPE_COMPLEX:
      this->m_Array = new Array<igtlComplex>;
      break;
    default:
      return 0;
      break;
    }

  this->m_Array->SetSize(size);
  memcpy(this->m_Array->GetRawArray(), info.array, this->m_Array->GetRawArraySize());
  
  return 1;
}

} // namespace igtl





