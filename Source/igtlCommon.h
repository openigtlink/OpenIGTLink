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

Program:   Open Image Guided Therapy Link Network Layer
Module:    $RCSfile: igtlCommon.h,v $
Language:  C++
Date:      $Date: 2008-12-22 19:05:42 -0500 (Mon, 22 Dec 2008) $
Version:   $Revision: 3460 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlCommon_h
#define __igtlCommon_h

#include <igtlConfigure.h>
#include <igtl_header.h>

namespace igtl
{
  /// This function contains the extrinsic knowledge of the protocol version history
  /// so that programmers don't need to know which protocol version uses which header version
  static int IGTLProtocolToHeaderLookup(int igtlProtocolVersion)
  {
    if( igtlProtocolVersion == OpenIGTLink_PROTOCOL_VERSION_1 || igtlProtocolVersion == OpenIGTLink_PROTOCOL_VERSION_2 )
    {
      return IGTL_HEADER_VERSION_1;
    }
    else if( igtlProtocolVersion == OpenIGTLink_PROTOCOL_VERSION_3)
    {
      return IGTL_HEADER_VERSION_2;
    }
    else
    {
      // TODO : which error mechanism to use for error reporting
      return IGTL_HEADER_VERSION_1;
    }
  }
}

#endif
