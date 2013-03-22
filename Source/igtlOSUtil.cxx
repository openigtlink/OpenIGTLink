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
#endif

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
  
  nanosleep(&req, NULL);
  
#endif
  
}

}
