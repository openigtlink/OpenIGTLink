/*=========================================================================

  Program:   OpenIGTLink Library
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <igtlTimeStamp.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#include <iostream>

int main(int, char * [] )
{
  // Simply testing Setter/Getter
  igtl::TimeStamp::Pointer ts = igtl::TimeStamp::New();
  ts->GetTime();

  igtlUint64 totalNanosSinceEpochBefore = ts->GetTimeStampInNanoseconds();
  ts->SetTime(0.0);
  ts->SetTimeInNanoseconds(totalNanosSinceEpochBefore);

  igtlUint64 totalNanosSinceEpochAfter = ts->GetTimeStampInNanoseconds();
  if (totalNanosSinceEpochAfter != totalNanosSinceEpochBefore)
  {
    std::cerr << "Expected totalNanosSinceEpochBefore=" << totalNanosSinceEpochBefore << " to equal totalNanosSinceEpochAfter=" << totalNanosSinceEpochAfter << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}





