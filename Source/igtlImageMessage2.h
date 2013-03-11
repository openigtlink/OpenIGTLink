/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlImageMessage2.h $
  Language:  C++
  Date:      $Date: 2011-05-06 16:55:54 -0400 (Fri, 06 May 2011) $
  Version:   $Revision: 7472 $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlImageMessage2_h
#define __igtlImageMessage2_h

#include "igtlObject.h"
//#include "igtlMacros.h"
#include "igtlMacro.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"



// Image message that supports fragmented pack
// 
// Fragmeted pack allows allocating memory for each segment independently.
// The version 1 library assumes that the memory area for entire message pack
// is allocated at once, causing extra memory copies in some applications.
// (For example, copying image from source into the pack memory)

#define  FRAGMENTED_PACK 


namespace igtl
{

class IGTLCommon_EXPORT GetImageMessage2: public MessageBase
{
public:
  typedef GetImageMessage2                Self;
  typedef MessageBase                    Superclass;
  typedef SmartPointer<Self>             Pointer;
  typedef SmartPointer<const Self>       ConstPointer;

  igtlTypeMacro(igtl::GetImageMessage2, igtl::MessageBase);
  igtlNewMacro(igtl::GetImageMessage2);

  
protected:
  GetImageMessage2() : MessageBase() { this->m_DefaultBodyType  = "GET_IMAGE"; };
  ~GetImageMessage2() {};
protected:
  virtual int  GetBodyPackSize() { return 0; };
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()      { return 1; };
};


class IGTLCommon_EXPORT ImageMessage2: public MessageBase
{
public:
  typedef ImageMessage2              Self;
  typedef MessageBase               Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  igtlTypeMacro(igtl::ImageMessage2, igtl::MessageBase)
  igtlNewMacro(igtl::ImageMessage2);

public:

  enum {
    COORDINATE_LPS,
    COORDINATE_RAS
  };

  enum {
    ENDIAN_BIG=1,
    ENDIAN_LITTLE=2
  };

  enum {
    DTYPE_SCALAR = 1,
    DTYPE_VECTOR = 3
  };

  enum {
    TYPE_INT8    = 2,
    TYPE_UINT8   = 3,
    TYPE_INT16   = 4,
    TYPE_UINT16  = 5,
    TYPE_INT32   = 6,
    TYPE_UINT32  = 7,
    TYPE_FLOAT32 = 10,
    TYPE_FLOAT64 = 11
  };


public:

  // Image dimensions.
  // SetDimensions() should be called prior to SetSubVolume(), since SetDimensions()
  // sets subvolume parameters automatically assuming that subvolume = entire volume.
  void SetDimensions(int s[3]);
  void SetDimensions(int i, int j, int k);
  void GetDimensions(int s[3]);
  void GetDimensions(int &i, int &j, int &k);

  // Sub-volume dimensions and offset.
  // SetSubVolume() should be called after calling SetDiemensions(), since SetDimensions()
  // reset the subvolume parameters automatically.
  int  SetSubVolume(int dim[3], int off[3]);
  int  SetSubVolume(int dimi, int dimj, int dimk, int offi, int offj, int offk);
  void GetSubVolume(int dim[3], int off[3]);
  void GetSubVolume(int &dimi, int &dimj, int &dimk, int &offi, int &offj, int &offk);

  // Spacings
  void SetSpacing(float s[3]);
  void SetSpacing(float si, float sj, float sk);
  void GetSpacing(float s[3]);
  void GetSpacing(float &si, float &sj, float &sk);

  // Origin coordinate
  void SetOrigin(float p[3]);
  void SetOrigin(float px, float py, float pz);
  void GetOrigin(float p[3]);
  void GetOrigin(float &px, float &py, float &pz);

  // Image orientation
  void SetNormals(float o[3][3]);
  void SetNormals(float t[3], float s[3], float n[3]);
  void GetNormals(float o[3][3]);
  void GetNormals(float t[3], float s[3], float n[3]);

