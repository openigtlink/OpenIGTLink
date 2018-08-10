/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

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


#define  FRAGMENTED_PACK 


namespace igtl
{


/// A class for the GET_IMAGE message type. The difference between the ImageMessage and
/// ImageMessage2 classes is that the ImageMessage2 class supports fragmented pack.
/// Fragmeted pack allows allocating memory for each segment independently.
/// The version 1 library assumes that the memory area for entire message pack
/// is allocated at once, causing extra memory copies in some applications.
/// (For example, copying image from source into the pack memory)
class IGTLCommon_EXPORT GetImageMessage2: public MessageBase
{
public:
  igtlTypeMacro(igtl::GetImageMessage2, igtl::MessageBase);
  igtlNewMacro(igtl::GetImageMessage2);

protected:
  GetImageMessage2() : MessageBase() { this->m_SendMessageType  = "GET_IMAGE"; };
  ~GetImageMessage2() {};
protected:
  virtual int  CalculateContentBufferSize() { return 0; };
  virtual int  PackContent()        { AllocateBuffer(); return 1; };
  virtual int  UnpackContent()      { return 1; };
};


/// A class for the IMAGE message type with support for fragmented packing.
/// The IMAGE format supports 2D or 3D images with metric information including
/// image matrix size, voxel size, coordinate system type, position, and orientation.
/// The body section of the IMAGE data consists of two parts: image header to transfer
/// the metric information and image body to transfer the array of pixel or voxel values.
/// The data type of pixel or voxel can be either scalar or vector, and numerical values
/// can be 8-, 16-, 32-bit integer, or 32- or 64-bit floating point. The pixel values
/// can be either big-endian or little-endian, since the sender software can specify
/// the byte order in the image header. The format also supports "partial image transfer",
/// in which a region of the image is transferred instead of the whole image. This mechanism
/// is suitable for real-time applications, in which images are updated region-by-region.
/// The sub-volume must be box-shaped and defined by 6 parameters consisting of the indices
/// for the corner voxel of the sub-volume and matrix size of the sub-volume.
///
/// The difference between the ImageMessage and
/// ImageMessage2 classes is that the ImageMessage2 class supports fragmented pack.
/// Fragmeted pack allows allocating memory for each segment independently.
/// The version 1 library assumes that the memory area for entire message pack
/// is allocated at once, causing extra memory copies in some applications.
/// (For example, copying image from source into the pack memory)
class IGTLCommon_EXPORT ImageMessage2: public MessageBase
{
public:
  igtlTypeMacro(igtl::ImageMessage2, igtl::MessageBase)
  igtlNewMacro(igtl::ImageMessage2);

public:

  /// Coordinate system. Either left-posterior-superior (LPS) or right-anterior-superior (RAS).
  enum {
    COORDINATE_RAS=1,
    COORDINATE_LPS=2
  };

  /// Endian used in the bite array for the image data.
  enum {
    ENDIAN_BIG=1,
    ENDIAN_LITTLE=2
  };

  /// Pixel data type either scalar or vector. 
  enum {
    DTYPE_SCALAR = 1,
    DTYPE_VECTOR = 3
  };

  /// Pixel data type.
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

  /// Sets image dimensions by an array of the numbers of pixels in i, j and k directions.
  /// SetDimensions() should be called prior to SetSubVolume(), since SetDimensions()
  /// sets subvolume parameters automatically assuming that subvolume = entire volume.
  void SetDimensions(int s[3]);

  /// Sets image dimensions by the numbers of pixels in i, j and k directions.
  /// SetDimensions() should be called prior to SetSubVolume(), since SetDimensions()
  /// sets subvolume parameters automatically assuming that subvolume = entire volume.
  void SetDimensions(int i, int j, int k);

  /// Gets image dimensions as an array of the numbers of pixels in i, j and k directions.
  void GetDimensions(int s[3]);

  /// Gets image dimensions as the numbers of pixels in i, j and k directions.
  void GetDimensions(int &i, int &j, int &k);

  /// Sets sub-volume dimensions and offset by arrays of the dimensions and the offset.
  /// SetSubVolume() should be called after calling SetDiemensions(), since SetDimensions()
  /// reset the subvolume parameters automatically. Returns non-zero value if the subvolume
  /// is successfully specified. Returns zero, if invalid subvolume is specified.
  int  SetSubVolume(int dim[3], int off[3]);

