/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlOSUtil.h"

#if defined(_WIN32) && !defined(__CYGWIN__)
  #include <ctime>
  #include <windows.h>
#else
  #include <sys/time.h>
  #include <time.h>
#endif

#include <string.h>

namespace igtl
{

//-----------------------------------------------------------------------------
void Sleep(int milliseconds)
{
#if defined(_WIN32) && !defined(__CYGWIN__)

  // Call Windows Native Sleep() function
  ::Sleep(milliseconds);
  
#else
  
  struct timespec req;
  req.tv_sec  = (int) milliseconds / 1000;
  req.tv_nsec = (milliseconds % 1000) * 1000000;
  
  nanosleep(&req, NULL);
  
#endif
}


//-----------------------------------------------------------------------------
// strnlen(), if not defined
#ifndef OpenIGTLink_HAVE_STRNLEN
size_t Strnlen(const char* s, size_t maxlen)
{

  size_t i;
 
  for(i = 0; i < maxlen; i++) {
    if(s[i] == '\0') return i;
  }
  
  return maxlen;

}
#endif


//-----------------------------------------------------------------------------
#if defined(_WIN32) && !defined(__CYGWIN__)

igtlUint64 GetHectoNanotime()
{
  FILETIME fileTime;
  GetSystemTimeAsFileTime(&fileTime);

  ULONGLONG hectoNanoSeconds = fileTime.dwHighDateTime;
  hectoNanoSeconds <<= 32;
  hectoNanoSeconds |= fileTime.dwLowDateTime;

  return hectoNanoSeconds;
}
#endif


//-----------------------------------------------------------------------------
igtlUint64 GetTimeUTC()
{
#if defined(_WIN32) && !defined(__CYGWIN__)

  // This call returns the time elapsed since the Windows epoch (January 1, 1601) in hectonanoseconds
  igtlUint64 curTime = GetHectoNanotime();

  // Need to convert this to Unix time (UTC, epoch January 1, 1970), so subtract 11644473600 seconds
  // (369 years, 89 of which are leap years = 134774 days)
  curTime -= 116444736000000000ui64; // Offset in hectonanosec.

  // Convert to nanosecs.
  curTime *= 100;

#else

  struct timeval tval;

  // Gets the system time.
  gettimeofday( &tval, 0 );

  igtlUint64 nanoSeconds = tval.tv_usec * 1e3;
  igtlUint64 seconds = tval.tv_sec *1e9;
  igtlUint64 curTime = seconds + nanoSeconds;
  

#endif // defined(_WIN32) && !defined(__CYGWIN__)

  return curTime;
}


//-----------------------------------------------------------------------------
void IGTLCommon_EXPORT GetTimeUTC(igtlInt32 &second, igtlInt32 &nanosecond)
{
  igtlUint64 curTime = GetTimeUTC();
  igtlUint64 sec = curTime / 1e9;
  second = static_cast<igtlInt32>(sec);
  nanosecond = static_cast<igtlInt32>(curTime - (sec * 1e9));
}

//-----------------------------------------------------------------------------
} // end namespace
