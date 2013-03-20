/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igltOSUtil_h
#define __igltOSUtil_h

#include "igtlWin32Header.h"
namespace igtl
{

  /** Stop the program for the duration specified in millisecond
   *  The maximum dulation is 
   * */
  void IGTLCommon_EXPORT Sleep(int millisecond);

}

#endif // __igltOSUtil_h


