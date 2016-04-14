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

void TestDebugCharArrayCmp(void* inputPointer, unsigned char* inputArray, int size)
{
  for (int i = 0 ; i< size;++i)
  {
    std::cerr<<i<<"  "<<+((igtlUint8*)inputPointer)[i]<<"  "<<+inputArray[i]<<" "<<((igtlUint8*)inputPointer)[i]<<"  "<<inputArray[i]<<std::endl;
  }
}
#endif // __igltOSUtil_h


