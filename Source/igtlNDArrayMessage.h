/*=========================================================================

  Program:   The OpenIGTLink Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink2_beta/Source/igtlNDArrayMessage.h $
  Language:  C++
  Date:      $Date: 2009-12-16 23:58:02 -0500 (Wed, 16 Dec 2009) $
  Version:   $Revision: 5466 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlNDArrayMessage_h
#define __igtlNDArrayMessage_h

#include <string>

#include "igtlObject.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"
#include "igtlTypes.h"

#define IGTL_STRING_MESSAGE_DEFAULT_ENCODING 3 /* Default encoding -- ANSI-X3.5-1968 */

namespace igtl
{

class IGTLCommon_EXPORT ArrayBase
{
public:
  typedef std::vector<igtlUint16> IndexType;

protected:
  ArrayBase();
  ~ArrayBase();

public:

  int                     SetSize(IndexType size);
  IndexType               GetSize()         { return this->m_Size;      };
  int                     GetDimension()    { return this->m_Size.size(); };

  int                     SetArray(void * array);
  igtlUint64              GetRawArraySize();
  void *                  GetRawArray()     { return this->m_ByteArray; };

protected:
  virtual int             GetElementSize() = 0;
  igtlUint32              GetNumberOfElements();
  igtlUint32              Get1DIndex(IndexType index);

private:
  IndexType               m_Size;
  void *                  m_ByteArray;

};


template <typename T>
class IGTLCommon_EXPORT Array : public ArrayBase
{
public:
  int                     SetValue(IndexType index, T value)
  {
    if (Get1DIndex(index) <= GetNumberOfElements()) {
      * (T *) this->m_ByteArray[Get1DIndex(index) * sizeof(T)] = value;
      return 1;
    } else {
      return 0;
    }
  }
  int                     GetValue(IndexType index,  T & value)
  {
    if (Get1DIndex(index) <= GetNumberOfElements()) {
      value = * (T *) this->m_ByteArray[Get1DIndex(index) * sizeof(T)];
      return 1;
    } else {
      return 0;
    }
  }

protected:
  virtual int             GetElementSize() { return sizeof(T); };
};


class IGTLCommon_EXPORT NDArrayMessage: public MessageBase
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
  typedef NDArrayMessage                 Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::NDArrayMessage, igtl::MessageBase);
  igtlNewMacro(igtl::NDArrayMessage);

public:

  int         SetArray(int type, ArrayBase * a);
  ArrayBase * GetArray() { return this->m_Array; };
  int         GetType()  { return this->m_Type; } ;

protected:
  NDArrayMessage();
  ~NDArrayMessage();
  
protected:

  virtual int  GetBodyPackSize();
  virtual int  PackBody();
  virtual int  UnpackBody();
  
  ArrayBase *  m_Array;
  int          m_Type;

};


} // namespace igtl

#endif // _igtlNDArrayMessage_h