  /// Sets sub-volume dimensions and offset by the dimensions and the offset in i, j and k
  /// directions. SetSubVolume() should be called after calling SetDiemensions(),
  /// since SetDimensions() reset the subvolume parameters automatically.
  /// Returns non-zero value if the subvolume is successfully specified. 
  /// Returns zero, if invalid subvolume is specified.
  int  SetSubVolume(int dimi, int dimj, int dimk, int offi, int offj, int offk);

  /// Gets sub-volume dimensions and offset using arrays of the dimensions and the offset.
  void GetSubVolume(int dim[3], int off[3]);

  /// Gets sub-volume dimensions and offset by the dimensions and the offset in i, j and k
  /// directions.
  void GetSubVolume(int &dimi, int &dimj, int &dimk, int &offi, int &offj, int &offk);

  /// Sets spacings by an array of spacing values in i, j and k directions.
  void SetSpacing(float s[3]);

  /// Sets spacings by spacing values in i, j and k directions.
  void SetSpacing(float si, float sj, float sk);

  /// Gets spacings using an array of spacing values in i, j and k directions.
  void GetSpacing(float s[3]);

  /// Gets spacings using spacing values in i, j and k directions.
  void GetSpacing(float &si, float &sj, float &sk);

  /// Sets the coordinates of the origin by an array of positions along the first (R or L),
  /// second (A or P) and the third (S) axes.
  void SetOrigin(float p[3]);

  /// Sets the coordinates of the origin by positions along the first (R or L), second (A or P)
  /// and the third (S) axes.
  void SetOrigin(float px, float py, float pz);

  /// Gets the coordinates of the origin using an array of positions along the first (R or L),
  /// second (A or P) and the third (S) axes.
  void GetOrigin(float p[3]);

  /// Gets the coordinates of the origin by positions along the first (R or L), second (A or P)
  /// and the third (S) axes.
  void GetOrigin(float &px, float &py, float &pz);

  /// Sets the orientation of the image by an array of the normal vectors for the i, j
  /// and k indeces.
  void SetNormals(float o[3][3]);

  /// Sets the orientation of the image by the normal vectors for the i, j and k indeces.
  void SetNormals(float t[3], float s[3], float n[3]);

  /// Gets the orientation of the image using an array of the normal vectors for the i, j
  /// and k indeces.
  void GetNormals(float o[3][3]);

  /// Gets the orientation of the image using the normal vectors for the i, j and k indeces.
  void GetNormals(float t[3], float s[3], float n[3]);

  /// Sets the number of components for each voxel.
  void SetNumComponents(int num);

  /// Gets the number of components for each voxel.
  int  GetNumComponents();

  /// Sets the origin/orientation matrix.
  void SetMatrix(Matrix4x4& mat);

  /// Gets the origin/orientation matrix.
  void GetMatrix(Matrix4x4& mat);

  /// Sets the image scalar type.
  void SetScalarType(int t)    { scalarType = t; };

  /// Sets the image scalar type to 8-bit integer.
  void SetScalarTypeToInt8()   { scalarType = TYPE_INT8; };

  /// Sets the image scalar type to unsigned 8-bit integer.
  void SetScalarTypeToUint8()  { scalarType = TYPE_UINT8; };

  /// Sets the image scalar type to 16-bit integer.
  void SetScalarTypeToInt16()  { scalarType = TYPE_INT16; };

  /// Sets the image scalar type to unsigned 16-bit integer.
  void SetScalarTypeToUint16() { scalarType = TYPE_UINT16; };

  /// Sets the image scalar type to 32-bit integer.
  void SetScalarTypeToInt32()  { scalarType = TYPE_INT32; };

  /// Sets the image scalar type to unsigned 32-bit integer.
  void SetScalarTypeToUint32() { scalarType = TYPE_UINT32; };

  /// Gets the image scalar type.
  int  GetScalarType()         { return scalarType; };

  /// Gets the size of the scalar type used in the current image data.
  /// (e.g. 1 byte for 8-bit integer)
  int  GetScalarSize()         { return ScalarSizeTable[scalarType]; };

  /// Gets the size of the specified scalar type. (e.g. 1 byte for 8-bit integer)
  int  GetScalarSize(int type) { return ScalarSizeTable[type]; };

  /// Sets the Endianess of the image scalars. (default is ENDIAN_BIG)
  void SetEndian(int e)        { endian = e; };

  /// Gets the Endianess of the image scalars.
  int  GetEndian()             { return endian; };

  /// Gets the size (length) of the byte array for the image data.
  /// The size is defined by dimensions[0]*dimensions[1]*dimensions[2]*scalarSize*numComponents.
  // TODO: Should returned value be 64-bit integer?
  int  GetImageSize()
  {
    return dimensions[0]*dimensions[1]*dimensions[2]*GetScalarSize()*numComponents;
  };

