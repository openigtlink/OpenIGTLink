[Back to Index](/Documents/Protocol/index.md)

Time Stamp Field in Header
==========================

- Protocol Version: 3.0
- Release Date: January 20, 2017


Timestamp field summary
===================

* *Big Endian format:* bits numbered in big-endian fashion from 0 starting at the left, or high-order, position.
* *Seconds and fraction of seconds:* timestamps are represented as a 64-bit unsigned fixed-point number, in seconds relative to 00:00:00 January 1, 1970, UTC. The integer part is in the first 32 bits (Unix-style timestamp) and the fraction part in the last 32 bits. In the fraction part, the non-significant low order can be set to 0.
* *Wrap around:* The first 32-bit field will overflow some time in 2106 (second 4,294,967,296)

Obtaining timestamp
===================
* [itk::RealTimeClock](http://www.itk.org/Doxygen34/html/classitk_1_1RealTimeClock.html)
* *Linux* / *Mac*: [ftime()](http://man7.org/linux/man-pages/man3/ftime.3.html)
* *Windows*: [ftime() - 10 ms resolution](http://msdn.microsoft.com/en-us/library/z54t9z5f.aspx)
 * *Old Timestamp* - 1 sec resoultion: Now, [Time](http://msdn2.microsoft.com/en-us/library/1f4c8f33.aspx), Timer 
 * *System time* - 10 msec resolution: GetTickCount, [GetTickCount64](http://msdn2.microsoft.com/en-us/library/ms724411.aspx) or [timeGetTime()](http://msdn2.microsoft.com/en-us/library/ms713418.aspx)
 * *Highres* - hardware dependent: [QueryPerformanceCounter](http://msdn2.microsoft.com/en-us/library/ms644904.aspx) (Intel IA32 instruction: [RDTSC](http://www.intel.com/design/pentium4/manuals/245471.htm))

Time synchronization
===================
Two solutions:
0. Install NTP on all devices
0. Compute timestamp differences (local NTP can be used)

Resources
===================
* NTP4 Timestamp: [RFC 2030](http://www.faqs.org/rfcs/rfc2030.html) and [RFC 1305](http://www.faqs.org/rfcs/rfc1305.html). (See "3. NTP Timestamp Format")
* [Obtaining Accurate Timestamps under Windows XP](http://www.lochan.org/2005/keith-cl/useful/win32time.html)


