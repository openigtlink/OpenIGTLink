/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igltMessageDebugFunction_h
#define __igltMessageDebugFunction_h

#define ABS_ERROR 1e-6

void TestDebugCharArrayCmp(void* inputPointer, unsigned char* inputArray, int size)
{
  for (int i = 0 ; i< size;++i)
  {
    std::cerr<<i<<"  "<<+((igtlUint8*)inputPointer)[i]<<"  "<<+inputArray[i]<<" "<<((igtlUint8*)inputPointer)[i]<<"  "<<inputArray[i];
    if (((igtlUint8*)inputPointer)[i] != inputArray[i])
    {
      std::cerr<<"    "<<+((igtlUint8*)inputPointer)[i]<<"  "<<+inputArray[i];
    }
    std::cerr<<std::endl;
  }
}

bool MatrixComparison(float outMatrix[4][4], float inMatrix[4][4], float tol )
{
  bool equal = true;
  tol = tol > +0.0f ? tol:-tol;
  for(int i=0;i < 4; i++)
  {
    for(int j=0;j < 4; j++)
    {
      if((outMatrix[i][j] - inMatrix[i][j]) > tol ||
         (outMatrix[i][j] - inMatrix[i][j]) < -tol)
      {
        equal = false;
      }
    }
  }
  return equal;
}

bool ArrayFloatComparison(float returnArray[], float inArray[], int size, float tol)
{
  bool equal = true;
  tol = tol > +0.0f ? tol:-tol;
  for(int j=0;j < size; j++)
  {
    if((returnArray[j] - inArray[j]) > tol ||
       (returnArray[j] - inArray[j]) < -tol)
    {
      equal = false;
    }
  }
  return equal;
}
#endif // __igltOSUtil_h


