/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlVideoMessage_h
#define __igtlVideoMessage_h

#include <string>

#include "igtlObject.h"
#include "igtlMacro.h"
#include "igtlMath.h"
#include "igtlMessageBase.h"

#include "igtl_header.h"
#include "igtl_video.h"

namespace igtl
{  
  /// A class for the STT_TDATA message type.
class IGTLCommon_EXPORT StartVideoMessage: public MessageBase
{
  
public:
  igtlTypeMacro(igtl::StartVideoMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StartVideoMessage);
  
public:
  /// Sets the time resolution for streaming of QTDATA messages
  void         SetTimeInterval(igtlInt32 interval)  { this->m_TimeInterval = interval; }; // ms
  
  /// Gets the time resolution for streaming of QTDATA messages
  igtlInt32    GetTimeInterval()               { return this->m_TimeInterval; };
  
  void    SetCodecType(const char codecType[]){ this->m_CodecType = std::string(codecType); };
                                                        
  std::string    GetCodecType()               { return this->m_CodecType; };
  
protected:
  StartVideoMessage() : MessageBase()
  {
    this->m_SendMessageType  = "STT_VIDEO";
    this->m_TimeInterval = 50;
    this->m_CodecType = IGTL_VIDEO_CODEC_NAME_VP9;
  };
  ~StartVideoMessage();
  
  /// Gets the size of the serialized content.
  virtual int  CalculateContentBufferSize();
  
protected:
  virtual int  GetBodyPackSize();
  virtual int  PackContent();
  virtual int  UnpackContent();
  
protected:
  
  /// codec is define here, "VP9" is the default
  /// A variable for codec protocal
  std::string   m_CodecType;
  /// Minimum time between two frames (ms). Use 0 for as fast as possible.
  igtlInt32     m_TimeInterval;
  
};
  
  
class IGTLCommon_EXPORT VideoMessageHeader: public MessageHeader
{
public:
  igtlTypeMacro(igtl::VideoMessageHeader, igtl::MessageBase);
  igtlNewMacro(igtl::VideoMessageHeader);
  virtual int  GetBodyPackSize() { return m_BodySizeToRead; };

protected:
  VideoMessageHeader() : MessageBase() { this->m_SendMessageType  = "VIDEO_HEADER"; };
  ~VideoMessageHeader() {};
protected:
  virtual int  PackBody()        { AllocatePack(); return 1; };
  virtual int  UnpackBody()  { return 1; };
};
  
  
class IGTLCommon_EXPORT StopVideoMessage: public MessageBase
{
public:
  igtlTypeMacro(igtl::StopVideoMessage, igtl::MessageBase);
  igtlNewMacro(igtl::StopVideoMessage);
  
protected:
  StopVideoMessage() : MessageBase() { this->m_SendMessageType  = "STP_VIDEO"; };
  ~StopVideoMessage() {};
protected:
  virtual int  CalculateContentBufferSize() { return 0; };
  virtual int  PackContent()        { AllocateBuffer(); return 1; };
  virtual int  UnpackContent()      { return 1; };
};

class IGTLCommon_EXPORT VideoMessage: public MessageBase
{
public:
  igtlTypeMacro(igtl::VideoMessage, igtl::MessageBase)
  igtlNewMacro(igtl::VideoMessage);

public:

  /// Endian used in the bite array for the image data.
  enum {
    ENDIAN_BIG=1,
    ENDIAN_LITTLE=2
  };

  /// Coordinate sysmtem. Either left-posterior-superior (LPS) or right-anterior-superior (RAS).
  enum {
    COORDINATE_RAS = 1,
    COORDINATE_LPS = 2
  };

  /// Pixel data type.

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
  };


public:

  /// Gets the size of the specified scalar type. (e.g. 1 byte for 8-bit integer)
  int  GetScalarSize(int type) { return ScalarSizeTable[type]; };

  /// Sets the Endianess of the image scalars. (default is ENDIAN_BIG)
  void SetEndian(int e)        { endian = e; };

  /// Gets the Endianess of the image scalars.
  int  GetEndian()             { return endian; };

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

  /// Sets the width of the image scalars.
  void SetWidth(int w)        { this->dimensions[0] = w; this->subDimensions[0] = w; };

  /// Gets the width of the image scalars.
  int  GetWidth()             { return this->dimensions[0]; };

  /// Sets the width of the image scalars.
  void SetHeight(int h)        { this->dimensions[1] = h; this->subDimensions[1] = h; };

  /// Gets the height of the image scalars.
  int  GetHeight()             { return this->dimensions[1]; };