  // Number of components
  void SetNumComponents(int num);
  int  GetNumComponents();

  // Origin/orientation matrix
  void SetMatrix(Matrix4x4& mat);
  void GetMatrix(Matrix4x4& mat);

  // Image scalar type
  void SetScalarType(int t)    { scalarType = t; };
  void SetScalarTypeToInt8()   { scalarType = TYPE_INT8; };
  void SetScalarTypeToUint8()  { scalarType = TYPE_UINT8; };
  void SetScalarTypeToInt16()  { scalarType = TYPE_INT16; };
  void SetScalarTypeToUint16() { scalarType = TYPE_UINT16; };
  void SetScalarTypeToInt32()  { scalarType = TYPE_INT32; };
  void SetScalarTypeToUint32() { scalarType = TYPE_UINT32; };
  int  GetScalarType()         { return scalarType; };
  int  GetScalarSize()         { return ScalarSizeTable[scalarType]; };
  int  GetScalarSize(int type) { return ScalarSizeTable[type]; };

  // Endian of image scalar (default is ENDIAN_BIG)
  void SetEndian(int e)        { endian = e; };
  int  GetEndian()             { return endian; };

  // Coordinate System: RAS=1/LPS=2
  int GetCoordinateSystem() { return coordinate;};
  void SetCoordinateSystem(int c) {coordinate = c;};

  // TBD: Should returned value be 64-bit integer?
  int  GetImageSize()
  {
    return dimensions[0]*dimensions[1]*dimensions[2]*GetScalarSize()*numComponents;
  };

  int  GetSubVolumeImageSize()
  {
    return subDimensions[0]*subDimensions[1]*subDimensions[2]*GetScalarSize()*numComponents;
  };


  // Note: If FragmentedPack is active, GetScalarPointer() causes extra memory allocation to
  // create a single pack memroy degrading the performance.
  virtual void  AllocateScalars();
  virtual void* GetScalarPointer();

#ifdef  FRAGMENTED_PACK 
  virtual void  SetScalarPointer(void * p);

  void* GetPackPointer();

  int   GetNumberOfPackFragments() { return 3;  /* header, image header and image body */ }
  void* GetPackFragmentPointer(int id);
  int   GetPackFragmentSize(int id);
#endif // FRAGMENTED_PACK 


protected:
  ImageMessage2();
  ~ImageMessage2();
  
protected:

  virtual int  GetBodyPackSize();

#ifdef FRAGMENTED_PACK  
public:  
  // Pack() serializes the header and body based on the member variables.
  // PackBody() must be implemented in the child class.
  virtual int   Pack();
public:
#endif //FRAGMENTED_PACK  

  virtual int  PackBody();
  virtual int  UnpackBody();

#ifdef FRAGMENTED_PACK  
  // Allocate memory specifying the body size
  // (used when create a brank package to receive data)
  virtual void AllocatePack(int bodySize);
#endif //FRAGMENTED_PACK  

  int    dimensions[3];
  float  spacing[3];
  int    subDimensions[3];
  int    subOffset[3];

  Matrix4x4 matrix;

  int    endian;
  int    dataType;
  int    numComponents;
  int    scalarType;
  int    coordinate;

  unsigned char*  m_ImageHeader;
  unsigned char*  m_Image;

#ifdef  FRAGMENTED_PACK 
  unsigned char*  m_SinglePack;
  int             m_SinglePackSize; 
  int             m_ImageSize;

  int             m_SelfAllocatedImage;
  int             m_SelfAllocatedImageHeader;

#endif 

  // NOTE on size parameters:
  // Following size parameters are used in the image message class:
  //   m_PackSize       : active pack size (header + body)
  //   m_ImageSize      : memory size of this->m_Image
  //   m_SinglePackSize : memory size of this->m_SinglePack


  int ScalarSizeTable[12];
};


} // namespace igtl

#endif // _igtlImageMessage2_h


