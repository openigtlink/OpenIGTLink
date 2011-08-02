/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $HeadURL: http://svn.na-mic.org/NAMICSandBox/trunk/OpenIGTLink/Source/igtlOSUtil.h $
  Language:  C++
  Date:      $Date: 2008-12-22 19:05:42 -0500 (Mon, 22 Dec 2008) $
  Version:   $Revision: 3460 $

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


