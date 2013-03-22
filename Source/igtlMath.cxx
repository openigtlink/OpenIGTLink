/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <math.h>
#include "igtlWin32Header.h"
#include "igtlMath.h"

namespace igtl {

void IGTLCommon_EXPORT PrintMatrix(igtl::Matrix4x4 &matrix)
{
  std::cout << "=============" << std::endl;
  std::cout << matrix[0][0] << ", " << matrix[0][1] << ", " << matrix[0][2] << ", " << matrix[0][3] << std::endl;
  std::cout << matrix[1][0] << ", " << matrix[1][1] << ", " << matrix[1][2] << ", " << matrix[1][3] << std::endl;
  std::cout << matrix[2][0] << ", " << matrix[2][1] << ", " << matrix[2][2] << ", " << matrix[2][3] << std::endl;
  std::cout << matrix[3][0] << ", " << matrix[3][1] << ", " << matrix[3][2] << ", " << matrix[3][3] << std::endl;
  std::cout << "=============" << std::endl;
}

void IGTLCommon_EXPORT PrintVector3(float v[3])
{
  std::cout << v[0] << ", " << v[1] << ", " << v[2] << std::endl;
}

void IGTLCommon_EXPORT PrintVector3(float x, float y, float z)
{
  std::cout << x << ", " << y << ", " << z << std::endl;
}

void IGTLCommon_EXPORT PrintVector4(float v[4])
{
  std::cout << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << std::endl;
}

void IGTLCommon_EXPORT PrintVector4(float x, float y, float z, float w)
{
  std::cout << x << ", " << y << ", " << z << ", " << w << std::endl;
}

void IGTLCommon_EXPORT QuaternionToMatrix(float* q, Matrix4x4& m)
{

  // normalize
  float mod = sqrt(q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3]);

  // convert to the matrix
  const float x = q[0] / mod;
  const float y = q[1] / mod; 
  const float z = q[2] / mod; 
  const float w = q[3] / mod;
  
  const float xx = x * x * 2.0;
  const float xy = x * y * 2.0;
  const float xz = x * z * 2.0;
  const float xw = x * w * 2.0;
  const float yy = y * y * 2.0;
  const float yz = y * z * 2.0;
  const float yw = y * w * 2.0;
  const float zz = z * z * 2.0;
  const float zw = z * w * 2.0;
  
  m[0][0] = 1.0 - (yy + zz);
  m[1][0] = xy + zw;
  m[2][0] = xz - yw;
  
  m[0][1] = xy - zw;
  m[1][1] = 1.0 - (xx + zz);
  m[2][1] = yz + xw;
  
  m[0][2] = xz + yw;
  m[1][2] = yz - xw;
  m[2][2] = 1.0 - (xx + yy);

  m[3][0] = 0.0;
  m[3][1] = 0.0;
  m[3][2] = 0.0;
  m[3][3] = 1.0;

  m[0][3] = 0.0;
  m[1][3] = 0.0;
  m[2][3] = 0.0;

}


void IGTLCommon_EXPORT MatrixToQuaternion(Matrix4x4& m, float* q)
{
  float trace = m[0][0] + m[1][1] + m[2][2];

  if( trace > 0.0 ) {

    float s = 0.5f / sqrt(trace + 1.0f);

    q[3] = 0.25f / s;
    q[0] = ( m[2][1] - m[1][2] ) * s;
    q[1] = ( m[0][2] - m[2][0] ) * s;
    q[2] = ( m[1][0] - m[0][1] ) * s;

  } else {

    if ( m[0][0] > m[1][1] && m[0][0] > m[2][2] ) {

      float s = 2.0f * sqrt( 1.0f + m[0][0] - m[1][1] - m[2][2]);

      q[3] = (m[2][1] - m[1][2] ) / s;
      q[0] = 0.25f * s;
      q[1] = (m[0][1] + m[1][0] ) / s;
      q[2] = (m[0][2] + m[2][0] ) / s;

    } else if (m[1][1] > m[2][2]) {

      float s = 2.0f * sqrt( 1.0f + m[1][1] - m[0][0] - m[2][2]);

      q[3] = (m[0][2] - m[2][0] ) / s;
      q[0] = (m[0][1] + m[1][0] ) / s;
      q[1] = 0.25f * s;
      q[2] = (m[1][2] + m[2][1] ) / s;

    } else {

      float s = 2.0f * sqrt( 1.0f + m[2][2] - m[0][0] - m[1][1] );

      q[3] = (m[1][0] - m[0][1] ) / s;
      q[0] = (m[0][2] + m[2][0] ) / s;
      q[1] = (m[1][2] + m[2][1] ) / s;
      q[2] = 0.25f * s;

    }
  }
}
  

  
void IGTLCommon_EXPORT Cross(float *a, float *b, float *c)
{
    a[0] = b[1]*c[2] - c[1]*b[2];
    a[1] = c[0]*b[2] - b[0]*c[2];
    a[2] = b[0]*c[1] - c[0]*b[1];
}

void IGTLCommon_EXPORT IdentityMatrix(igtl::Matrix4x4 &matrix)
{
  matrix[0][0] = 1.0;
  matrix[1][0] = 0.0;
  matrix[2][0] = 0.0;
  matrix[3][0] = 0.0;

  matrix[0][1] = 0.0;
  matrix[1][1] = 1.0;
  matrix[2][1] = 0.0;
  matrix[3][1] = 0.0;

  matrix[0][2] = 0.0;
  matrix[1][2] = 0.0;
  matrix[2][2] = 1.0;
  matrix[3][2] = 0.0;

  matrix[0][3] = 0.0;
  matrix[1][3] = 0.0;
  matrix[2][3] = 0.0;
  matrix[3][3] = 1.0;
}

} // namespace igtl