  /// Returns coordinate system (COORDINATE_RAS or COORDINATE_LPS)
  int GetCoordinateSystem() { return coordinate;};

  /// Sets coordinate system (COORDINATE_RAS or COORDINATE_LPS)
  void SetCoordinateSystem(int c) {coordinate = c;};

  /// Gets the size (length) of the byte array for the subvolume image data.
  /// The size is defined by subDimensions[0]*subDimensions[1]*subDimensions[2]*
  /// scalarSize*numComponents.
  int  GetSubVolumeImageSize()
  {
    return subDimensions[0]*subDimensions[1]*subDimensions[2]*GetScalarSize()*numComponents;
  };


  /// Allocates a memory area for the scalar data based on the dimensions of the subvolume,
  /// the number of components, and the scalar type.
  /// Note: If FragmentedPack is active, GetScalarPointer() causes extra memory allocation to
  /// create a single pack memroy degrading the performance.
  virtual void  AllocateScalars();

  /// Gets a pointer to the scalar data.
  virtual void* GetScalarPointer();

#ifdef  FRAGMENTED_PACK 
  /// Sets the pointer to the scalar data (for fragmented pack support).
  virtual void  SetScalarPointer(void * p);

  /// Gets a pointer to the scalar data (for fragmented pack support).
  void* GetBufferPointer();

  /// Gets the number of fragments for the packed (serialized) data. Returns 3
  /// consisting of header, image header and image body. (for fragmented pack support)
  int   GetNumberOfPackFragments() { return 3;  /* header, image header and image body */ }

  /// Gets a pointer to the specified fragment of the packed data. (for fragmented pack support)
  void* GetPackFragmentPointer(int id);

  /// Gets the size of the specified fragment. (for fragmented pack support)
  int   GetPackFragmentSize(int id);
#endif // FRAGMENTED_PACK 


protected:
  ImageMessage2();
  ~ImageMessage2();
  
protected:

  virtual int  CalculateContentBufferSize();

#ifdef FRAGMENTED_PACK  
public:  
  /// Pack() serializes the header and body based on the member variables.
  /// PackBody() must be implemented in the child class. (for fragmented pack support)
  virtual int   Pack();
public:
#endif //FRAGMENTED_PACK  

  virtual int  PackContent();
  virtual int  UnpackContent();

#ifdef FRAGMENTED_PACK  
  /// Allocate memory specifying the body size
  /// (used when create a brank package to receive data) (for fragmented pack support)
  virtual void AllocateBuffer(int contentSize);
#endif //FRAGMENTED_PACK  

  /// A vector containing the numbers of voxels in i, j and k directions.
  int    dimensions[3];

  /// A vector containing the spacings of the voxels in i, j and k directions.
  float  spacing[3];

  /// A vector containing the numbers of voxels of the subvolume in i, j and k directions.
  int    subDimensions[3];

  /// A vector containing the offset (number of voxels) of the first voxel of
  /// the subvolume from the first voxel of the original image.
  int    subOffset[3];

  /// A matrix representing the origin and the orientation of the image.
  /// The matrix is set to identity by default
  Matrix4x4 matrix;

  /// A variable for the Endian of the scalar values in the image.
  int    endian;

  /// A variable for the number of components.
  int    numComponents;

  /// A variable for the scalar type of the voxels.
  int    scalarType;

  /// A variable for the scalar type of the voxels.
  int    coordinate;

  /// A pointer to the serialized image header.
  unsigned char*  m_ImageHeader;

  /// A pointer to the serialized image data.
  unsigned char*  m_Image;

#ifdef  FRAGMENTED_PACK 
  /// A pointer to the serialized data (for backward compatibility with the conventional packing)
  unsigned char*  m_SinglePack;

  /// A variable for the memory size of this->m_SinglePack (for backward compatibility with the
  /// conventional packing)
  int             m_SinglePackSize; 
  /// A variable for the memory size of this->m_Image (for backward compatibility with the
  /// conventional packing)
  int             m_ImageSize;

  /// A flag for the fragmented image pack (allocated by this class).
  int             m_SelfAllocatedImage;

  /// A flag for the fragmented image pack (allocated by this class).
  int             m_SelfAllocatedImageHeader;

#endif 

  /// A table to look up the size of a given scalar type.
  int ScalarSizeTable[12];
};


} // namespace igtl

#endif // _igtlImageMessage2_h