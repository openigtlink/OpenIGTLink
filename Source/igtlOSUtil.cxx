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
  #include <windows.h>
#else
  #include <time.h>
  #include <errno.h>
#endif

#include <string.h>

namespace igtl
{

void Sleep(int milliseconds)
{
#if defined(_WIN32) && !defined(__CYGWIN__)

  // Call Windows Native Sleep() function
  ::Sleep(milliseconds);
  
#else
  
  struct timespec req;
  req.tv_sec  = (int) milliseconds / 1000;
  req.tv_nsec = (milliseconds % 1000) * 1000000;

  while ((nanosleep(&req, &req) == -1) && (errno == EINTR))
  {
    continue;
  }
  
#endif
}



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

  
}
