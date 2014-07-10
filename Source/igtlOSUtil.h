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

#include "igtlTypes.h"
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

#if defined(_WIN32) && !defined(__CYGWIN__)

  /**
   * \brief Gets the time since Windows epoch (January 1, 1601) in hectonanoseconds.
   */
  igtlUint64 IGTLCommon_EXPORT GetHectoNanotime();

#endif

  /**
   * \brief Gets the system time in nanoseconds since the Unix Epoch.
   */
  igtlUint64 IGTLCommon_EXPORT GetTimeUTC();

  /**
   * \brief Gets the system time in seconds and nanoseconds since the Unix Epoch.
   */
  void IGTLCommon_EXPORT GetTimeUTC(igtlInt32 &second, igtlInt32 &nanosecond);
}

#endif // __igltOSUtil_h


