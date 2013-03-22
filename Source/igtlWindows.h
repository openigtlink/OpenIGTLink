/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkWindows.h,v $
  Language:  C++
  Date:      $Date: 2008-12-22 19:05:42 -0500 (Mon, 22 Dec 2008) $
  Version:   $Revision: 3460 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/** This file is used to create the smallest windows.h possible.
 * Also it removes a few annoying #define's in windows.h. */
#ifndef __igtlWindows_h
#define __igtlWindows_h

#ifndef NOMINMAX
 #define NOMINMAX
#endif

#ifdef WIN32_LEAN_AND_MEAN
 #undef WIN32_LEAN_AND_MEAN
#endif

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winbase.h>

#endif
