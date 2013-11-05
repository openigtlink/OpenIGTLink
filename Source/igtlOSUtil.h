/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igltOSUtil_h
#define __igltOSUtil_h

#include <cstring>

#include "igtlWin32Header.h"
namespace igtl
{

  /** Stop the program for the duration specified in millisecond
   *  The maximum dulation is 
   * */
  void IGTLCommon_EXPORT Sleep(int millisecond);

  /** Just in case strnlen() is not defined (e.g. Mac OS X Snow Leopard) */
#ifndef OpenIGTLink_HAVE_STRNLEN
  size_t IGTLCommon_EXPORT Strnlen(const char* s, size_t maxlen);
#else
  inline size_t IGTLCommon_EXPORT Strnlen(const char* s, size_t maxlen)
  { return strnlen(s, maxlen); }
#endif

}

#endif // __igltOSUtil_h


