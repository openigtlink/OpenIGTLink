/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

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

  /// Vector type for an index of N-D array
  typedef std::vector<igtlUint16> IndexType;

protected:
  ArrayBase();
  ~ArrayBase();

public:

  /// Sets the size of the N-D array. Returns non-zero value, if success.
  int                     SetSize(IndexType size);

  /// Gets the size of the N-D array.
  IndexType               GetSize()         { return this->m_Size;      };

  /// Gets the dimension of the N-D array.
  int                     GetDimension()    { return this->m_Size.size(); };

  /// Sets an array from a byte array. Size and dimension must be specified prior to 
  /// calling the SetArray() function.
  int                     SetArray(void * array);

  /// Gets the size of the raw byte array stored in the class.
  igtlUint64              GetRawArraySize();

  /// Gets the raw byte array stored in the class.
  void *                  GetRawArray()     { return this->m_ByteArray; };

protected:

  /// Gets the size of a element of the array.
  virtual int             GetElementSize() = 0;

  /// Gets the number of elements in the array.
  igtlUint32              GetNumberOfElements();

  /// Returns the 1-D index of the element specified by 'index'.
  /// This function is used to calculate the index of the element in
  /// the raw array. 
  igtlUint32              Get1DIndex(IndexType index);

private:

  /// A vector representing the size of the N-D array.
  IndexType               m_Size;

  /// A pointer to the byte array data.
  void *                  m_ByteArray;

};


template <typename T>
class IGTLCommon_EXPORT Array : public ArrayBase
{
public:
  /// Sets a value of the element specified by 'index'
  int                     SetValue(IndexType index, T value)
  {
    if (Get1DIndex(index) <= GetNumberOfElements()) {
      T* TypeArray = reinterpret_cast<T*>(GetRawArray());
      TypeArray[Get1DIndex(index)] = value;
      return 1;
    } else {
      return 0;
    }
  }

  /// Gets a value of the element specified by 'index'
  int                     GetValue(IndexType index,  T & value)
  {
    if (Get1DIndex(index) <= GetNumberOfElements()) {
      T* TypeArray = reinterpret_cast<T*>(GetRawArray());
      value = TypeArray[Get1DIndex(index)];
      return 1;
    } else {
      return 0;
    }
  }

protected:

  /// Gets the size of elements (e.g. 1 byte in case of 8-bit integer)
  virtual int             GetElementSize() { return sizeof(T); };
};


class IGTLCommon_EXPORT NDArrayMessage: public MessageBase
{
public:

  /// Types of elements
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
  igtlTypeMacro(igtl::NDArrayMessage, igtl::MessageBase);
  igtlNewMacro(igtl::NDArrayMessage);

public:

  /// Sets an array with an element type.
  int         SetArray(int type, ArrayBase * a);

  /// Gets a pointer to the array.
  ArrayBase * GetArray() { return this->m_Array; };

  /// Gets the type of elements of the array. (e.g. TYPE_INT8)
  int         GetType()  { return this->m_Type; } ;

protected:
  NDArrayMessage();
  ~NDArrayMessage();
  
protected:

  igtlUint64 CalculateContentBufferSize() override;
  int  PackContent() override;
  int  UnpackContent() override;
  
  /// A pointer to the N-D array.
  ArrayBase *  m_Array;

  /// A variable for the type of the N-D array.
  int          m_Type;

};


} // namespace igtl

#endif // _igtlNDArrayMessage_h



