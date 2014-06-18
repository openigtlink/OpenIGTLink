/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlMath_h
#define __igtlMath_h

#include "igtlWin32Header.h"

namespace igtl
{

typedef float  Matrix4x4[4][4];

/// Prints out a 4-by-4 matrix on the standard output.
void IGTLCommon_EXPORT PrintMatrix(igtl::Matrix4x4 &matrix);

/// Prints out a 3-element vector on the standard output. 
void IGTLCommon_EXPORT PrintVector3(float v[3]);

/// Prints out a 3-element vector on the standard output.
void IGTLCommon_EXPORT PrintVector3(float x, float y, float z);

/// Prints out a 4-element vector on the standard output. 
void IGTLCommon_EXPORT PrintVector4(float v[4]);

/// Prints out a 4-element vector on the standard output. 
void IGTLCommon_EXPORT PrintVector4(float x, float y, float z, float w);

/// Converts a quaternion to a 3-by-3 rotation matrix. Only the 3-by-3 rotation matrix part
/// in the 4-by-4 matrix is used.
void IGTLCommon_EXPORT QuaternionToMatrix(float* q, Matrix4x4& m);

/// Converts a 3-by-3 rotation matrix a quaternion. Only the 3-by-3 rotation matrix part
/// in the 4-by-4 matrix is used.
void IGTLCommon_EXPORT MatrixToQuaternion(Matrix4x4& m, float* q);

/// Calculates the cross products of 3-element vectors 'b' and 'c'. The result is substituted into
/// a 3-element vector a.
void IGTLCommon_EXPORT Cross(float *a, float *b, float *c);

/// Initialize a 4-by-4 matrix as an identity matrix.
void IGTLCommon_EXPORT IdentityMatrix(igtl::Matrix4x4 &matrix);

}

#endif // __igtlMath_h



