/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlNDArrayMessage.h"

#include "igtlTypes.h"

#include "igtl_header.h"
#include "igtl_ndarray.h"

// Disable warning C4996 (strncpy() may be unsafe) in Windows. 
#define _CRT_SECURE_NO_WARNINGS

#include <string.h>

namespace igtl {


ArrayBase::ArrayBase()
{
  this->m_ByteArray = NULL;
  this->m_Size.clear();
}


ArrayBase::~ArrayBase()
{

  if (this->m_ByteArray)
    {
    delete (igtlUint8 *) this->m_ByteArray;
    }
}
  
int ArrayBase::SetSize(IndexType size)
{

  if (this->m_Size == size)
    {
    // If the size of the array is same as specified,
    // do nothing
    return 1;
    }

  if (this->m_ByteArray != NULL)
    {
    delete (igtlUint8 *) this->m_ByteArray;
    }
  this->m_Size = size;

  this->m_ByteArray = (void *) new igtlUint8[GetRawArraySize()];

  if (this->m_ByteArray)
    {
    return 1;
    }
  else
    {
    this->m_Size.clear();
    return 0;
    }
}


int ArrayBase::SetArray(void * array)
{
  if (this->m_ByteArray)
    {
    memcpy(this->m_ByteArray, array, GetRawArraySize());
    return 1;
    }
  else
    {
    return 0;
    }
};


igtlUint64 ArrayBase::GetRawArraySize()
{
  return (igtlUint64) GetNumberOfElements() * (igtlUint64) GetElementSize();
}


igtlUint32 ArrayBase::GetNumberOfElements()
{
  igtlUint32 len = 1;
  IndexType::iterator iter;
  for (iter = this->m_Size.begin(); iter != this->m_Size.end(); iter ++)
    {
    len *= *iter;
    }
  return len;
}


igtlUint32 ArrayBase::Get1DIndex(IndexType index)
{
  igtlUint32 p;
  igtlUint32 step;
  igtlUint16 dim = this->m_Size.size();

  if (this->m_Size.size() != index.size())
    {
    // TODO: how to pass the error to the caller?
    return 0;
    }

  p = 0;
  step = 1;
  IndexType::iterator iter;
  for (int i = dim-1; i >= 0; i --)
    {
    p += index[i] * step;
    step *= this->m_Size[i];
    }
  igtlUint32 m = GetNumberOfElements();
  if (p <= m)
    {
    return p;
    }
  else
    {
    // TODO: how to pass the error to the caller?
    return m; 
    }
}


NDArrayMessage::NDArrayMessage():
  MessageBase()
{
  this->m_SendMessageType = "NDARRAY";
  this->m_Array = NULL;
  this->m_Type = 0;
}


NDArrayMessage::~NDArrayMessage()
{
}


int NDArrayMessage::SetArray(int type, ArrayBase * a)
{
  m_IsBodyPacked = false;
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


igtlUint64 NDArrayMessage::CalculateContentBufferSize()
{
  igtlUint64 dataSize;
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


int NDArrayMessage::PackContent()
{
  // Allocate buffer
  AllocateBuffer();

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
  delete [] s;

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
  igtl_ndarray_pack(&info, this->m_Content, IGTL_TYPE_PREFIX_NONE);

  return 1;
}


int NDArrayMessage::UnpackContent()
{
  igtl_ndarray_info info;
  bool isUnpacked(true);
  igtl_ndarray_unpack(IGTL_TYPE_PREFIX_NONE, this->m_Content, &info, this->CalculateReceiveContentSize(isUnpacked));

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
