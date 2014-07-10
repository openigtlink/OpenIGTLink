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

  Part of the code is copied from itk::RealTimeClock class in
  Insight Segmentation & Registration Toolkit:


  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkRealTimeClock.cxx,v $
  Language:  C++
  Date:      $Date: 2011-03-24 00:08:23 -0400 (Thu, 24 Mar 2011) $
  Version:   $Revision: 7354 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igtlTimeStamp.h"

#include <math.h>

#if defined(WIN32) || defined(_WIN32)
  #include <windows.h>
#else
  #include <sys/time.h>
#endif  // defined(WIN32) || defined(_WIN32)

#include <string.h>

#include "igtl_util.h"
#include "igtlOSUtil.h"

namespace igtl
{

TimeStamp::TimeStamp(): Object()
{
  this->m_Frequency = 1000000;
  igtl::GetTimeUTC(m_Second, m_Nanosecond);
}


TimeStamp::~TimeStamp()
{
}


void TimeStamp::GetTime()
{
  igtl::GetTimeUTC(this->m_Second, this->m_Nanosecond);
}

void TimeStamp::SetTime(double tm)
{
  double second = floor(tm);
  this->m_Second = static_cast<igtlInt32>(second);
  this->m_Nanosecond = static_cast<igtlInt32>((tm - second)*1e9);
}

void TimeStamp::SetTime(igtlUint32 second, igtlUint32 nanosecond)
{
  if (nanosecond < 1e9)
    {
    this->m_Second = second;
    this->m_Nanosecond = nanosecond;
    }
}


void TimeStamp::SetTime(igtlUint64 tm)
{
  // Export from 64-bit fixed-point expression used in OpenIGTLink
  igtlInt32 sec      = static_cast<igtlInt32>((tm >> 32 ) & 0xFFFFFFFF);
  igtlInt32 fraction = static_cast<igtlInt32>(tm & 0xFFFFFFFF);
  this->m_Second     = sec;
  this->m_Nanosecond = igtl_frac_to_nanosec(static_cast<igtlUint32>(fraction));
}


double TimeStamp::GetTimeStamp()
{
  double tm;
  tm = static_cast<double>(this->m_Second) +
       static_cast<double>(this->m_Nanosecond) / 1e9;

  return tm;
}

void TimeStamp::GetTimeStamp(igtlUint32* second, igtlUint32* nanosecond)
{
  *second     = this->m_Second;
  *nanosecond = this->m_Nanosecond;
}
  
  
igtlUint64 TimeStamp::GetTimeStampUint64()
{
  // Export as 64-bit fixed-point expression used in OpenIGTLink
  igtlInt32 sec      = this->m_Second;
  igtlInt32 fraction = igtl_nanosec_to_frac(this->m_Nanosecond);

  igtlUint64 ts  =  sec & 0xFFFFFFFF;
  ts = (ts << 32) | (fraction & 0xFFFFFFFF);

  return ts;
}


void TimeStamp::PrintSelf( std::ostream& os) const
{
  Superclass::PrintSelf(os);

  std::string indent = "    ";

  os << indent << "Frequency of the clock: "
    << this->m_Frequency << std::endl;
  os << indent << "Second : "
    << this->m_Second << std::endl;
  os << indent << "Nanosecond : "
    << this->m_Nanosecond << std::endl;
}

}