  /// Sets the additionalZDimension of the image.
  void SetAdditionalZDimension(int zDimension)        { this->dimensions[2] = zDimension; this->subDimensions[2] = zDimension; };

  /// Gets the additionalZDimension of the image.
  int  GetAdditionalZDimension()             { return this->dimensions[2]; };

  /// Sets sub-volume dimensions and offset by arrays of the dimensions and the offset.
  /// SetSubVolume() should be called after calling SetDiemensions(), since SetDimensions()
  /// reset the subvolume parameters automatically. Returns non-zero value if the subvolume
  /// is successfully specified. Returns zero, if invalid subvolume is specified.
  int SetSubVolume(int dim[3], int off[3]);

  /// Sets sub-volume dimensions and offset by the dimensions and the offset in i, j and k
  /// directions. SetSubVolume() should be called after calling SetDiemensions(),
  /// since SetDimensions() reset the subvolume parameters automatically.
  /// Returns non-zero value if the subvolume is successfully specified. 
  /// Returns zero, if invalid subvolume is specified.
  int SetSubVolume(int dimi, int dimj, int dimk, int offi, int offj, int offk);

  /// Gets sub-volume dimensions and offset using arrays of the dimensions and the offset.
  void GetSubVolume(int dim[3], int off[3]);

  /// Gets sub-volume dimensions and offset by the dimensions and the offset in i, j and k
  /// directions.
  void GetSubVolume(int &dimi, int &dimj, int &dimk, int &offi, int &offj, int &offk);

  /// Sets the frame type of the encoded image
  void SetFrameType(int e){videoFrameType = e;}
  
  /// Gets the frame type of the encoded image
  int GetFrameType(){return videoFrameType;}
  
  /// Sets the codec type, e.g. H264, VP9, X265
  int    SetCodecType(const char codecType[]);
  
  std::string    GetCodecType()               { return this->m_CodecType; };
  
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

  /// Sets the origin/orientation matrix.
  void SetMatrix(Matrix4x4& mat);

  /// Gets the origin/orientation matrix.
  void GetMatrix(Matrix4x4& mat);

  /// This should only be called when the data is unpacked
  int  GetBitStreamSize();
  
  // Get the packed bit stream size
  int GetPackedBitStreamSize();
  
  // For memory allocation in packing process
  void SetBitStreamSize(int size);
  
  /// Allocates a memory area for the scalar data based on the dimensions of the subvolume,
  /// the number of components, and the scalar type.
  void  AllocateScalars();

  /// Gets a pointer to the scalar data (for fragmented pack support).
  void* GetPackPointer(){return GetBufferPointer();};

  /// Gets the number of fragments for the packed (serialized) data. Returns 3
  /// consisting of header, image header and image body. (for fragmented pack support)
  int   GetNumberOfPackFragments() { return 3;  /* header, image header and image body */ }

  /// Gets a pointer to the specified fragment of the packed data. (for fragmented pack support)
  unsigned char* GetPackFragmentPointer(int id);
  
  int GetPackFragmentSize(int id);

protected:
  VideoMessage();
  ~VideoMessage();
  
protected:
  
  /// Pack() serializes the header and body based on the member variables.
  /// PackBody() must be implemented in the child class. (for fragmented pack support)
  virtual int  PackContent();
  virtual int  UnpackContent();
  
  virtual int  CalculateContentBufferSize();

private:

  /// A variable for the Endian of the scalar values in the image.
  int    endian;

  /// A vector containing the numbers of voxels in i, j and k directions.
  int    dimensions[3];

  /// The frame type of the encoded image
  int  videoFrameType;
  
  /// A variable for the allocate the message body
  int    bitStreamSize;
  
  /// A pointer to the serialized image header.
  unsigned char*  m_FrameHeader;

  /// A pointer to the serialized image data.
  unsigned char*  m_Frame;

  /// A table to look up the size of a given scalar type.
  int ScalarSizeTable[12];
  
  std::string m_CodecType;

  /// A vector containing the spacings of the voxels in i, j and k directions.
  float  spacing[3];

  /// A variable for the scalar type of the voxels.
  int    coordinate;

  /// A vector containing the numbers of voxels of the subvolume in i, j and k directions.
  int    subDimensions[3];

  /// A vector containing the offset (number of voxels) of the first voxel of
  /// the subvolume from the first voxel of the original image.
  int    subOffset[3];

  /// A matrix representing the origin and the orientation of the image.
  /// The matrix is set to identity by default
  Matrix4x4 matrix;
};


} // namespace igtl

#endif // _igtlVideoMessage_h