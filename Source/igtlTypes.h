/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlTypes_H
#define __igtlTypes_H

#include "igtlTypeConfig.h"

/* 8-bit integer type */
#if IGTL_SIZEOF_CHAR == 1
  typedef unsigned char igtlUint8;
  typedef char          igtlInt8;
#else
  # error "No native data type can represent an 8-bit integer."
#endif

/* 16-bit integer type */
#if IGTL_SIZEOF_SHORT == 2
  typedef unsigned short igtlUint16;
  typedef signed short   igtlInt16;
#elif IGTL_SIZEOF_INT == 2
  typedef unsigned int   igtlUint16;
  typedef signed int     igtlInt16;
#else
  # error "No native data type can represent a 16-bit integer."
#endif

/* 32-bit integer type */
#if IGTL_SIZEOF_INT == 4
  typedef unsigned int   igtlUint32;
  typedef signed int     igtlInt32;
#elif IGTL_SIZEOF_LONG == 4
  typedef unsigned long  igtlUint32;
  typedef signed long    igtlInt32;
#else
  # error "No native data type can represent a 32-bit integer."
#endif

/* 64-bit integer type */
#if defined(IGTL_TYPE_USE_LONG_LONG) && IGTL_SIZEOF_LONG_LONG == 8
  typedef unsigned long long igtlUint64;
  typedef signed long long   igtlInt64;
#elif IGTL_SIZEOF_INT == 8
  typedef unsigned int       igtlUint64;
  typedef signed int         igtlInt64;
#elif IGTL_SIZEOF_LONG == 8
  typedef unsigned long      igtlUint64;
  typedef signed long        igtlInt64;
#elif defined(IGTL_TYPE_USE___INT64) && IGTL_SIZEOF___INT64 == 8
  typedef unsigned __int64   igtlUint64;
  typedef signed __int64     igtlInt64;
#elif defined(IGTL_TYPE_USE_INT64_T) && IGTL_SIZEOF_INT64_T == 8
  typedef unsigned int64_t   igtlUint64;
  typedef signed int64_t     igtlInt64;
#else
  # error "No native data type can represent a 64-bit integer."
#endif

/* 32-bit floating point type */
#if IGTL_SIZEOF_FLOAT == 4
  typedef float              igtlFloat32;
#else
# error "No native data type can represent a 32-bit floating point value."
#endif

/* 64-bit floating point type */
#if IGTL_SIZEOF_DOUBLE == 8
  typedef double             igtlFloat64;
#else
# error "No native data type can represent a 64-bit floating point value."
#endif

/* 128-bit complex type */
typedef double igtlComplex[2];


#endif /* __igtlTypes_H */
