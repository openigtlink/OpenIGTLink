/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igtlTimeStamp_h
#define __igtlTimeStamp_h

#include "igtlMacro.h"
#include "igtlObject.h"
#include "igtlObjectFactory.h"
#include "igtlTypes.h"

#if defined(WIN32) || defined(_WIN32)
#include <ctime>
#endif

namespace igtl
{

class IGTLCommon_EXPORT TimeStamp : public Object
{
public:
  igtlTypeMacro(TimeStamp, Object);
  igtlNewMacro(Self);

  /// Gets the frequency of a clock.
  igtlGetConstMacro(Frequency,  igtlUint32);

  /// Gets the second part of the time stamp.
  igtlGetConstMacro(Second,     igtlUint32);

  /// Gets the fraction of second part of the time stamp.
  igtlGetConstMacro(Nanosecond, igtlUint32);

  /// Gets the current time from the system's clock and save it as a time stamp.
  void   GetTime();

  /// Sets the time by double floating-point value.
  void   SetTime(double tm);

  /// Sets the time by second and nanosecond
  void   SetTime(igtlUint32 second, igtlUint32 nanosecond);

  /// Sets the time by using 64-bit fixed-point expression used in OpenIGTLink.
  void   SetTime(igtlUint64 tm);

  /// Sets the time using the total number of nano-seconds since the start of time (Unix Epoch = 1 Jan 1970).
  /// Note: This is deliberately different to SetTime();
  void SetTimeInNanoseconds(igtlUint64 tm);

  /// Gets the time stamp. Returns a double floating-point value.
  double GetTimeStamp();

  /// Gets the time stamp. The second and nanosecond parts are stored in 'second' and 'nanosecond'
  void   GetTimeStamp(igtlUint32* second, igtlUint32* nanosecond);

  /// Gets the time stamp. Returns a 64-bit fixed-point expression used in OpenIGTLink.
  igtlUint64 GetTimeStampUint64();

  /// Gets the time in nano-seconds since the start of time (Unix Epoch = 1 Jan 1970).
  /// Note: This is deliberately different to GetTimeStampUint64();
  igtlUint64 GetTimeStampInNanoseconds() const;

protected:

  /** constructor */
  TimeStamp();

  /** destructor */
  virtual ~TimeStamp();

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os) const;

private:

  /// Clock frequency (Hz)
  igtlInt32       m_Frequency;

  /// Second part of the time relative to 00:00:00 January 1, 1970 UTC
  igtlInt32       m_Second;

  /// Nano-second part of the time stamp
  igtlInt32       m_Nanosecond;


#if defined(WIN32) || defined(_WIN32)
  //typedef double        TimeStampType;
  //typedef double        FrequencyType;
  //
  //FrequencyType   m_WinFrequency;
  //TimeStampType   m_WinDifference;
  //TimeStampType   m_WinOrigin;

  time_t  m_WinTimeOrigin;
  clock_t m_WinClockOrigin;

#endif

};

} // end of namespace igtl

#endif  // __igtlTimeStamp_h


