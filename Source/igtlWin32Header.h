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
  Module:    $RCSfile: itkWin32Header.h,v $
  Language:  C++
  Date:      $Date: 2010-01-17 13:38:05 -0500 (Sun, 17 Jan 2010) $
  Version:   $Revision: 5577 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igtlWin32Header_h
#define __igtlWin32Header_h

#include "igtlConfigure.h"

// add in the Windows variants

#if defined(__CYGWIN__)
#ifndef WIN32
#define WIN32 1
#endif
#ifndef _WIN32
#define _WIN32 1
#endif
#endif

#if defined(_WIN32)
  // Include the windows header here only if requested by user code.
# if defined(IGTL_INCLUDE_WINDOWS_H)
#  include <windows.h>
   // Define types from the windows header file.
   typedef DWORD igtlWindowsDWORD;
   typedef PVOID igtlWindowsPVOID;
   typedef LPVOID igtlWindowsLPVOID;
   typedef HANDLE igtlWindowsHANDLE;
   typedef LPTHREAD_START_ROUTINE igtlWindowsLPTHREAD_START_ROUTINE;
# else
   // Define types from the windows header file.
   typedef unsigned long igtlWindowsDWORD;
   typedef void* igtlWindowsPVOID;
   typedef igtlWindowsPVOID igtlWindowsLPVOID;
   typedef igtlWindowsPVOID igtlWindowsHANDLE;
   typedef igtlWindowsDWORD (__stdcall *igtlWindowsLPTHREAD_START_ROUTINE)(igtlWindowsLPVOID);
# endif

#if ( _MSC_VER >= 1300 ) // Visual studio .NET
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4312 )
#  define igtlGetWindowLong GetWindowLongPtr
#  define igtlSetWindowLong SetWindowLongPtr
#  define igtlLONG LONG_PTR
#  define igtlGWL_WNDPROC GWLP_WNDPROC
#  define igtlGWL_HINSTANCE GWLP_HINSTANCE
#  define igtlGWL_USERDATA GWLP_USERDATA
#else // older or non-Visual studio
#  define igtlGetWindowLong GetWindowLong
#  define igtlSetWindowLong SetWindowLong
#  define igtlLONG LONG
#  define igtlGWL_WNDPROC GWL_WNDPROC
#  define igtlGWL_HINSTANCE GWL_HINSTANCE
#  define igtlGWL_USERDATA GWL_USERDATA
#endif //
#endif


#if defined(_MSC_VER)
  // Enable MSVC compiler warning messages that are useful but off by default.
# pragma warning ( default : 4263 ) /* no override, call convention differs */
  // Disable MSVC compiler warning messages that often occur in valid code.
# if !defined(IGTL_DISPLAY_WIN32_WARNINGS)
#  pragma warning ( disable : 4003 ) /* not enough actual parameters for macro */
#  pragma warning ( disable : 4097 ) /* typedef is synonym for class */
#  pragma warning ( disable : 4127 ) /* conditional expression is constant */
#  pragma warning ( disable : 4244 ) /* possible loss in conversion */
#  pragma warning ( disable : 4251 ) /* missing DLL-interface */
#  pragma warning ( disable : 4305 ) /* truncation from type1 to type2 */
#  pragma warning ( disable : 4309 ) /* truncation of constant value */
#  pragma warning ( disable : 4514 ) /* unreferenced inline function */
#  pragma warning ( disable : 4706 ) /* assignment in conditional expression */
#  pragma warning ( disable : 4710 ) /* function not inlined */
#  pragma warning ( disable : 4786 ) /* identifier truncated in debug info */
#  pragma warning ( disable : 4996 ) /* 'strncpy': This function or variable may be unsafe. */
# endif

// typename keyword in default template arguments is not accepted by
// MSVC.  This macro should only be used in such places.
# if !defined(CABLE_CONFIGURATION) && (_MSC_VER < 1310)
#  define IGTL_TYPENAME
# else
#  define IGTL_TYPENAME typename
# endif
#else
# define IGTL_TYPENAME typename
#endif

// MSVC 6.0 in release mode will warn about code it produces with its
// optimizer.  Disable the warnings specifically for this
// configuration.  Real warnings will be revealed by a debug build or
// by other compilers.
#if defined(_MSC_VER) && (_MSC_VER < 1300) && defined(NDEBUG)
# pragma warning ( disable : 4701 ) /* Variable may be used uninitialized.  */
# pragma warning ( disable : 4702 ) /* Unreachable code.  */
#endif

#if defined(__BORLANDC__)
  // Disable Borland compiler warning messages that often occur in valid code.
# if !defined(IGTL_DISPLAY_WIN32_WARNINGS)
#  pragma warn -8004 /* assigned a value that is never used */
#  pragma warn -8008 /* condition is always false */
#  pragma warn -8026 /* funcs w/class-by-value args not expanded inline */
#  pragma warn -8027 /* functions w/ do/for/while not expanded inline */
#  pragma warn -8060 /* possibly incorrect assignment */
#  pragma warn -8066 /* unreachable code */
#  pragma warn -8072 /* suspicious pointer arithmetic */
# endif
#endif

// IGTL_EXPORT can not be used
#define IGTL_EXPORT

#if (defined(_WIN32) || defined(WIN32)) && !defined(IGTLSTATIC) 
# ifdef IGTLCommon_EXPORTS
#  define IGTLCommon_EXPORT __declspec(dllexport)
# else
#  define IGTLCommon_EXPORT __declspec(dllimport)
# endif  /* IGTLCommon_EXPORT */
#else
/* unix needs nothing */
#define IGTLCommon_EXPORT 
#endif




#endif
